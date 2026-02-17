#include "vkCommandListPool.h"

#include <mutex>

#include "clogr.h"
#include "vkDevice.h"

namespace urhi
{
    VkCommandListPool::VkCommandListPool(VkDevice* device, const grl::Rc<VkQueueState> &queueState)
        : m_queueState(queueState)
    {
        m_device = device;
        vk::CommandPoolCreateInfo commandPoolInfo =  {};
        commandPoolInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
        commandPoolInfo.pNext = nullptr;
        commandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        commandPoolInfo.queueFamilyIndex = m_queueState->family;

        auto res = m_device->getHandle().createCommandPool(&commandPoolInfo, nullptr, &m_commandPool);
    }


    vk::CommandBuffer VkCommandListPool::acquire()
    {
        if (canReset())
        {
            m_device->getHandle().resetCommandPool(m_commandPool);
            m_nextBufferIndex = 0;
        }

        if (m_nextBufferIndex < m_commandBuffers.size())
        {
            return m_commandBuffers[m_nextBufferIndex++];
        }

        clogr::ensure(m_commandBuffers.size() < ERROR_THRESHOLD, "Too many command buffers allocated. You may have forgot to submit command buffers.");

        const vk::CommandBufferAllocateInfo info(
            m_commandPool,
            vk::CommandBufferLevel::ePrimary,
            1
        );

        vk::CommandBuffer cmd;
        auto res1 = m_device->getHandle().allocateCommandBuffers(&info, &cmd);
        m_commandBuffers.push_back(cmd);
        m_nextBufferIndex++;
        return cmd;
    }

    void VkCommandListPool::submit(const VkCommandList* cmd, const vk::Semaphore swapchainImageSemaphore)
    {
        m_recordingCount--;
        uint64_t signalValue = ++m_queueState->nextTimelineValue;
        m_lastSubmittedValue = signalValue;

        std::vector<vk::Semaphore> waitSemaphores;
        std::vector<vk::PipelineStageFlags> waitStages;
        std::vector<uint64_t> waitValues;

        if (swapchainImageSemaphore)
        {
            waitSemaphores.push_back(swapchainImageSemaphore);
            waitStages.push_back(vk::PipelineStageFlagBits::eColorAttachmentOutput);
            waitValues.push_back(0);
        }

        std::vector<uint64_t> signalValues = { signalValue };

        vk::TimelineSemaphoreSubmitInfo timelineInfo{};
        timelineInfo.waitSemaphoreValueCount = static_cast<uint32_t>(waitValues.size());
        timelineInfo.pWaitSemaphoreValues = waitValues.data();
        timelineInfo.signalSemaphoreValueCount = 1;
        timelineInfo.pSignalSemaphoreValues = signalValues.data();

        auto cmdBuffer = cmd->getCmdBuffer();
        vk::Semaphore signalSemaphore = m_queueState->timeline;

        vk::SubmitInfo submitInfo{};
        submitInfo.pNext = &timelineInfo;
        submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
        submitInfo.pWaitSemaphores = waitSemaphores.data();
        submitInfo.pWaitDstStageMask = waitStages.data();
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &signalSemaphore;

        std::scoped_lock lock(*m_queueState->mutex);
        m_queueState->queue.submit({submitInfo});
    }

    bool VkCommandListPool::canReset() const
    {
        if(m_recordingCount != 0 || m_lastSubmittedValue == 0) return false;

        uint64_t completedValue = 0;
        auto res = m_device->getHandle().getSemaphoreCounterValue(m_queueState->timeline, &completedValue);

        return completedValue >= m_lastSubmittedValue;
    }
}

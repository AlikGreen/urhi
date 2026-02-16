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

        vk::SemaphoreTypeCreateInfo typeCreateInfo{};
        typeCreateInfo.sType = vk::StructureType::eSemaphoreTypeCreateInfo;
        typeCreateInfo.semaphoreType = vk::SemaphoreType::eTimeline;
        typeCreateInfo.initialValue = 0;

        vk::SemaphoreCreateInfo createInfo{};
        createInfo.sType = vk::StructureType::eSemaphoreCreateInfo;
        createInfo.pNext = &typeCreateInfo;

        res = m_device->getHandle().createSemaphore(&createInfo, nullptr, &m_timeline);
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

    void VkCommandListPool::submit(const VkCommandList* cmd)
    {
        m_recordingCount--;
        uint64_t signalValue = ++m_nextBufferIndex;

        const vk::TimelineSemaphoreSubmitInfo timelineInfo(
            {},
            {signalValue}
        );

        auto cmdBuffer = cmd->getCmdBuffer();

        const vk::SubmitInfo submitInfo(
            {},
            {},
            {cmdBuffer},
            {m_timeline},
            &timelineInfo
        );

        std::scoped_lock lock(*m_queueState->mutex);
        auto res = m_queueState->queue.submit(1, &submitInfo, VK_NULL_HANDLE);
    }

    bool VkCommandListPool::canReset() const
    {
        if(m_recordingCount != 0 || m_nextBufferIndex == 0) return false;

        uint64_t completedValue = 0;
        auto res = m_device->getHandle().getSemaphoreCounterValue(m_timeline, &completedValue);

        return completedValue >= m_nextBufferIndex;
    }
}

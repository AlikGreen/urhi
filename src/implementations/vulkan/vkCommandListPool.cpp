#include "vkCommandListPool.h"

#include <mutex>

#include "vkDevice.h"

namespace urhi
{
    VkCommandListPool::VkCommandListPool(const uint32_t queueFamily, VkDevice* device)
    {
        m_device = device;
        vk::CommandPoolCreateInfo commandPoolInfo =  {};
        commandPoolInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
        commandPoolInfo.pNext = nullptr;
        commandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        commandPoolInfo.queueFamilyIndex = queueFamily;

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
        auto res = m_device->getHandle().getSemaphoreCounterValue(m_timeline, &m_completedSubmitCount);
        if (m_recordingCount == 0 &&
            m_submittedCount > 0 &&
            m_submittedCount <= m_completedSubmitCount)
        {
            m_device->getHandle().resetCommandPool(m_commandPool);
            m_submittedCount = 0;
            m_completedSubmitCount = 0;
            m_nextBufferIndex = 0;
        }

        if (m_nextBufferIndex < m_commandBuffers.size())
            return m_commandBuffers[m_nextBufferIndex++];

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
        m_submittedCount++;
        m_recordingCount--;

        const uint64_t signalValue = m_completedSubmitCount + 1;

        const vk::TimelineSemaphoreSubmitInfo timelineInfo(
            {},
            {signalValue}
        );

        auto cmdBuffer = cmd->getHandle();

        const vk::SubmitInfo submitInfo(
            {},
            {},
            {cmdBuffer},
            {m_timeline},
            &timelineInfo
        );


        const vk::Queue queue = m_device->getQueue(cmd->getQueueType());
        auto res = queue.submit(1, &submitInfo, VK_NULL_HANDLE);

        {
            std::scoped_lock lock(m_device->getQueueMutex(cmd->getQueueType()));
            res = queue.submit(1, &submitInfo, VK_NULL_HANDLE);
        }
    }
}

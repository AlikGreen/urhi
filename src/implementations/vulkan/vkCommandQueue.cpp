#include "vkCommandQueue.h"

#include <mutex>

#include "vkDevice.h"

namespace urhi
{
    VkCommandQueue::VkCommandQueue(VkDevice *device, QueueType type, uint32_t familyIndex, vk::Queue queue)
        : m_device(device), m_type(type),  m_queue(queue), m_family(familyIndex)
    {
        for(size_t i = 0; i < kMaxInFlight; i++)
        {
            m_contexts[i] = grl::makeBox<VkSubmissionContext>(device, familyIndex);
        }

        vk::SemaphoreTypeCreateInfo timelineCreateInfo{};
        timelineCreateInfo.semaphoreType = vk::SemaphoreType::eTimeline;
        timelineCreateInfo.initialValue = 0;

        vk::SemaphoreCreateInfo createInfo{};
        createInfo.pNext = &timelineCreateInfo;

        m_timelineSemaphore = m_device->handle().createSemaphore(createInfo);
    }

    VkCommandQueue::~VkCommandQueue()
    {
        if (m_timelineSemaphore)
            m_device->handle().destroySemaphore(m_timelineSemaphore);
    }

    void VkCommandQueue::submit(const vk::CommandBuffer cmd, VkSubmissionContext* context, const vk::Semaphore waitSwapchainSemaphore)
    {
        std::scoped_lock lock(m_mutex);

        const uint64_t signalValue = ++m_currentTimelineValue;

        // context cannot be reset until the GPU reaches this timeline value
        context->completionValue(signalValue);

        std::vector<vk::SemaphoreSubmitInfo> waitSemaphoreInfos;
        std::vector<vk::SemaphoreSubmitInfo> signalSemaphoreInfos;

        if (waitSwapchainSemaphore)
        {
            vk::SemaphoreSubmitInfo waitInfo{};
            waitInfo.semaphore = waitSwapchainSemaphore;
            waitInfo.value = 0; // binary semaphore
            waitInfo.stageMask = vk::PipelineStageFlagBits2::eAllCommands;
            waitSemaphoreInfos.push_back(waitInfo);
        }

        vk::SemaphoreSubmitInfo timelineSignalInfo{};
        timelineSignalInfo.semaphore = m_timelineSemaphore;
        timelineSignalInfo.value = signalValue;
        timelineSignalInfo.stageMask = vk::PipelineStageFlagBits2::eAllCommands;
        signalSemaphoreInfos.push_back(timelineSignalInfo);

        vk::CommandBufferSubmitInfo cmdInfo{};
        cmdInfo.commandBuffer = cmd;

        vk::SubmitInfo2 submitInfo{};
        submitInfo.waitSemaphoreInfoCount = static_cast<uint32_t>(waitSemaphoreInfos.size());
        submitInfo.pWaitSemaphoreInfos = waitSemaphoreInfos.data();
        submitInfo.commandBufferInfoCount = 1;
        submitInfo.pCommandBufferInfos = &cmdInfo;
        submitInfo.signalSemaphoreInfoCount = static_cast<uint32_t>(signalSemaphoreInfos.size());
        submitInfo.pSignalSemaphoreInfos = signalSemaphoreInfos.data();

        m_queue.submit2({submitInfo});

        m_contextIndex = (m_contextIndex + 1) % kMaxInFlight;
    }

    vk::CommandBuffer VkCommandQueue::acquireCommandBuffer()
    {
        std::scoped_lock lock(m_mutex);

        const auto& context = m_contexts[m_contextIndex];

        const uint64_t waitValue = context->completionValue();
        if (waitValue > 0)
        {
            vk::SemaphoreWaitInfo waitInfo{};
            waitInfo.semaphoreCount = 1;
            waitInfo.pSemaphores = &m_timelineSemaphore;
            waitInfo.pValues = &waitValue;

            auto res = m_device->handle().waitSemaphores(waitInfo, UINT64_MAX);
        }

        context->reset();

        return context->allocateCommandBuffer();
    }

    VkSubmissionContext & VkCommandQueue::submissionContext() const
    {
        return *m_contexts[m_contextIndex];
    }
}

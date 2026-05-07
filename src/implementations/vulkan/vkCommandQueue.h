#pragma once
#include <mutex>

#include "vkSubmissionContext.h"
#include "enums/queueType.h"

namespace urhi
{
class VkDevice;
class VkCommandQueue
{
public:
    VkCommandQueue(VkDevice* device, QueueType type, uint32_t familyIndex, vk::Queue queue);
    ~VkCommandQueue();

    void submit(vk::CommandBuffer cmd, VkSubmissionContext *context, vk::Semaphore waitSwapchainSemaphore);
    vk::CommandBuffer acquireCommandBuffer();

    [[nodiscard]] VkSubmissionContext& submissionContext() const;

    [[nodiscard]] vk::Queue handle() const { return m_queue; }
    [[nodiscard]] uint32_t family() const { return m_family; }
    [[nodiscard]] QueueType type() const { return m_type; }
    [[nodiscard]] std::mutex& mutex() { return m_mutex; }

    [[nodiscard]] uint64_t timelineValue() const { return m_currentTimelineValue; }
    uint64_t incrementTimeline() { return ++m_currentTimelineValue; }

    [[nodiscard]] vk::Semaphore timelineSemaphore() const { return m_timelineSemaphore; }
private:
    static constexpr uint32_t kMaxInFlight = 4;
    VkDevice* m_device;
    QueueType m_type;
    vk::Queue m_queue;
    uint32_t m_family;

    std::mutex m_mutex;
    vk::Semaphore m_timelineSemaphore;
    uint64_t m_currentTimelineValue = 0;

    uint32_t m_contextIndex = 0;
    std::array<grl::Box<VkSubmissionContext>, kMaxInFlight> m_contexts;
};
}

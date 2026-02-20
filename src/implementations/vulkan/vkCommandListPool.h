#pragma once
#include <vulkan/vulkan.hpp>

#include "vkCommandList.h"
#include "vkLinearStagingAllocator.h"

namespace urhi
{
class VkDevice;
class VkLinearStagingAllocator;
struct VkQueueState;

class VkCommandListPool
{
public:
    VkCommandListPool(VkDevice *device, const grl::Rc<VkQueueState> &queueState);

    vk::CommandBuffer acquire();
    void submit(VkCommandList* cmd, vk::Semaphore swapchainImageSemaphore);
private:
    friend VkCommandList;

    [[nodiscard]] bool canReset() const;

    static constexpr size_t WARN_THRESHOLD = 64;
    static constexpr size_t ERROR_THRESHOLD = 256;

    VkDevice* m_device{};
    vk::CommandPool m_commandPool;
    std::vector<vk::CommandBuffer> m_commandBuffers;
    grl::Rc<VkQueueState> m_queueState;
    grl::Box<VkLinearStagingAllocator> m_linearStagingAllocator;

    uint32_t m_recordingCount = 0;
    uint64_t m_submittedCount = 0;
    uint32_t m_nextBufferIndex = 0;
    uint32_t m_lastSubmittedValue = 0;
};
}

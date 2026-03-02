#pragma once
#include <vulkan/vulkan.hpp>

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
    void submit(vk::CommandBuffer cmd, vk::Semaphore swapchainImageSemaphore);
private:
    friend VkCommandList;
    friend VkDevice;

    [[nodiscard]] bool canReset() const;

    static constexpr size_t kWarnThreshold = 64;
    static constexpr size_t kErrorThreshold = 256;

    VkDevice* m_device{};
    vk::CommandPool m_commandPool;
    std::vector<vk::CommandBuffer> m_commandBuffers;
    grl::Rc<VkQueueState> m_queueState;
    grl::Rc<VkLinearStagingAllocator> m_linearStagingAllocator;

    uint64_t m_submittedCount = 0;
    uint32_t m_nextBufferIndex = 0;
    uint32_t m_lastSubmittedValue = 0;
};
}

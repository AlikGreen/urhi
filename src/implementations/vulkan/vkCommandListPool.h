#pragma once
#include <vulkan/vulkan.hpp>

#include "vkCommandList.h"

namespace urhi
{
class VkDevice;
struct VkQueueState;
class VkCommandListPool
{
public:
    VkCommandListPool(VkDevice *device, const grl::Rc<VkQueueState> &queueState);

    vk::CommandBuffer acquire();
    void submit(const VkCommandList* cmd);
private:
    friend VkCommandList;

    [[nodiscard]] bool canReset() const;

    static constexpr size_t WARN_THRESHOLD = 64;
    static constexpr size_t ERROR_THRESHOLD = 256;

    VkDevice* m_device{};
    vk::CommandPool m_commandPool;
    std::vector<vk::CommandBuffer> m_commandBuffers;
    grl::Rc<VkQueueState> m_queueState;
    vk::Semaphore m_timeline;

    uint32_t m_recordingCount = 0;
    uint64_t m_submittedCount = 0;
    uint32_t m_nextBufferIndex = 0;
};
}

#pragma once
#include <vulkan/vulkan.hpp>

#include "vkCommandList.h"

namespace urhi
{
class VkDevice;

class VkCommandListPool
{
public:
    VkCommandListPool() = default;
    VkCommandListPool(uint32_t queueFamily, VkDevice *device);

    vk::CommandBuffer acquire();
    void submit(const VkCommandList* cmd);
private:
    friend VkCommandList;
    VkDevice* m_device{};
    vk::CommandPool m_commandPool;
    std::vector<vk::CommandBuffer> m_commandBuffers;
    vk::Semaphore m_timeline;

    uint32_t m_recordingCount = 0;
    uint32_t m_submittedCount = 0;
    uint32_t m_nextBufferIndex = 0;
    uint64_t m_completedSubmitCount = 0;
};
}

#pragma once
#include "readbackRequest.h"

#include <vulkan/vulkan.hpp>

#include "vkLinearStagingAllocator.h"

namespace urhi
{
class VkDevice;
class VkReadbackRequest final : public ReadbackRequest
{
public:
    VkReadbackRequest(VkDevice* device, void* mapped, size_t size, vk::Buffer buffer, VmaAllocation bufferAllocation);
    ~VkReadbackRequest() override;
    bool isReady() const override;
    void wait() const override;

    const void* data() const override;
    size_t size() const override;
private:
    friend class VkCommandList;
    VkDevice* m_device;

    void* m_mapped;
    size_t m_size;

    VmaAllocation m_bufferAllocation;
    vk::Buffer m_buffer;

    vk::Semaphore m_timeline = VK_NULL_HANDLE;
    uint64_t m_waitValue = ~0u;
};
}

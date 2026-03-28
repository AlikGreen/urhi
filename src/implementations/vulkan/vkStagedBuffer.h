#pragma once
#include <vulkan/vulkan.hpp>

#include "vkBuffer.h"
#include "vkLinearStagingAllocator.h"
#include "descriptions/bufferDesc.h"

namespace urhi
{
class VkCommandList;
class VkDevice;

class VkStagedBuffer final : public VkBuffer
{
public:
    explicit VkStagedBuffer(VkDevice* device, BufferDesc desc);
    ~VkStagedBuffer() override;

    void barrier(vk::CommandBuffer cmd) const;

    vk::Buffer handle() const override;
    uint64_t size() const override;
    VkLifetime& lifetime() override;
private:
    VkDevice* m_device;
    VmaAllocation m_allocation;
    vk::Buffer m_buffer{};
    uint64_t m_bufferSize;

    BufferUsage m_usage;

    VkLifetime m_life{};
};
}

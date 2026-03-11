#pragma once
#include <vulkan/vulkan.hpp>

#include "vkBuffer.h"
#include "vkLinearStagingAllocator.h"
#include "descriptions/bufferDesc.h"

namespace urhi
{
class VkDevice;
class VkMappedBuffer final : public VkBuffer
{
public:
    VkMappedBuffer(VkDevice* device, const BufferDesc& desc);
    ~VkMappedBuffer() override;

    void upload(const void* data, size_t size) const;
    [[nodiscard]] vk::Buffer handle() const override;
    [[nodiscard]] uint64_t size() const override;

    VkLifetime& lifetime() override;
private:
    VkDevice* m_device;

    vk::Buffer m_buffer;
    uint64_t m_size;
    VmaAllocation m_allocation{};
    bool m_hostCoherent;
    void* m_mappedPtr;

    VkLifetime m_life;
};
}

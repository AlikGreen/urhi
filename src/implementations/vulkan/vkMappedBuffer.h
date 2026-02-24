#pragma once
#include <vulkan/vulkan.hpp>

#include "buffer.h"
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

    void upload(const void* data, size_t size) const;
    vk::Buffer getHandle() const override;
    uint64_t getSize() const override;
private:
    VkDevice* m_device;

    vk::Buffer m_buffer;
    uint64_t m_size;
    VmaAllocation m_allocation;
    bool m_hostCoherent;
    void* m_mappedPtr;
};
}

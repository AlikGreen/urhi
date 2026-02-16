#pragma once
#include <grl/mem.h>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include "buffer.h"
#include "vkStagingBufferPool.h"
#include "descriptions/bufferDesc.h"

namespace urhi
{
class VkCommandList;
class VkDevice;

class VkStagedBuffer final : public Buffer
{
public:
    explicit VkStagedBuffer(VkDevice* device, BufferDesc desc);

    void write(const void* data, size_t size, size_t offset, const VkCommandList* cmd) const;
private:
    VkDevice* m_device;
    VmaAllocation m_allocation{};
    vk::Buffer m_buffer{};
    uint64_t m_bufferSize;
};
}

#pragma once
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include "buffer.h"
#include "descriptions/bufferDesc.h"

namespace urhi
{
class VkCommandList;
class VkDevice;

class VkStagedBuffer final : public Buffer
{
public:
    explicit VkStagedBuffer(VkDevice* device, BufferDesc desc);

    vk::Buffer getHandle() const;
    uint64_t getSize() const;
private:
    VkDevice* m_device;
    VmaAllocation m_allocation{};
    vk::Buffer m_buffer{};
    uint64_t m_bufferSize;
};
}

#include "vkStagedBuffer.h"
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "clogr.h"
#include "vkConvert.h"
#include "vkDevice.h"

namespace urhi
{
    VkStagedBuffer::VkStagedBuffer(VkDevice* device, const BufferDesc desc)
        : m_device(device), m_bufferSize(desc.size)
    {
        const VkBufferCreateInfo bufferCI{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = desc.size,
            .usage = static_cast<VkBufferUsageFlags>(VkConvert::bufferUsage(desc.usage))
        };

        constexpr VmaAllocationCreateInfo bufferAllocCI{
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                     VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO
        };

        vmaCreateBuffer(device->getAllocator(), &bufferCI, &bufferAllocCI,reinterpret_cast<VkBuffer *>(&m_buffer), &m_allocation, nullptr);
    }

    void VkStagedBuffer::write(const void *data, const size_t size, const size_t offset, const VkCommandList* cmd) const
    {
        clogr::ensure(offset + size <= m_bufferSize, "Write exceeds buffer size");

        m_device->getStagingBufferPool()->upload(data, size, m_buffer, offset, cmd->getCmdBuffer());
    }
}

#include "vkStagedBuffer.h"
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "clogr.h"
#include "vkConvert.h"
#include "vkDevice.h"

namespace urhi
{
    VkStagedBuffer::VkStagedBuffer(VkDevice* device, const BufferDesc desc)
        : m_device(device), m_allocation(nullptr), m_bufferSize(desc.size)
    {
        const vk::BufferCreateInfo bufferCI{
            {},
            desc.size,
            VkConvert::bufferUsage(desc.usage) | vk::BufferUsageFlagBits::eTransferDst
        };

        constexpr VmaAllocationCreateInfo bufferAllocCI
        {
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                     VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT,
            .usage = VMA_MEMORY_USAGE_GPU_ONLY
        };

        vmaCreateBuffer(device->getAllocator(), reinterpret_cast<const VkBufferCreateInfo *>(&bufferCI), &bufferAllocCI,
                        reinterpret_cast<::VkBuffer *>(&m_buffer), &m_allocation, nullptr);
    }

    VkStagedBuffer::~VkStagedBuffer()
    {
        m_device->queueDestroy(m_life,
        [buf = m_buffer, alloc = m_allocation](const VkDevice* device)
        {
            vmaDestroyBuffer(device->getAllocator(), buf, alloc);
        });
    }

    VkLifetime& VkStagedBuffer::lifetime()
    {
        return m_life;
    }

    vk::Buffer VkStagedBuffer::handle() const
    {
        return m_buffer;
    }

    uint64_t VkStagedBuffer::size() const
    {
        return m_bufferSize;
    }
}

#include "vkMappedBuffer.h"

#include "validation.h"
#include "vkConvert.h"
#include "vkDevice.h"
#include "vkLinearStagingAllocator.h"

namespace urhi
{
    VkMappedBuffer::VkMappedBuffer(VkDevice *device, const BufferDesc &desc)
        : m_device(device), m_size(desc.size)
    {
        const vk::BufferCreateInfo bufferCI
        {
            {},
            desc.size,
            VkConvert::bufferUsage(desc.usage)
        };

        VmaAllocationCreateInfo bufferAllocCI{};
        bufferAllocCI.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        bufferAllocCI.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        auto res = vmaCreateBuffer(device->getAllocator(), reinterpret_cast<const VkBufferCreateInfo*>(&bufferCI), &bufferAllocCI, reinterpret_cast<::VkBuffer*>(&m_buffer), &m_allocation, nullptr);
        URHI_VALIDATE(res == VK_SUCCESS, "Failed to create buffer allocation - vmaCreateBuffer returned {}", vk::to_string(static_cast<vk::Result>(res)));

        VmaAllocationInfo allocInfo;
        vmaGetAllocationInfo(device->getAllocator(), m_allocation, &allocInfo);
        m_mappedPtr = allocInfo.pMappedData;

        VkMemoryPropertyFlags memFlags;
        vmaGetAllocationMemoryProperties(device->getAllocator(), m_allocation, &memFlags);

        m_hostCoherent = memFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }

    VkMappedBuffer::~VkMappedBuffer()
    {
        m_device->queueDestroy(m_life,
                               [h = m_buffer](const vk::Device device)
                               {
                                   device.destroyBuffer(h);
                               });
    }

    void VkMappedBuffer::upload(const void *data, const size_t size) const
    {
        memcpy(m_mappedPtr, data, size);

        if (!m_hostCoherent)
        {
            auto res = vmaFlushAllocation(m_device->getAllocator(), m_allocation, 0, size);
            URHI_VALIDATE(res == VK_SUCCESS, "Failed to flush buffer allocation - vmaFlushAllocation buffer returned {}", vk::to_string(static_cast<vk::Result>(res)));
        }
    }

    vk::Buffer VkMappedBuffer::handle() const
    {
        return m_buffer;
    }

    uint64_t VkMappedBuffer::size() const
    {
        return m_size;
    }

    VkLifetime & VkMappedBuffer::lifetime()
    {
        return m_life;
    }
}

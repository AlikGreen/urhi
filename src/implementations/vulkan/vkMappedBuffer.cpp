#include "vkMappedBuffer.h"

#include "vkConvert.h"
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

        vmaCreateBuffer(device->getAllocator(), reinterpret_cast<const VkBufferCreateInfo*>(&bufferCI), &bufferAllocCI, reinterpret_cast<::VkBuffer*>(&m_buffer), &m_allocation, nullptr);

        VmaAllocationInfo allocInfo;
        vmaGetAllocationInfo(device->getAllocator(), m_allocation, &allocInfo);
        m_mappedPtr = allocInfo.pMappedData;

        VkMemoryPropertyFlags memFlags;
        vmaGetAllocationMemoryProperties(device->getAllocator(), m_allocation, &memFlags);

        m_hostCoherent = !(memFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }

    void VkMappedBuffer::upload(const void *data, const size_t size) const
    {
        memcpy(m_mappedPtr, data, size);

        if (!m_hostCoherent)
            vmaFlushAllocation(m_device->getAllocator(), m_allocation, 0, size);
    }

    vk::Buffer VkMappedBuffer::getHandle() const
    {
        return m_buffer;
    }

    uint64_t VkMappedBuffer::getSize() const
    {
        return m_size;
    }
}

#include "vkStagingBufferPool.h"

#include "clogr.h"
#include "vkDevice.h"


namespace urhi
{
    VkStagingBufferPool::VkStagingBufferPool(VkDevice *device)
        : m_device(device) { }

    void VkStagingBufferPool::upload(const void *srcData, const size_t size, vk::Buffer dstBuffer, const size_t dstOffset, vk::CommandBuffer cmd)
    {
        PoolBuffer* staging = acquireBuffer(size);
        memcpy(staging->mapped, srcData, size);

        const vk::BufferCopy region
        {
            0,
            dstOffset,
            size
        };
        cmd.copyBuffer(staging->buffer, dstBuffer, 1, &region);
    }

    VkStagingBufferPool::PoolBuffer* VkStagingBufferPool::acquireBuffer(const size_t size)
    {
        for (auto& buf : m_buffers)
        {
            if(buf.size < size) continue;

            uint64_t completedValue = 0;
            // auto res = m_device->getHandle().getSemaphoreCounterValue(
            //     m_timeline, &completedValue
            // );

            if (completedValue >= buf.timelineValue)
            {
                return &buf;
            }
        }

        clogr::ensure(m_buffers.size() < 32, "Too many staging buffers created this is an internal fault of urhi and not your program");

        PoolBuffer newBuf;
        const VkBufferCreateInfo bufferInfo =
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = std::max(size, 1024ull * 1024ull),
            .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        };

        constexpr VmaAllocationCreateInfo allocInfo =
        {
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                     VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO
        };

        VmaAllocationInfo allocResult;
        vmaCreateBuffer(m_device->getAllocator(), &bufferInfo, &allocInfo,
                       reinterpret_cast<VkBuffer*>(&newBuf.buffer), &newBuf.allocation, &allocResult);
        newBuf.mapped = allocResult.pMappedData;
        newBuf.size = bufferInfo.size;
        newBuf.timelineValue = 0;

        m_buffers.push_back(newBuf);
        return &m_buffers.back();
    }
}

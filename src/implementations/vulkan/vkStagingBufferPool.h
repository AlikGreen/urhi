#pragma once

#include <grl/mem.h>
#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

namespace urhi
{
class VkCommandList;
class VkDevice;

class VkStagingBufferPool
{
public:
    explicit VkStagingBufferPool(VkDevice* device);
    void upload(const void* srcData, size_t size, vk::Buffer dstBuffer, size_t dstOffset, vk::CommandBuffer cm);
private:
    struct PoolBuffer
    {
        vk::Buffer buffer;
        VmaAllocation allocation;
        void* mapped;
        uint64_t timelineValue;
        size_t size;
    };

    PoolBuffer* acquireBuffer(size_t size);

    VkDevice* m_device;
    std::vector<PoolBuffer> m_buffers;
    uint64_t m_nextTimelineValue = 0;
};
}

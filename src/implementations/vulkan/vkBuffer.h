#pragma once
#include <vulkan/vulkan.hpp>
#include "buffer.h"
#include "vkLifetime.h"

namespace urhi
{
class VkBuffer : public Buffer
{
public:
    ~VkBuffer() override = default;
    [[nodiscard]] virtual vk::Buffer handle() const = 0;
    virtual VkLifetime& lifetime() = 0;

    void barrier(const vk::CommandBuffer cmd, const vk::PipelineStageFlags2 srcStage, const vk::AccessFlags2 srcAccess,
                         const vk::PipelineStageFlags2 dstStage, const vk::AccessFlags2 dstAccess) const
    {
        vk::BufferMemoryBarrier2 barrier{};
        barrier.buffer = handle();
        barrier.offset = 0;
        barrier.size = size();

        barrier.srcStageMask = srcStage;
        barrier.srcAccessMask = srcAccess;

        barrier.dstStageMask = dstStage;
        barrier.dstAccessMask = dstAccess;

        vk::DependencyInfo depInfo{};
        depInfo.bufferMemoryBarrierCount = 1;
        depInfo.pBufferMemoryBarriers = &barrier;

        cmd.pipelineBarrier2(depInfo);
    }
};
}

#include "vkStagedBuffer.h"
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "clogr.h"
#include "vkConvert.h"
#include "vkDevice.h"

namespace urhi
{
    VkStagedBuffer::VkStagedBuffer(VkDevice* device, const BufferDesc desc)
        : m_device(device), m_allocation(nullptr), m_bufferSize(desc.size), m_usage(desc.usage)
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

    void VkStagedBuffer::barrier(const vk::CommandBuffer cmd) const
    {
        vk::BufferMemoryBarrier2 barrier{};
        barrier.buffer = m_buffer;
        barrier.offset = 0;
        barrier.size = m_bufferSize;

        barrier.srcStageMask = vk::PipelineStageFlagBits2::eTransfer;
        barrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;

        switch (m_usage)
        {
            case BufferUsage::Vertex:
                barrier.dstStageMask  = vk::PipelineStageFlagBits2::eVertexAttributeInput;
                barrier.dstAccessMask = vk::AccessFlagBits2::eVertexAttributeRead;
                break;

            case BufferUsage::Index:
                barrier.dstStageMask  = vk::PipelineStageFlagBits2::eIndexInput;
                barrier.dstAccessMask = vk::AccessFlagBits2::eIndexRead;
                break;

            case BufferUsage::Uniform:
                barrier.dstStageMask  = vk::PipelineStageFlagBits2::eAllGraphics |
                                        vk::PipelineStageFlagBits2::eComputeShader;
                barrier.dstAccessMask = vk::AccessFlagBits2::eUniformRead;
                break;

            case BufferUsage::ShaderStorage:
                barrier.dstStageMask  = vk::PipelineStageFlagBits2::eAllGraphics |
                                        vk::PipelineStageFlagBits2::eComputeShader;
                barrier.dstAccessMask = vk::AccessFlagBits2::eShaderStorageRead |
                                        vk::AccessFlagBits2::eShaderStorageWrite;
                break;

            default:
                barrier.dstStageMask  = vk::PipelineStageFlagBits2::eAllCommands;
                barrier.dstAccessMask = vk::AccessFlagBits2::eMemoryRead;
                break;
        }

        vk::DependencyInfo depInfo{};
        depInfo.bufferMemoryBarrierCount = 1;
        depInfo.pBufferMemoryBarriers = &barrier;

        cmd.pipelineBarrier2(depInfo);
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

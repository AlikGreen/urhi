#include "vkCommandList.h"

#include <utility>

#include "clogr.h"
#include "renderPass.h"
#include "vkCommandListPool.h"
#include "vkComputePass.h"
#include "vkConvert.h"
#include "vkDevice.h"
#include "vkMappedBuffer.h"
#include "vkStagedBuffer.h"
#include "vkRenderPass.h"
#include "vkTexture.h"
#include "vkTextureView.h"

namespace urhi
{
    VkCommandList::VkCommandList(VkDevice* device, VkCommandListPool* pool, const QueueType queueType, const vk::CommandBuffer commandBuffer)
        : m_device(device),  m_cmd(commandBuffer), m_queueType(queueType), m_pool(pool)
    {

    }

    void VkCommandList::begin()
    {
        constexpr vk::CommandBufferBeginInfo info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        m_cmd.begin(info);
        m_pool->m_recordingCount++;
        m_readbackRequests.clear();
    }

    grl::Rc<RenderPass> VkCommandList::beginRenderPass(const RenderPassDesc &desc)
    {
        return grl::makeRc<VkRenderPass>(m_device, m_cmd, desc);
    }

    grl::Rc<ComputePass> VkCommandList::beginComputePass()
    {
        return grl::makeRc<VkComputePass>(m_device, m_cmd);
    }

    void VkCommandList::updateTexture(const TextureUploadDesc &desc)
    {
        const auto vkTex = dynamic_cast<VkTexture*>(desc.texture.get());
        m_pool->m_linearStagingAllocator->uploadToImage(desc, vkTex, m_cmd);
    }

    void VkCommandList::generateMipmaps(const grl::Rc<Texture> &texture)
    {
        const uint32_t mipLevels = texture->mipLevelCount();

        const vk::FormatProperties formatProperties = m_device->getPhysicalDevice().getFormatProperties(VkConvert::pixelFormat(texture->format()));
        clogr::ensure(static_cast<bool>(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear), "Texture format is not blitable (so cannot generate mipmaps) if mipmaps are needed generate them manually");

        const auto vkTex = dynamic_cast<VkTexture*>(texture.get());
        int32_t mipWidth = vkTex->width();
        int32_t mipHeight = vkTex->height();

        vkTex->transitionLayout(m_cmd, vk::ImageLayout::eTransferDstOptimal);

        for (uint32_t i = 1; i < mipLevels; i++)
        {
            vk::ImageMemoryBarrier2 srcBarrier{
                vk::PipelineStageFlagBits2::eTransfer,
                vk::AccessFlagBits2::eTransferWrite,
                vk::PipelineStageFlagBits2::eTransfer,
                vk::AccessFlagBits2::eTransferRead,
                vk::ImageLayout::eTransferDstOptimal,
                vk::ImageLayout::eTransferSrcOptimal,
                VK_QUEUE_FAMILY_IGNORED,
                VK_QUEUE_FAMILY_IGNORED,
                vkTex->getHandle(),
                { vk::ImageAspectFlagBits::eColor, i - 1, 1, 0, vkTex->arrayLayerCount() }
            };

            m_cmd.pipelineBarrier2(vk::DependencyInfo{}.setImageMemoryBarriers(srcBarrier));

            vk::ImageBlit2 blit{};
            blit.srcOffsets = std::array{ vk::Offset3D{ 0, 0, 0 }, vk::Offset3D{ mipWidth, mipHeight, 1 } };
            blit.srcSubresource = vk::ImageSubresourceLayers
            {
                vk::ImageAspectFlagBits::eColor,
                i - 1,
                0,
                vkTex->arrayLayerCount()
            };

            blit.dstOffsets = std::array{ vk::Offset3D{ 0, 0, 0 }, vk::Offset3D{ mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 } };
            blit.dstSubresource = vk::ImageSubresourceLayers
            {
                vk::ImageAspectFlagBits::eColor,
                i,
                0,
                vkTex->arrayLayerCount()
            };

            vk::BlitImageInfo2 blitInfo
            {
                vkTex->getHandle(),
                vk::ImageLayout::eTransferSrcOptimal,
                vkTex->getHandle(),
                vk::ImageLayout::eTransferDstOptimal,
                1u,
                &blit,
                vk::Filter::eLinear
            };

            m_cmd.blitImage2(blitInfo);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;

            vk::ImageMemoryBarrier2 dstBarrier{
                vk::PipelineStageFlagBits2::eTransfer,
                vk::AccessFlagBits2::eTransferWrite,
                vk::PipelineStageFlagBits2::eTransfer,
                vk::AccessFlagBits2::eTransferRead,
                vk::ImageLayout::eTransferSrcOptimal,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                VK_QUEUE_FAMILY_IGNORED,
                VK_QUEUE_FAMILY_IGNORED,
                vkTex->getHandle(),
                { vk::ImageAspectFlagBits::eColor, i - 1, 1, 0, vkTex->arrayLayerCount() }
            };

            m_cmd.pipelineBarrier2(vk::DependencyInfo{}.setImageMemoryBarriers(dstBarrier));
        }

        vk::ImageMemoryBarrier2 dstBarrier{
            vk::PipelineStageFlagBits2::eTransfer,
            vk::AccessFlagBits2::eTransferWrite,
            vk::PipelineStageFlagBits2::eTransfer,
            vk::AccessFlagBits2::eTransferRead,
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eShaderReadOnlyOptimal,
            VK_QUEUE_FAMILY_IGNORED,
            VK_QUEUE_FAMILY_IGNORED,
            vkTex->getHandle(),
            { vk::ImageAspectFlagBits::eColor, mipLevels - 1, 1, 0, vkTex->arrayLayerCount() }
        };

        m_cmd.pipelineBarrier2(vk::DependencyInfo{}.setImageMemoryBarriers(dstBarrier));

        vkTex->m_currentLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    }

    grl::Rc<ReadbackRequest> VkCommandList::readback(const TextureReadbackDesc &desc)
    {
        const auto vkTex = dynamic_cast<VkTexture*>(desc.texture.get());
        const uint32_t width = std::min(vkTex->width(), desc.width);
        const uint32_t height = std::min(vkTex->height(), desc.height);
        const uint32_t depth = std::min(vkTex->depth(), desc.depth);

        vk::BufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = desc.mipLevel;
        region.imageSubresource.baseArrayLayer = desc.baseArrayLayer;
        region.imageSubresource.layerCount = desc.arrayLayerCount;
        region.imageOffset = vk::Offset3D{ desc.x, desc.y, desc.z };
        region.imageExtent = vk::Extent3D{ width, height, depth };

        const uint32_t size = width * height * depth * VkConvert::pixelFormatBytes(vkTex->format());

        // create buffer
        const VkBufferCreateInfo bufferInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size,
            .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT
        };

        constexpr VmaAllocationCreateInfo allocInfo = {
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                     VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO
        };

        VmaAllocation allocation;
        VmaAllocationInfo allocationResult;
        vk::Buffer buffer;

        vmaCreateBuffer(
            m_device->getAllocator(),
            &bufferInfo,
            &allocInfo,
            reinterpret_cast<::VkBuffer*>(&buffer),
            &allocation,
            &allocationResult
        );

        vkTex->transitionLayout(m_cmd, vk::ImageLayout::eTransferSrcOptimal);

        m_cmd.copyImageToBuffer(
        vkTex->getHandle(),
            vk::ImageLayout::eTransferSrcOptimal,
           buffer,
           1,
           &region
       );

        vkTex->transitionLayout(m_cmd, vk::ImageLayout::eShaderReadOnlyOptimal);

        auto request = grl::makeRc<VkReadbackRequest>(m_device, allocationResult.pMappedData, size, buffer, allocation);
        m_readbackRequests.push_back(request);
        return request;
    }

    void VkCommandList::onSubmit(const uint64_t submittedValue)
    {
        const auto timeline = m_device->getQueueState(m_queueType)->timeline;

        for(const auto readback : m_readbackRequests)
        {
            readback->m_timeline = timeline;
            readback->m_waitValue = submittedValue;
        }
    }

    vk::CommandBuffer VkCommandList::getCmdBuffer() const
    {
        return m_cmd;
    }

    QueueType VkCommandList::getQueueType() const
    {
        return m_queueType;
    }

    VkCommandListPool& VkCommandList::getPool() const
    {
        return *m_pool;
    }

    void VkCommandList::updateBufferImpl(const grl::Rc<Buffer> &buffer, void *data, const uint32_t size)
    {
        if(const auto vkStaged = dynamic_cast<VkStagedBuffer*>(buffer.get()))
        {
            m_pool->m_linearStagingAllocator->upload(data, size, vkStaged->getHandle(), 0, m_cmd);
        }
        else if(const auto vkMapped = dynamic_cast<VkMappedBuffer*>(buffer.get()))
        {
            vkMapped->upload(data, size);
        }else
        {
            clogr::abort("Buffer was not a VkStagedBuffer or VkMappedBuffer");
        }
    }
}

#include "vkCommandList.h"

#include <utility>

#include "clogr.h"
#include "renderPass.h"
#include "vkConvert.h"
#include "vkDevice.h"
#include "vkMappedBuffer.h"
#include "vkStagedBuffer.h"
#include "vkRenderPass.h"
#include "vkTexture.h"

namespace urhi
{
    VkCommandList::VkCommandList(VkDevice* device, VkCommandListPool* pool, const QueueType queueType, const vk::CommandBuffer commandBuffer)
        : m_commandBuffer(commandBuffer),  m_queueType(queueType), m_pool(pool), m_device(device)
    {

    }

    void VkCommandList::begin()
    {
        constexpr vk::CommandBufferBeginInfo info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        m_commandBuffer.begin(info);
        m_pool->m_recordingCount++;
    }

    grl::Rc<RenderPass> VkCommandList::beginRenderPass(const RenderPassDesc &desc)
    {
        return grl::makeRc<VkRenderPass>(m_device, m_commandBuffer, desc);
    }

    void VkCommandList::updateTexture(const grl::Rc<Texture> &texture, const TextureUploadDesc &desc)
    {
        const auto vkTex = dynamic_cast<VkTexture*>(texture.get());
        m_pool->m_linearStagingAllocator->uploadToImage(desc, vkTex, m_commandBuffer);
    }

    void VkCommandList::generateMipmaps(const grl::Rc<Texture> &texture)
    {
        const uint32_t mipLevels = texture->getMipLevels();

        const vk::FormatProperties formatProperties = m_device->getPhysicalDevice().getFormatProperties(VkConvert::pixelFormat(texture->getFormat()));
        clogr::ensure(static_cast<bool>(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear), "Texture format is not blitable (so cannot generate mipmaps) if mipmaps are needed generate them manually");

        const auto vkTex = dynamic_cast<VkTexture*>(texture.get());
        int32_t mipWidth = vkTex->getWidth();
        int32_t mipHeight = vkTex->getHeight();

        vkTex->transitionLayout(m_commandBuffer, vk::ImageLayout::eTransferDstOptimal);

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
                { vk::ImageAspectFlagBits::eColor, i - 1, 1, 0, vkTex->getArrayLayers() }
            };

            m_commandBuffer.pipelineBarrier2(vk::DependencyInfo{}.setImageMemoryBarriers(srcBarrier));

            vk::ImageBlit2 blit{};
            blit.srcOffsets = std::array{ vk::Offset3D{ 0, 0, 0 }, vk::Offset3D{ mipWidth, mipHeight, 1 } };
            blit.srcSubresource = vk::ImageSubresourceLayers
            {
                vk::ImageAspectFlagBits::eColor,
                i - 1,
                0,
                vkTex->getArrayLayers()
            };

            blit.dstOffsets = std::array{ vk::Offset3D{ 0, 0, 0 }, vk::Offset3D{ mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 } };
            blit.dstSubresource = vk::ImageSubresourceLayers
            {
                vk::ImageAspectFlagBits::eColor,
                i,
                0,
                vkTex->getArrayLayers()
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

            m_commandBuffer.blitImage2(blitInfo);

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
                { vk::ImageAspectFlagBits::eColor, i - 1, 1, 0, vkTex->getArrayLayers() }
            };

            m_commandBuffer.pipelineBarrier2(vk::DependencyInfo{}.setImageMemoryBarriers(dstBarrier));
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
            { vk::ImageAspectFlagBits::eColor, mipLevels - 1, 1, 0, vkTex->getArrayLayers() }
        };

        m_commandBuffer.pipelineBarrier2(vk::DependencyInfo{}.setImageMemoryBarriers(dstBarrier));

        vkTex->m_currentLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    }

    vk::CommandBuffer VkCommandList::getCmdBuffer() const
    {
        return m_commandBuffer;
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
            m_pool->m_linearStagingAllocator->upload(data, size, vkStaged->getHandle(), 0, m_commandBuffer);
        }
        else if(const auto vkMapped = dynamic_cast<VkMappedBuffer*>(buffer.get()))
        {
            vkMapped->upload(data, size);
        }else
        {
            clogr::abort("Buffer was not a VkStagedBuffer or VkMappedBuffer");
        }
    }

    void VkCommandList::readTextureImpl(const grl::Rc<TextureView> &texture, const TextureReadDesc &desc, size_t destSize, void *dest)
    {
        clogr::ensure(false, "not implemented");
    }
}

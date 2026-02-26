#include "vkTexture.h"

#include "vkConvert.h"
#include "vkDevice.h"

namespace urhi
{
    VkTexture::VkTexture(VkDevice* device, const TextureDesc &desc)
        : m_width(desc.width), m_height(desc.height), m_depth(desc.depth),
        m_arrayLayers(desc.arrayLayers),
        m_format(desc.format), m_type(desc.type),
        m_device(device)
    {
        m_mipLevels = std::min(desc.maxMipLevels, static_cast<uint32_t>(std::floor(std::log2(std::max(m_width, m_height)))) + 1);

        vk::ImageCreateInfo imageInfo
        {
            vk::ImageCreateFlags{0},
            VkConvert::textureType(desc.type),
            VkConvert::pixelFormat(desc.format),
            {desc.width, desc.height, desc.depth},
            m_mipLevels,
            desc.arrayLayers,
            vk::SampleCountFlagBits::e1,
            vk::ImageTiling::eOptimal,
            VkConvert::textureUsage(desc.usage) | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc,
            vk::SharingMode::eExclusive,
        };

        m_image = m_device->getHandle().createImage(imageInfo);

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocCreateInfo.flags = 0;

        VmaAllocation alloc;

        auto res1 = vmaCreateImage(
            m_device->getAllocator(),
            reinterpret_cast<VkImageCreateInfo*>(&imageInfo),
            &allocCreateInfo,
            reinterpret_cast<VkImage*>(&m_image),
            &alloc,
            nullptr
        );
    }

    VkTexture::VkTexture(VkDevice *device, const vk::Image image, const PixelFormat format, const uint32_t width, const uint32_t height)
        : m_width(width), m_height(height), m_depth(1),
        m_mipLevels(1), m_arrayLayers(1),
        m_format(format), m_type(TextureType::Texture2D),
        m_device(device), m_image(image)
    {
    }

    uint32_t VkTexture::getWidth() const
    {
        return m_width;
    }

    uint32_t VkTexture::getHeight() const
    {
        return m_height;
    }

    uint32_t VkTexture::getDepth() const
    {
        return m_depth;
    }

    uint32_t VkTexture::getMipLevels() const
    {
        return m_mipLevels;
    }

    uint32_t VkTexture::getArrayLayers() const
    {
        return m_arrayLayers;
    }

    PixelFormat VkTexture::getFormat() const
    {
        return m_format;
    }

    TextureType VkTexture::getType() const
    {
        return m_type;
    }

    vk::Image VkTexture::getHandle() const
    {
        return m_image;
    }

    void VkTexture::transitionLayout(const vk::CommandBuffer cmd, const vk::ImageLayout newLayout)
    {
        if(m_currentLayout == newLayout) return;
        
        vk::ImageMemoryBarrier barrier;
        barrier.oldLayout = m_currentLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = m_image;
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

        vk::PipelineStageFlags srcStage = {};
        vk::PipelineStageFlags dstStage = {};


        switch (m_currentLayout)
        {
            case vk::ImageLayout::eUndefined:
                barrier.srcAccessMask = {};
                srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
                break;

            case vk::ImageLayout::eTransferDstOptimal:
                barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
                srcStage = vk::PipelineStageFlagBits::eTransfer;
                break;

            case vk::ImageLayout::eTransferSrcOptimal:
                barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
                srcStage = vk::PipelineStageFlagBits::eTransfer;
                break;

            case vk::ImageLayout::eColorAttachmentOptimal:
                barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
                srcStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                break;

            case vk::ImageLayout::eDepthStencilAttachmentOptimal:
                barrier.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
                srcStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
                break;

            case vk::ImageLayout::eShaderReadOnlyOptimal:
                barrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
                srcStage = vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eComputeShader;
                break;

            default:
                barrier.srcAccessMask = {};
                srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
        }

        switch (newLayout)
        {
            case vk::ImageLayout::eTransferDstOptimal:
                barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
                dstStage = vk::PipelineStageFlagBits::eTransfer;
                break;

            case vk::ImageLayout::eShaderReadOnlyOptimal:
                barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
                dstStage = vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eComputeShader;
                break;

            case vk::ImageLayout::eColorAttachmentOptimal:
                barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
                dstStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
                break;

            case vk::ImageLayout::eDepthStencilAttachmentOptimal:
                barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
                dstStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
                break;

            default:
                barrier.dstAccessMask = {};
                dstStage = vk::PipelineStageFlagBits::eBottomOfPipe;
        }

        cmd.pipelineBarrier(
            srcStage,
            dstStage,
            {},
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        m_currentLayout = newLayout;
    }
}

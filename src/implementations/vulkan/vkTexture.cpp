#include "vkTexture.h"

#include "validation.h"
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
            VkConvert::pixelFormat(desc.format, m_device),
            {desc.width, desc.height, desc.depth},
            m_mipLevels,
            desc.arrayLayers,
            vk::SampleCountFlagBits::e1,
            vk::ImageTiling::eOptimal,
            VkConvert::textureUsage(desc.usage) | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc,
            vk::SharingMode::eExclusive,
        };

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocCreateInfo.flags = 0;

        auto res = vmaCreateImage(
            m_device->allocator(),
            reinterpret_cast<VkImageCreateInfo*>(&imageInfo),
            &allocCreateInfo,
            reinterpret_cast<VkImage*>(&m_image),
            &m_allocation,
            nullptr
        );

        URHI_VALIDATE(res == VK_SUCCESS, "Failed to create texture allocation - vmaCreateImage returned {}", vk::to_string(static_cast<vk::Result>(res)));
    }

    VkTexture::VkTexture(VkDevice *device, const vk::Image image, const PixelFormat format, const uint32_t width, const uint32_t height)
        : m_width(width), m_height(height), m_depth(1),
          m_mipLevels(1), m_arrayLayers(1),
          m_format(format), m_type(TextureType::Texture2D),
          m_device(device), m_allocation(nullptr), m_image(image),
          m_swapchainTexture(true)
    {
    }

    VkTexture::~VkTexture()
    {
        if(m_swapchainTexture) return;

        m_device->queueDestroy(m_life,
        [img = m_image, alloc = m_allocation](const VkDevice* device)
        {
            if(alloc != nullptr)
                vmaDestroyImage(device->allocator(), img, alloc);
            else
                device->handle().destroyImage(img);
        });
    }

    uint32_t VkTexture::width(const uint32_t mip) const
    {
        return std::max(1u, m_width >> mip);
    }

    uint32_t VkTexture::height(const uint32_t mip) const
    {
        return std::max(1u, m_height >> mip);
    }

    uint32_t VkTexture::depth(const uint32_t mip) const
    {
        return std::max(1u, m_depth >> mip);
    }

    uint32_t VkTexture::mipLevelCount() const
    {
        return m_mipLevels;
    }

    uint32_t VkTexture::arrayLayerCount() const
    {
        return m_arrayLayers;
    }

    PixelFormat VkTexture::format() const
    {
        return m_format;
    }

    TextureType VkTexture::type() const
    {
        return m_type;
    }

    vk::Image VkTexture::getHandle() const
    {
        return m_image;
    }

    void VkTexture::transitionLayout(
         const vk::CommandBuffer cmd,
         const vk::ImageLayout newLayout,
         const vk::PipelineStageFlags2 dstStageMask,
         const vk::AccessFlags2 dstAccessMask)
    {
        if (newLayout == vk::ImageLayout::eUndefined ||
            newLayout == vk::ImageLayout::ePreinitialized)
        {
            return;
        }

        vk::ImageMemoryBarrier2 barrier{};

        // 1. Use the tracked history for the Source!
        barrier.srcStageMask  = m_currentStage;
        barrier.srcAccessMask = m_currentAccess;
        barrier.oldLayout     = m_currentLayout;

        // 2. Use the user's request for the Destination!
        barrier.dstStageMask  = dstStageMask;
        barrier.dstAccessMask = dstAccessMask;
        barrier.newLayout     = newLayout;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = m_image;
        barrier.subresourceRange.aspectMask = VkConvert::aspectMask(m_format);
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

        vk::DependencyInfo depInfo{};
        depInfo.imageMemoryBarrierCount = 1;
        depInfo.pImageMemoryBarriers = &barrier;

        cmd.pipelineBarrier2(depInfo);

        // 3. IMPORTANT: Update the tracked history for the NEXT time this is called!
        m_currentLayout = newLayout;
        m_currentStage  = dstStageMask;
        m_currentAccess = dstAccessMask;
    }

    void VkTexture::resetTrackedState(vk::ImageLayout layout, vk::PipelineStageFlags2 stage, vk::AccessFlags2 access)
    {
        m_currentLayout = layout;
        m_currentStage = stage;
        m_currentAccess = access;
    }


    VkLifetime& VkTexture::lifetime()
    {
        return m_life;
    }

    bool VkTexture::isSwapchainTexture()
    {
        return m_swapchainTexture;
    }
}

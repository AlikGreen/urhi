#include "vkTexture.h"

#include "vkConvert.h"
#include "vkDevice.h"

namespace urhi
{
    VkTexture::VkTexture(VkDevice* device, const TextureDesc &desc)
        : m_width(desc.width), m_height(desc.height), m_depth(desc.depth),
        m_mipLevels(desc.numMipmaps), m_arrayLayers(desc.arrayLayers),
        m_format(desc.format), m_type(desc.type),
        m_device(device)
    {
        vk::ImageCreateInfo imageInfo
        {
            vk::ImageCreateFlags{0},
            VkConvert::textureType(desc.type),
            VkConvert::pixelFormat(desc.format),
            {desc.width, desc.height, desc.depth},
            desc.numMipmaps,
            desc.arrayLayers,
            vk::SampleCountFlagBits::e1,
            vk::ImageTiling::eOptimal,
            VkConvert::textureUsage(desc.usage),
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
}

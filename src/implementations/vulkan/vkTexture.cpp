#include "vkTexture.h"

#include "vkConvert.h"
#include "vkDevice.h"

namespace urhi
{
    VkTexture::VkTexture(VkDevice* device, const TextureDesc &desc)
        : m_device(device)
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

        auto res = m_device->getHandle().createImage(&imageInfo, nullptr, &m_image);

        VmaAllocationCreateInfo allocCreateInfo{};
        allocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO;
        allocCreateInfo.flags = 0;

        auto res1 = vmaCreateImage(
            m_device->getAllocator(),
            reinterpret_cast<VkImageCreateInfo*>(&imageInfo),
            &allocCreateInfo,
            reinterpret_cast<VkImage*>(&m_image),
            &m_allocation,
            nullptr
        );
    }

    vk::Image VkTexture::getHandle() const
    {
        return m_image;
    }
}

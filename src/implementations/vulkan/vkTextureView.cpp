#include "vkTextureView.h"

#include "vkConvert.h"
#include "vkDevice.h"
#include "vkTexture.h"

namespace urhi
{
    VkTextureView::VkTextureView(VkDevice *device, const TextureViewDesc &desc)
        : m_device(device), m_mipLevels(desc.mipLevels), m_arrayLayers(desc.arrayLayers), m_format(desc.format)
    {
        m_texture = std::dynamic_pointer_cast<VkTexture>(desc.texture);
        const vk::ImageViewCreateInfo viewInfo
        {
            vk::ImageViewCreateFlags{0},
            m_texture->getHandle(),
            VkConvert::textureViewType(desc.texture->getType()),
            VkConvert::pixelFormat(desc.format),
            {},
            {
                VkConvert::aspectMask(desc.format),
                desc.baseMipLevel,
                desc.mipLevels > 0 ? desc.mipLevels : VK_REMAINING_MIP_LEVELS,
                desc.baseArrayLayer,
                desc.arrayLayers > 0 ? desc.arrayLayers : VK_REMAINING_ARRAY_LAYERS
            }
        };

        m_imageView = device->getHandle().createImageView(viewInfo);
    }

    VkTextureView::VkTextureView(VkDevice *device, const grl::Rc<VkTexture> &texture, const PixelFormat format, const vk::ImageView view)
        : m_device(device), m_mipLevels(1), m_arrayLayers(1), m_format(format), m_imageView(view), m_texture(texture)
    {
    }

    VkTextureView::~VkTextureView()
    {
        m_device->getHandle().destroyImageView(m_imageView);
    }

    uint32_t VkTextureView::getMipLevels() const
    {
        return m_mipLevels;
    }

    uint32_t VkTextureView::getArrayLayers() const
    {
        return m_arrayLayers;
    }

    PixelFormat VkTextureView::getFormat() const
    {
        return m_format;
    }

    grl::Rc<Texture> VkTextureView::getTexture() const
    {
        return m_texture;
    }

    vk::ImageView VkTextureView::getHandle() const
    {
        return m_imageView;
    }
}

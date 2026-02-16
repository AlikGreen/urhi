#include "vkTextureView.h"

#include "vkConvert.h"
#include "vkDevice.h"
#include "vkTexture.h"

namespace urhi
{
    VkTextureView::VkTextureView(VkDevice *device, const TextureViewDesc &desc)
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

        auto res = device->getHandle().createImageView(&viewInfo, nullptr, &m_imageView);
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

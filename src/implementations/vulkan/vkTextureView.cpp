#include "vkTextureView.h"

#include "vkConvert.h"
#include "vkDevice.h"
#include "vkTexture.h"

namespace urhi
{
    VkTextureView::VkTextureView(VkDevice *device, const TextureViewDesc &desc)
        : m_device(device), m_baseMipLevel(desc.baseMipLevel), m_baseArrayLayer(desc.baseArrayLayer),
        m_mipLevels(desc.mipLevels), m_arrayLayers(desc.arrayLayers), m_format(desc.format)
    {
        m_texture = std::dynamic_pointer_cast<VkTexture>(desc.texture);
        const vk::ImageViewCreateInfo viewInfo
        {
            vk::ImageViewCreateFlags{0},
            m_texture->getHandle(),
            VkConvert::textureViewType(desc.texture->type()),
            VkConvert::pixelFormat(desc.format, m_device),
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
        : m_device(device), m_baseMipLevel(0), m_baseArrayLayer(0), m_mipLevels(1), m_arrayLayers(1), m_format(format), m_imageView(view), m_texture(texture), m_owned(false)
    {
    }

    VkTextureView::~VkTextureView()
    {
        if(!m_owned) return;

        m_device->queueDestroy(m_life,
        [h = m_imageView](const VkDevice* device)
        {
            device->getHandle().destroyImageView(h);
        });
    }

    uint32_t VkTextureView::baseMipLevel() const
    {
        return m_baseMipLevel;
    }

    uint32_t VkTextureView::mipLevelCount() const
    {
        return m_mipLevels;
    }

    uint32_t VkTextureView::baseArrayLayer() const
    {
        return m_baseArrayLayer;
    }

    uint32_t VkTextureView::arrayLayerCount() const
    {
        return m_arrayLayers;
    }

    PixelFormat VkTextureView::format() const
    {
        return m_format;
    }

    grl::Rc<Texture> VkTextureView::texture() const
    {
        return m_texture;
    }

    vk::ImageView VkTextureView::getHandle() const
    {
        return m_imageView;
    }

    VkLifetime& VkTextureView::lifetime()
    {
        return m_life;
    }

    void VkTextureView::markUsed(const QueueType type, const uint64_t submitValue)
    {
        lifetime().markUsed(type, submitValue);
        m_texture->lifetime().markUsed(type, submitValue);
    }
}

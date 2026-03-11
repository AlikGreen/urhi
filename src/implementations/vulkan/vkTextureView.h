#pragma once
#include "textureView.h"
#include "descriptions/textureViewDesc.h"

#include <vulkan/vulkan.hpp>

#include "vkTexture.h"

namespace urhi
{
class VkDevice;

class VkTextureView final : public TextureView
{
public:
    VkTextureView(VkDevice* device, const TextureViewDesc& desc);
    VkTextureView(VkDevice* device, const grl::Rc<VkTexture> &texture, PixelFormat format, vk::ImageView view);

    ~VkTextureView() override;

    [[nodiscard]] uint32_t baseMipLevel() const override;
    [[nodiscard]] uint32_t mipLevelCount() const override;

    [[nodiscard]] uint32_t baseArrayLayer() const override;
    [[nodiscard]] uint32_t arrayLayerCount() const override;

    [[nodiscard]] PixelFormat format() const override;
    [[nodiscard]] grl::Rc<Texture> texture() const override;

    [[nodiscard]]  vk::ImageView getHandle() const;
    VkLifetime& lifetime();

    void markUsed(QueueType type, uint64_t submitValue);
private:
    VkDevice* m_device;
    uint32_t m_baseMipLevel, m_baseArrayLayer;
    uint32_t m_mipLevels, m_arrayLayers;
    PixelFormat m_format;

    vk::ImageView m_imageView;
    grl::Rc<VkTexture> m_texture;

    VkLifetime m_life{};
    bool m_owned = true;
};
}

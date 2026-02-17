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

    ~VkTextureView();

    [[nodiscard]] uint32_t getMipLevels() const override;
    [[nodiscard]] uint32_t getArrayLayers() const override;

    [[nodiscard]] PixelFormat getFormat() const override;
    [[nodiscard]] grl::Rc<Texture> getTexture() const override;

    [[nodiscard]]  vk::ImageView getHandle() const;
private:
    VkDevice* m_device;
    uint32_t m_mipLevels, m_arrayLayers;
    PixelFormat m_format;

    vk::ImageView m_imageView;
    grl::Rc<VkTexture> m_texture;
};
}

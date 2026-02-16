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
    [[nodiscard]] uint32_t getWidth() const override;
    [[nodiscard]] uint32_t getHeight() const override;
    [[nodiscard]] uint32_t getDepth() const override;

    [[nodiscard]] uint32_t getMipLevels() const override;
    [[nodiscard]] uint32_t getArrayLayers() const override;

    [[nodiscard]] PixelFormat getFormat() const override;
    [[nodiscard]] grl::Rc<Texture> getTexture() const override;

    [[nodiscard]]  vk::ImageView getHandle() const;
private:
    vk::ImageView m_imageView;
    grl::Rc<VkTexture> m_texture;
};
}

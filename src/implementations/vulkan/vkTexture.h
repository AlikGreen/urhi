#pragma once
#include <vma/vk_mem_alloc.h>

#include "texture.h"
#include "descriptions/textureDesc.h"

#include <vulkan/vulkan.hpp>

namespace urhi
{
class VkDevice;

class VkTexture final : public Texture
{
public:
    VkTexture(VkDevice* device, const TextureDesc& desc);
    VkTexture(VkDevice* device, vk::Image image, PixelFormat format, uint32_t width, uint32_t height);
    [[nodiscard]] uint32_t getWidth() const override;
    [[nodiscard]] uint32_t getHeight() const override;
    [[nodiscard]] uint32_t getDepth() const override;

    [[nodiscard]] uint32_t getMipLevels() const override;
    [[nodiscard]] uint32_t getArrayLayers() const override;

    [[nodiscard]] PixelFormat getFormat() const override;
    [[nodiscard]] TextureType getType() const override;

    [[nodiscard]] vk::Image getHandle() const;
private:
    uint32_t m_width, m_height, m_depth;
    uint32_t m_mipLevels, m_arrayLayers;
    PixelFormat m_format;
    TextureType m_type;

    VkDevice* m_device;
    vk::Image m_image;
};
}

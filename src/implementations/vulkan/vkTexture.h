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
    [[nodiscard]] uint32_t getWidth() const override;
    [[nodiscard]] uint32_t getHeight() const override;
    [[nodiscard]] uint32_t getDepth() const override;

    [[nodiscard]] uint32_t getMipLevels() const override;
    [[nodiscard]] uint32_t getArrayLayers() const override;

    [[nodiscard]] PixelFormat getFormat() const override;
    [[nodiscard]] TextureType getType() const override;

    [[nodiscard]] vk::Image getHandle() const;
private:
    VkDevice* m_device;
    vk::Image m_image;
    VmaAllocation m_allocation;
};
}

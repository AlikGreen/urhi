#pragma once
#include <vma/vk_mem_alloc.h>

#include "texture.h"
#include "descriptions/textureDesc.h"

#include <vulkan/vulkan.hpp>

#include "vkLifetime.h"

namespace urhi
{
class VkDevice;

class VkTexture final : public Texture
{
public:
    VkTexture(VkDevice* device, const TextureDesc& desc);
    VkTexture(VkDevice* device, vk::Image image, PixelFormat format, uint32_t width, uint32_t height);
    ~VkTexture() override;

    [[nodiscard]] uint32_t width(uint32_t mip = 0) const override;
    [[nodiscard]] uint32_t height(uint32_t mip = 0) const override;
    [[nodiscard]] uint32_t depth(uint32_t mip = 0) const override;

    [[nodiscard]] uint32_t mipLevelCount() const override;
    [[nodiscard]] uint32_t arrayLayerCount() const override;

    [[nodiscard]] PixelFormat format() const override;
    [[nodiscard]] TextureType type() const override;

    [[nodiscard]] vk::Image getHandle() const;

    void transitionLayout(vk::CommandBuffer cmd, vk::ImageLayout newLayout);

    VkLifetime& lifetime();
private:
    friend class VkCommandListEmitter;

    uint32_t m_width, m_height, m_depth;
    uint32_t m_mipLevels{};
    uint32_t m_arrayLayers;
    PixelFormat m_format;
    TextureType m_type;

    VkDevice* m_device;
    vk::Image m_image;
    vk::ImageLayout m_currentLayout = vk::ImageLayout::eUndefined;

    VkLifetime m_life;
    bool m_owned = true;
};
}

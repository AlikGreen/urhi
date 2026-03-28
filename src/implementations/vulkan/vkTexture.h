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

    [[nodiscard]] uint32_t width(uint32_t mip) const override;
    [[nodiscard]] uint32_t height(uint32_t mip) const override;
    [[nodiscard]] uint32_t depth(uint32_t mip) const override;

    [[nodiscard]] uint32_t mipLevelCount() const override;
    [[nodiscard]] uint32_t arrayLayerCount() const override;

    [[nodiscard]] PixelFormat format() const override;
    [[nodiscard]] TextureType type() const override;

    [[nodiscard]] vk::Image getHandle() const;

    void transitionLayout(vk::CommandBuffer cmd, vk::ImageLayout newLayout, vk::PipelineStageFlags2 dstStageMask, vk::AccessFlags2 dstAccessMask);
    void resetTrackedState(vk::ImageLayout layout, vk::PipelineStageFlags2 stage, vk::AccessFlags2 access);

    [[nodiscard]] vk::ImageLayout getLayout() const { return m_currentLayout; }
    [[nodiscard]] vk::PipelineStageFlags2 getStage() const { return m_currentStage; }
    [[nodiscard]] vk::AccessFlags2 getAccess() const { return m_currentAccess; }

    VkLifetime& lifetime();
private:
    uint32_t m_width, m_height, m_depth;
    uint32_t m_mipLevels{};
    uint32_t m_arrayLayers;
    PixelFormat m_format;
    TextureType m_type;

    VkDevice* m_device;
    VmaAllocation m_allocation{};
    vk::Image m_image;

    vk::ImageLayout m_currentLayout = vk::ImageLayout::eUndefined;
    vk::PipelineStageFlags2 m_currentStage = vk::PipelineStageFlagBits2::eTopOfPipe;
    vk::AccessFlags2 m_currentAccess = vk::AccessFlagBits2::eNone;

    VkLifetime m_life;
    bool m_owned = true;
};
}

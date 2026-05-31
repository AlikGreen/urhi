#pragma once

#include "texture.h"
#include "descriptions/textureDesc.h"

namespace urhi
{
class GlDevice;

class GlTexture final : public Texture
{
public:
    GlTexture(GlDevice* device, const TextureDesc& desc);
    GlTexture() = default;
    ~GlTexture() override;

    [[nodiscard]] uint32_t width(uint32_t mip) const override;
    [[nodiscard]] uint32_t height(uint32_t mip) const override;
    [[nodiscard]] uint32_t depth(uint32_t mip) const override;

    [[nodiscard]] uint32_t mipLevelCount() const override;

    [[nodiscard]] PixelFormat format() const override;
    [[nodiscard]] TextureType type() const override;

    [[nodiscard]] uint32_t handle() const { return m_handle; }


    bool isSwapchainTexture();
private:
    friend class GlSwapchain;

    uint32_t m_width, m_height, m_depth;
    uint32_t m_mipLevels{};
    PixelFormat m_format;
    TextureType m_type;

    GlDevice* m_device;

    uint32_t m_handle{};
    bool m_swapchainTexture = false;
};
}

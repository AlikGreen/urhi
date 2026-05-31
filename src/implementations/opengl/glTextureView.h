#pragma once
#include "textureView.h"
#include "descriptions/textureViewDesc.h"

#include "glTexture.h"

namespace urhi
{
class GlDevice;

class GlTextureView final : public TextureView
{
public:
    GlTextureView(GlDevice* device, const TextureViewDesc& desc);
    GlTextureView();
    // GlTextureView(GlDevice* device, const grl::Rc<GlTexture> &texture, PixelFormat format, gl::ImageView view);

    ~GlTextureView() override;

    [[nodiscard]] uint32_t baseMipLevel() const override { return m_baseMipLevel; }
    [[nodiscard]] uint32_t mipLevelCount() const override { return m_mipLevels; }

    [[nodiscard]] uint32_t baseArrayLayer() const override { return m_baseArrayLayer; }
    [[nodiscard]] uint32_t arrayLayerCount() const override { return m_arrayLayers; }

    [[nodiscard]] PixelFormat format() const override { return m_format; }
    [[nodiscard]] grl::Rc<Texture> texture() const override { return m_texture; }

    [[nodiscard]]  uint32_t handle() const { return m_handle; };
private:
    friend class GlSwapchain;
    GlDevice* m_device;
    uint32_t m_baseMipLevel, m_baseArrayLayer;
    uint32_t m_mipLevels, m_arrayLayers;
    PixelFormat m_format;

    uint32_t m_handle = 0;

    grl::Rc<GlTexture> m_texture;
};
}

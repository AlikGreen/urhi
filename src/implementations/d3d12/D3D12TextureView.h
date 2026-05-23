#pragma once
#include "d3D12Device.h"
#include "d3D12Texture.h"
#include "textureView.h"

namespace urhi
{
class D3D12TextureView final : public TextureView
{
public:
    D3D12TextureView(D3D12Device* device, const TextureViewDesc& desc);
    ~D3D12TextureView() override;

    [[nodiscard]] uint32_t baseMipLevel() const override { return m_firstMip; }
    [[nodiscard]] uint32_t mipLevelCount() const override { return m_mipCount; }
    [[nodiscard]] uint32_t baseArrayLayer() const override { return m_firstLayer; }
    [[nodiscard]] uint32_t arrayLayerCount() const override { return m_layerCount; }

    [[nodiscard]] PixelFormat format() const override { return m_format; }

    [[nodiscard]] grl::Rc<Texture> texture() const override { return m_texture; }
private:
    uint32_t m_firstMip, m_mipCount;
    uint32_t m_firstLayer, m_layerCount;
    PixelFormat m_format;

    grl::Rc<D3D12Texture> m_texture;
};
}

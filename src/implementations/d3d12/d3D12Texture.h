#pragma once
#include "d3D12Device.h"
#include "texture.h"
#include "descriptions/textureDesc.h"

namespace urhi
{
class D3D12Texture final : public Texture
{
public:
    D3D12Texture(D3D12Device* device, const TextureDesc& desc);
    ~D3D12Texture() override;

    [[nodiscard]] uint32_t width(const uint32_t mip) const override { return std::max(1u, m_width >> mip); }
    [[nodiscard]] uint32_t height(const uint32_t mip) const override { return std::max(1u, m_height >> mip); }
    [[nodiscard]] uint32_t depth(const uint32_t mip) const override { return std::max(1u, m_depth >> mip); }

    [[nodiscard]] uint32_t mipLevelCount() const override { return m_mipLevels; }
    [[nodiscard]] PixelFormat format() const override { return m_format; }
    [[nodiscard]] TextureType type() const override { return m_type; }

    D3D12Lifetime& life() { return m_life; }
private:
    D3D12Device* m_device;
    D3D12Lifetime m_life;

    uint32_t m_width, m_depth, m_height;
    uint32_t m_mipLevels{};
    PixelFormat m_format;
    TextureType m_type;

    ID3D12Resource* m_resource{};
    D3D12MA::Allocation* m_allocation{};
};
}

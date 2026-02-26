#pragma once
#include <cstdint>
#include <grl/grl.h>
#include "texture.h"

#include "enums/pixelFormat.h"

namespace urhi
{
class TextureView
{
public:
    TextureView() = default;
    virtual ~TextureView() = default;

    TextureView(const TextureView&) = delete;
    TextureView& operator= (const TextureView&) = delete;

    [[nodiscard]] virtual uint32_t baseMipLevel() const = 0;
    [[nodiscard]] virtual uint32_t mipLevelCount() const = 0;

    [[nodiscard]] virtual uint32_t baseArrayLayer() const = 0;
    [[nodiscard]] virtual uint32_t arrayLayerCount() const = 0;

    [[nodiscard]] virtual PixelFormat format() const = 0;

    [[nodiscard]] virtual grl::Rc<Texture> texture() const = 0;
};
}

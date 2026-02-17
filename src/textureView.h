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

    [[nodiscard]] virtual uint32_t getMipLevels() const = 0;
    [[nodiscard]] virtual uint32_t getArrayLayers() const = 0;
    [[nodiscard]] virtual PixelFormat getFormat() const = 0;

    [[nodiscard]] virtual grl::Rc<Texture> getTexture() const = 0;
};
}

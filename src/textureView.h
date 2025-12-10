#pragma once
#include <cstdint>

#include "enums/pixelFormat.h"

namespace Neon::RHI
{
class TextureView
{
public:
    virtual ~TextureView() = default;

    [[nodiscard]] virtual uint32_t getWidth() const = 0;
    [[nodiscard]] virtual uint32_t getHeight() const = 0;
    [[nodiscard]] virtual uint32_t getDepth() const = 0;

    [[nodiscard]] virtual uint32_t getMipLevels() const = 0;
    [[nodiscard]] virtual uint32_t getArrayLayers() const = 0;
    [[nodiscard]] virtual PixelFormat getFormat() const = 0;
};
}

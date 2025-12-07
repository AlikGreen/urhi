#pragma once
#include <cstdint>

#include "enums/pixelFormat.h"

namespace Neon::RHI
{
class Texture
{
public:
    virtual ~Texture() = default;

    [[nodiscard]] virtual uint32_t getWidth() const = 0;
    [[nodiscard]] virtual uint32_t getHeight() const = 0;
    [[nodiscard]] virtual uint32_t getDepth() const = 0;

    virtual uint32_t getMipLevels() = 0;
    virtual uint32_t getArrayLayers() = 0;
    virtual PixelFormat getFormat() = 0;
};
}

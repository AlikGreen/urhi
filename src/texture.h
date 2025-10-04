#pragma once
#include <cstdint>

#include "enums/pixelFormat.h"

namespace Neon::RHI
{
class Texture
{
public:
    virtual ~Texture() = default;

    virtual uint32_t getWidth() = 0;
    virtual uint32_t getHeight() = 0;
    virtual uint32_t getDepth() = 0;

    virtual uint32_t getMipLevels() = 0;
    virtual uint32_t getArrayLayers() = 0;
    virtual PixelFormat getFormat() = 0;
};
}

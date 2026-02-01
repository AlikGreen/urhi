#pragma once
#include <cstdint>

#include "enums/pixelLayout.h"
#include "enums/pixelType.h"

namespace Neon::RHI
{
struct TextureReadDesc
{
    uint32_t mipLevel = 0;

    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t z = 0;
    uint32_t width  = 0;
    uint32_t height = 0;
    uint32_t depth  = 0;

    PixelType pixelType = PixelType::UnsignedByte;
    PixelLayout pixelLayout = PixelLayout::RGBA;
};
}

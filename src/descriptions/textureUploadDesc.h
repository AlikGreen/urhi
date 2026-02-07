#pragma once
#include "enums/pixelLayout.h"
#include "enums/pixelType.h"

#include "glm/glm.hpp"

namespace urhi
{
    struct TextureUploadDesc
    {
        const void* data = nullptr;

        PixelType pixelType = PixelType::UnsignedByte;
        PixelLayout pixelLayout = PixelLayout::RGBA;

        uint32_t mipLevel = 0;

        uint32_t x = 0;
        uint32_t y = 0;
        uint32_t z = 0;
        uint32_t width  = 0;
        uint32_t height = 0;
        uint32_t depth  = 0;

        uint32_t baseLayer  = 0;
        uint32_t layerCount = 1;
    };
}

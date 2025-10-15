#pragma once
#include "enums/pixelLayout.h"
#include "enums/pixelType.h"

#include "glm/glm.hpp"

namespace Neon::RHI
{
struct TextureUploadDescription
{
    const void* data{};
    PixelType pixelType = PixelType::UnsignedByte;
    PixelLayout pixelLayout = PixelLayout::RGBA;

    uint32_t mipLevel = 0;

    glm::uvec3 offset{};
    glm::uvec3 size{};
};
}

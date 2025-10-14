#pragma once
#include "enums/pixelFormat.h"
#include "enums/pixelType.h"

namespace Neon::RHI
{
struct TextureUploadDescription
{
    const void* data{};
    PixelType pixelType = PixelType::UnsignedByte;

    uint32_t mipLevel = 0;

    glm::uvec3 offset{};
    glm::uvec3 size{};
};
}

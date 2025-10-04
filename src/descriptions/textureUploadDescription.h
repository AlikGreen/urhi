#pragma once
#include "enums/pixelFormat.h"
#include "enums/pixelType.h"

namespace Neon::RHI
{
struct TextureUploadDescription
{
    const void* data{};
    PixelType pixelType = PixelType::UnsignedByte;
};
}

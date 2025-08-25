#pragma once
#include <cstdint>

#include "enums/textureFormat.h"
#include "enums/textureUsage.h"

namespace NRHI
{
struct TextureDescription
{
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t depth = 0;

    TextureFormat format = TextureFormat::R8G8B8A8Unorm;
    TextureUsage usage = TextureUsage::Sampler;

    uint32_t numMipmaps = 1;
};
}

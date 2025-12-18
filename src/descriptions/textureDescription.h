#pragma once
#include <cstdint>

#include "enums/textureType.h"
#include "enums/textureUsage.h"
#include "glm/glm.hpp"

namespace Neon::RHI
{
struct TextureDescription
{
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t depth = 0;

    uint32_t arrayLayers = 1;

    PixelFormat format = PixelFormat::Invalid;
    TextureUsage usage = TextureUsage::Sampled;
    TextureType type = TextureType::Texture2D;

    uint32_t numMipmaps{};

    static TextureDescription Texture1D(
        const uint32_t width,
        const PixelFormat format,
        const TextureUsage usage = TextureUsage::Sampled,
        const uint32_t numMipmaps = 1,
        const uint32_t arrayLayers = 1)
    {
        TextureDescription desc;
        desc.type = TextureType::Texture1D;
        desc.width = width;
        desc.arrayLayers = arrayLayers;
        desc.format = format;
        desc.usage = usage;
        desc.numMipmaps = numMipmaps;
        return desc;
    }

    static TextureDescription Texture2D(
        const uint32_t width,
        const uint32_t height,
        const PixelFormat format,
        const TextureUsage usage = TextureUsage::Sampled,
        const uint32_t numMipmaps = 1,
        const uint32_t arrayLayers = 1)
    {
        TextureDescription desc;
        desc.type = TextureType::Texture2D;
        desc.width = width;
        desc.height = height;
        desc.arrayLayers = arrayLayers;
        desc.format = format;
        desc.usage = usage;
        desc.numMipmaps = numMipmaps;
        return desc;
    }

    static TextureDescription Texture3D(
        const uint32_t width,
        const uint32_t height,
        const uint32_t depth,
        const PixelFormat format,
        const TextureUsage usage = TextureUsage::Sampled,
        const uint32_t numMipmaps = 1,
        const uint32_t arrayLayers = 1)
    {
        TextureDescription desc;
        desc.type = TextureType::Texture3D;
        desc.width = width;
        desc.height = height;
        desc.depth = depth;
        desc.arrayLayers = arrayLayers;
        desc.format = format;
        desc.usage = usage;
        desc.numMipmaps = numMipmaps;
        return desc;
    }
};
}

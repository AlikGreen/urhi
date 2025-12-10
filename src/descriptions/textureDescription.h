#pragma once
#include <cstdint>

#include "enums/textureType.h"
#include "enums/textureUsage.h"
#include "glm/glm.hpp"

namespace Neon::RHI
{
struct TextureDescription
{
    glm::uvec3 dimensions{};

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
        desc.dimensions = glm::uvec3(width, 1, 1);
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
        desc.dimensions = glm::uvec3(width, height, 1);
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
        desc.dimensions = glm::uvec3(width, height, depth);
        desc.arrayLayers = arrayLayers;
        desc.format = format;
        desc.usage = usage;
        desc.numMipmaps = numMipmaps;
        return desc;
    }
};
}

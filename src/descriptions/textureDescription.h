#pragma once
#include <cstdint>

#include "enums/textureType.h"
#include "enums/textureUsage.h"
#include "glm/glm.hpp"

namespace Neon::RHI
{
struct TextureDescription
{
    uint32_t width = 1;
    uint32_t height = 1;
    uint32_t depth = 1;

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

    static TextureDescription Texture2DArray(
    const uint32_t width,
    const uint32_t height,
    const uint32_t arrayLayers,
    const PixelFormat format,
    const TextureUsage usage = TextureUsage::Sampled,
    const uint32_t numMipmaps = 1)
    {
        TextureDescription desc;
        desc.type = TextureType::Texture2DArray;
        desc.width = width;
        desc.height = height;
        desc.depth = 1;
        desc.arrayLayers = arrayLayers;
        desc.format = format;
        desc.usage = usage;
        desc.numMipmaps = numMipmaps;
        return desc;
    }

    static TextureDescription TextureCube(
    const uint32_t size,
    const PixelFormat format,
    const TextureUsage usage = TextureUsage::Sampled,
    const uint32_t numMipmaps = 1)
    {
        TextureDescription desc;
        desc.type = TextureType::TextureCube;
        desc.width = size;
        desc.height = size;
        desc.depth = 1;
        desc.arrayLayers = 6;
        desc.format = format;
        desc.usage = usage;
        desc.numMipmaps = numMipmaps;
        return desc;
    }

    static TextureDescription TextureCubeArray(
        const uint32_t size,
        const uint32_t cubeCount,
        const PixelFormat format,
        const TextureUsage usage = TextureUsage::Sampled,
        const uint32_t numMipmaps = 1)
    {
        TextureDescription desc;
        desc.type = TextureType::TextureCubeArray;
        desc.width = size;
        desc.height = size;
        desc.depth = 1;
        desc.arrayLayers = 6 * cubeCount;
        desc.format = format;
        desc.usage = usage;
        desc.numMipmaps = numMipmaps;
        return desc;
    }
};
}

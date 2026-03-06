#pragma once
#include <cstdint>

#include "enums/textureType.h"
#include "enums/textureUsage.h"
#include "glm/glm.hpp"

namespace urhi
{
struct TextureDesc
{
    uint32_t width = 1;
    uint32_t height = 1;
    uint32_t depth = 1;

    uint32_t arrayLayers = 1;

    PixelFormat format = PixelFormat::Unknown;
    TextureUsage usage = TextureUsage::Sampled;
    TextureType type = TextureType::Texture2D;

    uint32_t maxMipLevels = 1;

    static TextureDesc Texture1D(
        const uint32_t width,
        const PixelFormat format,
        const TextureUsage usage = TextureUsage::Sampled,
        const uint32_t maxMipLevels = 1,
        const uint32_t arrayLayers = 1)
    {
        TextureDesc desc;
        desc.type = TextureType::Texture1D;
        desc.width = width;
        desc.arrayLayers = arrayLayers;
        desc.format = format;
        desc.usage = usage;
        desc.maxMipLevels = maxMipLevels;
        return desc;
    }

    static TextureDesc Texture2D(
        const uint32_t width,
        const uint32_t height,
        const PixelFormat format,
        const TextureUsage usage = TextureUsage::Sampled,
        const uint32_t maxMipLevels = 1,
        const uint32_t arrayLayers = 1)
    {
        TextureDesc desc;
        desc.type = TextureType::Texture2D;
        desc.width = width;
        desc.height = height;
        desc.arrayLayers = arrayLayers;
        desc.format = format;
        desc.usage = usage;
        desc.maxMipLevels = maxMipLevels;
        return desc;
    }

    static TextureDesc Texture3D(
        const uint32_t width,
        const uint32_t height,
        const uint32_t depth,
        const PixelFormat format,
        const TextureUsage usage = TextureUsage::Sampled,
        const uint32_t maxMipLevels = 1,
        const uint32_t arrayLayers = 1)
    {
        TextureDesc desc;
        desc.type = TextureType::Texture3D;
        desc.width = width;
        desc.height = height;
        desc.depth = depth;
        desc.arrayLayers = arrayLayers;
        desc.format = format;
        desc.usage = usage;
        desc.maxMipLevels = maxMipLevels;
        return desc;
    }

    static TextureDesc Texture2DArray(
    const uint32_t width,
    const uint32_t height,
    const uint32_t arrayLayers,
    const PixelFormat format,
    const TextureUsage usage = TextureUsage::Sampled,
    const uint32_t maxMipLevels = 1)
    {
        TextureDesc desc;
        desc.type = TextureType::Texture2DArray;
        desc.width = width;
        desc.height = height;
        desc.depth = 1;
        desc.arrayLayers = arrayLayers;
        desc.format = format;
        desc.usage = usage;
        desc.maxMipLevels = maxMipLevels;
        return desc;
    }

    static TextureDesc TextureCube(
    const uint32_t size,
    const PixelFormat format,
    const TextureUsage usage = TextureUsage::Sampled,
    const uint32_t maxMipLevels = 1)
    {
        TextureDesc desc;
        desc.type = TextureType::TextureCube;
        desc.width = size;
        desc.height = size;
        desc.depth = 1;
        desc.arrayLayers = 6;
        desc.format = format;
        desc.usage = usage;
        desc.maxMipLevels = maxMipLevels;
        return desc;
    }

    static TextureDesc TextureCubeArray(
        const uint32_t size,
        const uint32_t cubeCount,
        const PixelFormat format,
        const TextureUsage usage = TextureUsage::Sampled,
        const uint32_t maxMipLevels = 1)
    {
        TextureDesc desc;
        desc.type = TextureType::TextureCubeArray;
        desc.width = size;
        desc.height = size;
        desc.depth = 1;
        desc.arrayLayers = 6 * cubeCount;
        desc.format = format;
        desc.usage = usage;
        desc.maxMipLevels = maxMipLevels;
        return desc;
    }
};
}

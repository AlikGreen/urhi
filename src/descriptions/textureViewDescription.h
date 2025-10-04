#pragma once
#include "texture.h"
#include "textureDescription.h"

namespace Neon::RHI
{
struct TextureViewDescription
{
    Texture* target{};

    uint32_t baseMipLevel = 0;
    uint32_t mipLevels = 1;
    uint32_t baseArrayLayer = 0;
    uint32_t arrayLayers = 1;

    PixelFormat format = PixelFormat::R8G8B8A8Unorm;

    explicit TextureViewDescription(Texture* target)
    {
        this->target = target;
        baseMipLevel = 0;
        mipLevels = target->getMipLevels();
        baseArrayLayer = 0;
        arrayLayers = target->getArrayLayers();
        format = target->getFormat();
    }

    TextureViewDescription(Texture* target, const PixelFormat format)
    {
        this->target = target;
        baseMipLevel = 0;
        mipLevels = target->getMipLevels();
        baseArrayLayer = 0;
        arrayLayers = target->getArrayLayers();
        this->format = format;
    }

    TextureViewDescription(Texture* target, const uint32_t baseMipLevel, const uint32_t mipLevels, const uint32_t baseArrayLayer, const uint32_t arrayLayers)
    {
        this->target = target;
        this->baseMipLevel = baseMipLevel;
        this->mipLevels = mipLevels;
        this->baseArrayLayer = baseArrayLayer;
        this->arrayLayers = arrayLayers;
        format = target->getFormat();
    }
    TextureViewDescription(Texture* target, const PixelFormat format, const uint32_t baseMipLevel, const uint32_t mipLevels, const uint32_t baseArrayLayer, const uint32_t arrayLayers)
    {
        this->target = target;
        this->baseMipLevel = baseMipLevel;
        this->mipLevels = mipLevels;
        this->baseArrayLayer = baseArrayLayer;
        this->arrayLayers = arrayLayers;
        this->format = format;
    }
};
}

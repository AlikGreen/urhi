#pragma once
#include "texture.h"
#include <neonCore/neonCore.h>

namespace Neon::RHI
{
struct TextureViewDescription
{
    Rc<Texture> target = nullptr;

    uint32_t baseMipLevel = 0;
    uint32_t mipLevels = 1;
    uint32_t baseArrayLayer = 0;
    uint32_t arrayLayers = 1;

    PixelFormat format = PixelFormat::Invalid;

    TextureViewDescription() = default;

    explicit TextureViewDescription(const Rc<Texture>& target)
    {
        this->target = target;
        baseMipLevel = 0;
        mipLevels = target->getMipLevels();
        baseArrayLayer = 0;
        arrayLayers = target->getArrayLayers();
        format = target->getFormat();
    }

    TextureViewDescription(const Rc<Texture>& target, const PixelFormat format)
    {
        this->target = target;
        baseMipLevel = 0;
        mipLevels = target->getMipLevels();
        baseArrayLayer = 0;
        arrayLayers = target->getArrayLayers();
        this->format = format;
    }

    TextureViewDescription(const Rc<Texture>& target, const uint32_t baseMipLevel, const uint32_t mipLevels, const uint32_t baseArrayLayer, const uint32_t arrayLayers)
    {
        this->target = target;
        this->baseMipLevel = baseMipLevel;
        this->mipLevels = mipLevels;
        this->baseArrayLayer = baseArrayLayer;
        this->arrayLayers = arrayLayers;
        format = target->getFormat();
    }

    TextureViewDescription(const Rc<Texture>& target, const PixelFormat format, const uint32_t baseMipLevel, const uint32_t mipLevels, const uint32_t baseArrayLayer, const uint32_t arrayLayers)
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

#pragma once
#include "texture.h"
#include <grl/grl.h>

namespace urhi
{
struct TextureViewDesc
{
    grl::Rc<Texture> texture = nullptr;

    uint32_t baseMipLevel = 0;
    uint32_t mipLevels = 1;
    uint32_t baseArrayLayer = 0;
    uint32_t arrayLayers = 1;

    PixelFormat format = PixelFormat::Unknown;

    TextureViewDesc() = default;

    explicit TextureViewDesc(const grl::Rc<Texture>& texture)
    {
        this->texture = texture;
        baseMipLevel = 0;
        mipLevels = texture->mipLevelCount();
        baseArrayLayer = 0;
        arrayLayers = texture->arrayLayerCount();
        format = texture->format();
    }

    TextureViewDesc(const grl::Rc<Texture>& texture, const PixelFormat format)
    {
        this->texture = texture;
        baseMipLevel = 0;
        mipLevels = texture->mipLevelCount();
        baseArrayLayer = 0;
        arrayLayers = texture->arrayLayerCount();
        this->format = format;
    }

    TextureViewDesc(const grl::Rc<Texture>& texture, const uint32_t baseMipLevel, const uint32_t mipLevels, const uint32_t baseArrayLayer, const uint32_t arrayLayers)
    {
        this->texture = texture;
        this->baseMipLevel = baseMipLevel;
        this->mipLevels = mipLevels;
        this->baseArrayLayer = baseArrayLayer;
        this->arrayLayers = arrayLayers;
        format = texture->format();
    }

    TextureViewDesc(const grl::Rc<Texture>& texture, const PixelFormat format, const uint32_t baseMipLevel, const uint32_t mipLevels, const uint32_t baseArrayLayer, const uint32_t arrayLayers)
    {
        this->texture = texture;
        this->baseMipLevel = baseMipLevel;
        this->mipLevels = mipLevels;
        this->baseArrayLayer = baseArrayLayer;
        this->arrayLayers = arrayLayers;
        this->format = format;
    }
};
}

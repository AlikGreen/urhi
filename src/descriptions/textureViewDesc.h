#pragma once
#include "texture.h"
#include <grl/grl.h>

namespace urhi
{
struct TextureViewDesc
{
    grl::Rc<Texture> texture = nullptr;

    uint32_t firstMip = 0;
    uint32_t mipCount = 1;
    uint32_t firstLayer = 0;
    uint32_t layerCount = 1;

    PixelFormat format = PixelFormat::Unknown;

    TextureViewDesc() = default;

    explicit TextureViewDesc(const grl::Rc<Texture>& texture)
    {
        this->texture = texture;
        firstMip = 0;
        mipCount = texture->mipLevelCount();
        firstLayer = 0;
        layerCount = texture->depth();
        format = texture->format();
    }

    TextureViewDesc(const grl::Rc<Texture>& texture, const PixelFormat format)
    {
        this->texture = texture;
        firstMip = 0;
        mipCount = texture->mipLevelCount();
        firstLayer = 0;
        layerCount = texture->depth();
        this->format = format;
    }

    TextureViewDesc(const grl::Rc<Texture>& texture, const uint32_t baseMipLevel, const uint32_t mipLevels, const uint32_t baseArrayLayer, const uint32_t arrayLayers)
    {
        this->texture = texture;
        this->firstMip = baseMipLevel;
        this->mipCount = mipLevels;
        this->firstLayer = baseArrayLayer;
        this->layerCount = arrayLayers;
        format = texture->format();
    }

    TextureViewDesc(const grl::Rc<Texture>& texture, const PixelFormat format, const uint32_t baseMipLevel, const uint32_t mipLevels, const uint32_t baseArrayLayer, const uint32_t arrayLayers)
    {
        this->texture = texture;
        this->firstMip = baseMipLevel;
        this->mipCount = mipLevels;
        this->firstLayer = baseArrayLayer;
        this->layerCount = arrayLayers;
        this->format = format;
    }
};
}

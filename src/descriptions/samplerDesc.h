#pragma once

#include "enums/mipmapFilter.h"
#include "enums/textureFilter.h"
#include "enums/textureWrap.h"

namespace Neon::RHI
{
struct SamplerDesc
{
    TextureFilter minFilter = TextureFilter::Linear;
    TextureFilter magFilter = TextureFilter::Linear;
    MipmapFilter mipmapFilter = MipmapFilter::None;

    struct WrapMode
    {
        TextureWrap x = TextureWrap::ClampToEdge;
        TextureWrap y = TextureWrap::ClampToEdge;
        TextureWrap z = TextureWrap::ClampToEdge;
    };

    WrapMode wrapMode = WrapMode{};

    float lodBias = 0.0f;
};
}

#pragma once
#include <cstdint>

#include "enums/mipmapFilter.h"
#include "enums/textureFilter.h"
#include "enums/textureFormat.h"
#include "enums/textureUsage.h"
#include "enums/textureWrap.h"

namespace NRHI
{
struct SamplerDescription
{
    TextureFilter minFilter = TextureFilter::Linear;
    TextureFilter magFilter = TextureFilter::Linear;
    MipmapFilter mipmapFilter = MipmapFilter::Linear;

    struct WrapMode
    {
        TextureWrap x = TextureWrap::Repeat;
        TextureWrap y = TextureWrap::Repeat;
        TextureWrap z = TextureWrap::Repeat;
    };

    WrapMode wrapMode = WrapMode{};

    float lodBias = 0.0f;
};
}

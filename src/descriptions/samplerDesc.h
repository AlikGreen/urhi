#pragma once

#include "enums/addressMode.h"
#include "enums/borderColor.h"
#include "enums/compareOp.h"
#include "enums/mipmapFilter.h"
#include "enums/textureFilter.h"

namespace urhi
{
struct SamplerDesc
{
    TextureFilter minFilter = TextureFilter::Linear;
    TextureFilter magFilter = TextureFilter::Linear;
    MipmapFilter mipmapFilter = MipmapFilter::Linear;

    AddressMode addressModeU = AddressMode::ClampToEdge;
    AddressMode addressModeV = AddressMode::ClampToEdge;
    AddressMode addressModeW = AddressMode::ClampToEdge;

    BorderColor borderColor = BorderColor::OpaqueBlack;

    float lodBias = 0.0f;

    bool enableCompare = false;
    CompareOp compareOp = CompareOp::Always;

    bool unnormalizedCoordinates = false;
};
}

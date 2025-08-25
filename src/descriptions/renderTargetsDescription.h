#pragma once
#include "enums/textureFormat.h"

namespace NRHI
{
struct RenderTargetsDescription
{
    std::vector<TextureFormat> colorTargetFormats = {};
    TextureFormat depthTargetFormat = TextureFormat::Invalid;
};
}

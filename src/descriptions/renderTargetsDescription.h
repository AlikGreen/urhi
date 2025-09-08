#pragma once
#include "enums/textureFormat.h"

namespace Neon::RHI
{
struct RenderTargetsDescription
{
    std::vector<TextureFormat> colorTargetFormats = {};
    TextureFormat depthTargetFormat = TextureFormat::Invalid;
};
}

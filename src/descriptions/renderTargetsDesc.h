#pragma once

#include "enums/pixelFormat.h"

namespace Neon::RHI
{
struct RenderTargetsDesc
{
    std::vector<PixelFormat> colorTargetFormats = {};
    PixelFormat depthTargetFormat = PixelFormat::Invalid;
};
}

#pragma once

#include "enums/pixelFormat.h"

namespace Neon::RHI
{
struct RenderTargetsDescription
{
    std::vector<PixelFormat> colorTargetFormats = {};
    PixelFormat depthTargetFormat = PixelFormat::Invalid;
};
}

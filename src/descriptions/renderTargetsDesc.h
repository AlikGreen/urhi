#pragma once

#include "enums/pixelFormat.h"

namespace urhi
{
struct RenderTargetsDesc
{
    std::vector<PixelFormat> colorTargetFormats = {};
    PixelFormat depthTargetFormat = PixelFormat::Invalid;
};
}

#pragma once
#include "blendState.h"
#include "enums/pixelFormat.h"

namespace urhi
{
struct ColorAttachmentDesc
{
    PixelFormat format = PixelFormat::Unknown;
    BlendState blend = BlendState::opaque();
};
}

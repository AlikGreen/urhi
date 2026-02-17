#pragma once
#include "blendState.h"
#include "enums/pixelFormat.h"

namespace urhi
{
struct ColorAttachmentDesc
{
    PixelFormat format = PixelFormat::Undefined;
    BlendState blend = BlendState::opaque();
};
}

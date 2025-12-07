#pragma once
#include "enums/blendFactor.h"
#include "enums/blendOp.h"

namespace Neon::RHI
{
    struct BlendState
    {
        bool enableBlend = false;

        BlendFactor srcColorFactor = BlendFactor::One;
        BlendFactor dstColorFactor = BlendFactor::Zero;
        BlendOp colorOp = BlendOp::Add;

        BlendFactor srcAlphaFactor = BlendFactor::One;
        BlendFactor dstAlphaFactor = BlendFactor::Zero;
        BlendOp alphaOp = BlendOp::Add;
    };
}
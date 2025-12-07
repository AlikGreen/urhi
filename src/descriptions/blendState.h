#pragma once
#include "enums/blendFactor.h"
#include "enums/blendOp.h"

namespace Neon::RHI
{
    struct BlendState
    {
        bool enableBlend = false;

        BlendFactor srcColorFactor = BlendFactor::SrcAlpha;
        BlendFactor dstColorFactor = BlendFactor::InvSrcAlpha;
        BlendOp colorOp            = BlendOp::Add;

        BlendFactor srcAlphaFactor = BlendFactor::One;
        BlendFactor dstAlphaFactor = BlendFactor::InvSrcAlpha;
        BlendOp alphaOp            = BlendOp::Add;
    };
}
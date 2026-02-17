#pragma once
#include "enums/blendFactor.h"
#include "enums/blendOp.h"
#include "enums/colorWriteMask.h"

namespace urhi
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

    ColorWriteMask writeMask = ColorWriteMask::All;

    static BlendState opaque()
    {
        BlendState state;
        state.enableBlend = false;
        state.srcColorFactor = BlendFactor::One;
        state.dstColorFactor = BlendFactor::Zero;
        state.colorOp = BlendOp::Add;
        state.srcAlphaFactor = BlendFactor::One;
        state.dstAlphaFactor = BlendFactor::Zero;
        state.alphaOp = BlendOp::Add;
        state.writeMask = ColorWriteMask::All;
        return state;
    }

    static BlendState alphaBlend()
    {
        BlendState state;
        state.enableBlend = true;
        state.srcColorFactor = BlendFactor::SrcAlpha;
        state.dstColorFactor = BlendFactor::InvSrcAlpha;
        state.colorOp = BlendOp::Add;
        state.srcAlphaFactor = BlendFactor::One;
        state.dstAlphaFactor = BlendFactor::InvSrcAlpha;
        state.alphaOp = BlendOp::Add;
        state.writeMask = ColorWriteMask::All;
        return state;
    }

    static BlendState additive()
    {
        BlendState state;
        state.enableBlend = true;
        state.srcColorFactor = BlendFactor::One;
        state.dstColorFactor = BlendFactor::One;
        state.colorOp = BlendOp::Add;
        state.srcAlphaFactor = BlendFactor::One;
        state.dstAlphaFactor = BlendFactor::One;
        state.alphaOp = BlendOp::Add;
        state.writeMask = ColorWriteMask::All;
        return state;
    }

    static BlendState premultipliedAlpha()
    {
        BlendState state;
        state.enableBlend = true;
        state.srcColorFactor = BlendFactor::One;
        state.dstColorFactor = BlendFactor::InvSrcAlpha;
        state.colorOp = BlendOp::Add;
        state.srcAlphaFactor = BlendFactor::One;
        state.dstAlphaFactor = BlendFactor::InvSrcAlpha;
        state.alphaOp = BlendOp::Add;
        state.writeMask = ColorWriteMask::All;
        return state;
    }

    static BlendState writeNone()
    {
        BlendState state;
        state.enableBlend = false;
        state.writeMask = ColorWriteMask::None;
        return state;
    }
};
}

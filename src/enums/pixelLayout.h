#pragma once

namespace Neon::RHI
{
enum class PixelLayout
{
    R,
    RG,
    RGB,
    BGR,
    RGBA,
    BGRA,
    Depth,
    Stencil,
    DepthStencil,

    RInt,
    RGInt,
    RGBInt,
    RGBAInt,
    BGRInt,
    BGRAInt
};
}

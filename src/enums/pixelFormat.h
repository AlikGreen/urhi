#pragma once

namespace urhi
{
enum class PixelFormat : uint16_t
{
    Unknown = 0,

    // 8-bit
    R8UNorm,
    R8SNorm,
    R8UInt,
    R8SInt,

    // 16-bit
    R16UInt,
    R16SInt,
    R16Float,

    RG8UNorm,
    RG8SNorm,
    RG8UInt,
    RG8SInt,

    // 32-bit
    R32UInt,
    R32SInt,
    R32Float,

    RG16UInt,
    RG16SInt,
    RG16Float,

    RGBA8UNorm,
    RGBA8UNormSrgb,
    RGBA8SNorm,
    RGBA8UInt,
    RGBA8SInt,

    BGRA8UNorm,
    BGRA8UNormSrgb,

    // Packed 32-bit
    RGB9E5UFloat,
    RGB10A2UInt,
    RGB10A2UNorm,
    RG11B10UFloat,

    // 64-bit
    RG32UInt,
    RG32SInt,
    RG32Float,

    RGBA16UInt,
    RGBA16SInt,
    RGBA16Float,

    // 128-bit
    RGBA32UInt,
    RGBA32SInt,
    RGBA32Float,

    // Depth / stencil
    Depth16UNorm,
    Depth32Float,
    Stencil8,
    Depth24PlusStencil8,
};
}

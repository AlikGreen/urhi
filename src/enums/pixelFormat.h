#pragma once
#include <cstdint>

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

constexpr std::uint32_t bytesPerPixel(const PixelFormat format)
{
    switch (format)
    {
    case PixelFormat::R8UNorm:
    case PixelFormat::R8SNorm:
    case PixelFormat::R8UInt:
    case PixelFormat::R8SInt:
    case PixelFormat::Stencil8:
        return 1;

    case PixelFormat::R16UInt:
    case PixelFormat::R16SInt:
    case PixelFormat::R16Float:
    case PixelFormat::RG8UNorm:
    case PixelFormat::RG8SNorm:
    case PixelFormat::RG8UInt:
    case PixelFormat::RG8SInt:
    case PixelFormat::Depth16UNorm:
        return 2;

    case PixelFormat::R32UInt:
    case PixelFormat::R32SInt:
    case PixelFormat::R32Float:
    case PixelFormat::RG16UInt:
    case PixelFormat::RG16SInt:
    case PixelFormat::RG16Float:
    case PixelFormat::RGBA8UNorm:
    case PixelFormat::RGBA8UNormSrgb:
    case PixelFormat::RGBA8SNorm:
    case PixelFormat::RGBA8UInt:
    case PixelFormat::RGBA8SInt:
    case PixelFormat::BGRA8UNorm:
    case PixelFormat::BGRA8UNormSrgb:
    case PixelFormat::RGB9E5UFloat:
    case PixelFormat::RGB10A2UInt:
    case PixelFormat::RGB10A2UNorm:
    case PixelFormat::RG11B10UFloat:
    case PixelFormat::Depth32Float:
    case PixelFormat::Depth24PlusStencil8:
        return 4;

    case PixelFormat::RG32UInt:
    case PixelFormat::RG32SInt:
    case PixelFormat::RG32Float:
    case PixelFormat::RGBA16UInt:
    case PixelFormat::RGBA16SInt:
    case PixelFormat::RGBA16Float:
        return 8;

    case PixelFormat::RGBA32UInt:
    case PixelFormat::RGBA32SInt:
    case PixelFormat::RGBA32Float:
        return 16;

    case PixelFormat::Unknown:
    default:
        return 0;
    }
}
}

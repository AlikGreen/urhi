#include "urhiToString.h"

namespace urhi
{
    std::string toString(const PixelFormat format)
    {
        switch (format)
        {
            // 8-bit R
            case PixelFormat::R8UNorm:          return "R8UNorm";
            case PixelFormat::R8SNorm:          return "R8SNorm";
            case PixelFormat::R8UInt:           return "R8UInt";
            case PixelFormat::R8SInt:           return "R8SInt";

            // 16-bit R
            case PixelFormat::R16UInt:          return "R16UInt";
            case PixelFormat::R16SInt:          return "R16SInt";
            case PixelFormat::R16Float:         return "R16Float";

            // 32-bit R
            case PixelFormat::R32Float:         return "R32Float";
            case PixelFormat::R32UInt:          return "R32UInt";
            case PixelFormat::R32SInt:          return "R32SInt";

            // 8-bit RG
            case PixelFormat::RG8UNorm:         return "RG8UNorm";
            case PixelFormat::RG8SNorm:         return "RG8SNorm";
            case PixelFormat::RG8UInt:          return "RG8UInt";
            case PixelFormat::RG8SInt:          return "RG8SInt";

            // 16-bit RG
            case PixelFormat::RG16UInt:         return "RG16UInt";
            case PixelFormat::RG16SInt:         return "RG16SInt";
            case PixelFormat::RG16Float:        return "RG16Float";

            // 32-bit RG
            case PixelFormat::RG32Float:        return "RG32Float";
            case PixelFormat::RG32UInt:         return "RG32UInt";
            case PixelFormat::RG32SInt:         return "RG32SInt";

            // 8-bit RGBA / BGRA
            case PixelFormat::RGBA8UNorm:       return "RGBA8UNorm";
            case PixelFormat::RGBA8SNorm:       return "RGBA8SNorm";
            case PixelFormat::RGBA8UInt:        return "RGBA8UInt";
            case PixelFormat::RGBA8SInt:        return "RGBA8SInt";
            case PixelFormat::RGBA8UNormSrgb:   return "RGBA8UNormSrgb";
            case PixelFormat::BGRA8UNorm:       return "BGRA8UNorm";
            case PixelFormat::BGRA8UNormSrgb:   return "BGRA8UNormSrgb";

            // 16-bit RGBA
            case PixelFormat::RGBA16UInt:       return "RGBA16UInt";
            case PixelFormat::RGBA16SInt:       return "RGBA16SInt";
            case PixelFormat::RGBA16Float:      return "RGBA16Float";

            // 32-bit RGBA
            case PixelFormat::RGBA32Float:      return "RGBA32Float";
            case PixelFormat::RGBA32UInt:       return "RGBA32UInt";
            case PixelFormat::RGBA32SInt:       return "RGBA32SInt";

            // Packed / Special
            case PixelFormat::RGB10A2UNorm:     return "RGB10A2UNorm";
            case PixelFormat::RG11B10UFloat:    return "RG11B10UFloat";
            case PixelFormat::RGB9E5UFloat:     return "RGB9E5UFloat";
            case PixelFormat::RGB10A2UInt:      return "RGB10A2UInt";

            // Depth / Stencil
            case PixelFormat::Depth24PlusStencil8: return "Depth24PlusStencil8";
            case PixelFormat::Depth32Float:        return "Depth32Float";
            case PixelFormat::Depth16UNorm:        return "Depth16UNorm";
            case PixelFormat::Stencil8:            return "Stencil8";

            case PixelFormat::Unknown:
            default:                               return "Unknown";
        }
    }
}

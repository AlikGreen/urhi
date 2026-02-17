#pragma once
#include <cstdint>

namespace urhi
{
    enum class TextureUsage : uint8_t
    {
        None                = 0u,
        Sampled             = 1u << 0,
        ColorTarget         = 1u << 1,
        DepthStencilTarget  = 1u << 2,
        Storage             = 1u << 3,
    };

    constexpr TextureUsage operator|(TextureUsage a, TextureUsage b)
    {
        return static_cast<TextureUsage>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }

    constexpr TextureUsage& operator|=(TextureUsage& a, const TextureUsage b)
    {
        a = a | b;
        return a;
    }

    constexpr TextureUsage operator&(TextureUsage a, TextureUsage b)
    {
        return static_cast<TextureUsage>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
    }

    constexpr TextureUsage& operator&=(TextureUsage& a, const TextureUsage b)
    {
        a = a & b;
        return a;
    }
}

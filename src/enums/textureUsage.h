#pragma once
#include <cstdint>

namespace Neon::RHI
{
    enum class TextureUsage : uint32_t
    {
        Sampled = 1u << 0,
        ColorTarget = 1u << 1,
        DepthStencilTarget = 1u << 2,
        GraphicsStorageRead = 1u << 3,
        ComputeStorageRead = 1u << 4,
        ComputeStorageWrite = 1u << 5,
    };

    constexpr TextureUsage operator|(TextureUsage a, TextureUsage b) {
        return static_cast<TextureUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    constexpr TextureUsage& operator|=(TextureUsage& a, const TextureUsage b) {
        a = a | b;
        return a;
    }
}

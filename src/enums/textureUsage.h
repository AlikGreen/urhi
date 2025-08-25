#pragma once
#include <cstdint>

namespace NRHI
{
    enum class TextureUsage : uint32_t {
        Sampler = 1u << 0,
        ColorTarget = 1u << 1,
        DepthStencilTarget = 1u << 2,
        GraphicsStorageRead = 1u << 3,
        ComputeStorageRead = 1u << 4,
        ComputeStorageWrite = 1u << 5,
        ComputeStorageSimultaneousReadWrite = 1u << 6
    };

    constexpr TextureUsage operator|(TextureUsage a, TextureUsage b) {
        return static_cast<TextureUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    constexpr TextureUsage& operator|=(TextureUsage& a, const TextureUsage b) {
        a = a | b;
        return a;
    }
}

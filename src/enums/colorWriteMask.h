#pragma once
#include <cstdint>

namespace urhi
{
enum class ColorWriteMask : uint8_t
{
    None = 0u,
    R    = 1u << 0,
    G    = 1u << 1,
    B    = 1u << 2,
    A    = 1u << 3,
    All  = R | G | B | A,
};

inline ColorWriteMask operator|(ColorWriteMask a, ColorWriteMask b)
{
    return static_cast<ColorWriteMask>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline ColorWriteMask operator&(ColorWriteMask a, ColorWriteMask b)
{
    return static_cast<ColorWriteMask>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}
}

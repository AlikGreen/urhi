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

URHI_DEFINE_ENUM_FLAGS(ColorWriteMask);
}

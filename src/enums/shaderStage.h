#pragma once
#include "enumFlags.h"

namespace urhi
{
enum class ShaderStage : uint32_t
{
    Vertex   = 1u << 0,
    Fragment = 1u << 1,
    Compute  = 1u << 2,
    Geometry = 1u << 3,
    None     = 1u << 4
};


URHI_DEFINE_ENUM_FLAGS(ShaderStage);
}

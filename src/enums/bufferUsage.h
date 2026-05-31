#pragma once
#include "enumFlags.h"

namespace urhi
{
enum class BufferUsage
{
    None     = 0,
    Vertex   = 1 << 0,
    Index    = 1 << 1,
    Uniform  = 1 << 2,
    Storage  = 1 << 3,
    Indirect = 1 << 4,
    Static = 1 << 5,
    Dynamic  = 1 << 6,
};

URHI_DEFINE_ENUM_FLAGS(BufferUsage);
}

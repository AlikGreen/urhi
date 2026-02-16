#pragma once
#include <cstdint>
#include <variant>

namespace urhi
{
struct ClearColorFloat  { float r, g, b, a; };
struct ClearColorInt    { int32_t r, g, b, a; };
struct ClearColorUint   { uint32_t r, g, b, a; };

using ClearValue = std::variant<
    ClearColorFloat,
    ClearColorInt,
    ClearColorUint
>;
}

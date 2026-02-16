#pragma once

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


inline ShaderStage operator|(ShaderStage lhs, ShaderStage rhs)
{
    using U = std::underlying_type_t<ShaderStage>;
    return static_cast<ShaderStage>(static_cast<U>(lhs) | static_cast<U>(rhs));
}

inline ShaderStage operator&(ShaderStage lhs, ShaderStage rhs)
{
    using U = std::underlying_type_t<ShaderStage>;
    return static_cast<ShaderStage>(static_cast<U>(lhs) & static_cast<U>(rhs));
}

inline ShaderStage& operator|=(ShaderStage &lhs, const ShaderStage rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

inline ShaderStage& operator&=(ShaderStage &lhs, const ShaderStage rhs)
{
    lhs = lhs & rhs;
    return lhs;
}
}

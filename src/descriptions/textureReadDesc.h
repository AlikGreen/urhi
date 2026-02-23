#pragma once
#include <cstdint>

namespace urhi
{
struct TextureReadDesc
{
    uint32_t mipLevel = 0;

    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t z = 0;
    uint32_t width  = 1;
    uint32_t height = 1;
    uint32_t depth  = 1;
};
}

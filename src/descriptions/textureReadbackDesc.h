#pragma once
#include <cstdint>
#include <grl/mem.h>

#include "texture.h"

namespace urhi
{
struct TextureReadbackDesc
{
    grl::Rc<Texture> texture;

    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t z = 0;
    uint32_t width  = 1;
    uint32_t height = 1;
    uint32_t depth  = 1; // or layer count

    uint32_t mipLevel = 0;
    uint32_t baseArrayLayer = 0;
};
}

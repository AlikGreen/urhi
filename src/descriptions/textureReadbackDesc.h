#pragma once
#include <cstdint>
#include <grl/mem.h>

#include "texture.h"

namespace urhi
{
struct TextureReadbackDesc
{
    grl::Rc<Texture> texture;

    int32_t x = 0;
    int32_t y = 0;
    int32_t z = 0;
    uint32_t width  = ~0u;
    uint32_t height = ~0u;
    uint32_t depth  = ~0u;

    uint32_t mipLevel = 0;
    uint32_t baseArrayLayer = 0;
    uint32_t arrayLayerCount = 1;
};
}

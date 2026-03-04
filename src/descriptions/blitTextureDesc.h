#pragma once
#include <grl/mem.h>

#include "texture.h"
#include "enums/textureFilter.h"
#include "glm/vec3.hpp"

namespace urhi
{
struct BlitTextureDesc
{
    grl::Rc<Texture> src;
    grl::Rc<Texture> dst;

    uint32_t    srcMipLevel   = 0;
    uint32_t    srcArrayLayer = 0;
    glm::uvec3  srcOffset     = {0, 0, 0};
    glm::ivec3  srcExtent     = {0, 0, 0};

    uint32_t    dstMipLevel   = 0;
    uint32_t    dstArrayLayer = 0;
    glm::uvec3  dstOffset     = {0, 0, 0};
    glm::ivec3  dstExtent     = {0, 0, 0};

    TextureFilter filter = TextureFilter::Linear;
};
}

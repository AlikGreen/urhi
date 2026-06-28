#pragma once
#include <cstdint>
#include <grl/mem.h>

#include "sampler.h"
#include "textureView.h"

namespace urhi
{
class TextureArray
{
public:
    TextureArray() = default;
    virtual ~TextureArray() = default;

    TextureArray(const TextureArray&) = delete;
    TextureArray& operator= (const TextureArray&) = delete;

    virtual void set(uint32_t index, grl::Rc<TextureView> texture, grl::Rc<Sampler> sampler) = 0;
};
}

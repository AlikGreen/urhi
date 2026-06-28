#pragma once
#include "textureArray.h"

namespace urhi
{
class GlTextureArray final : public TextureArray
{
public:
    void set(uint32_t index, grl::Rc<TextureView> texture, grl::Rc<Sampler> sampler) override;
private:

};
}

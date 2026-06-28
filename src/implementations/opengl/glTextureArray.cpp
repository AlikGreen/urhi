#include "glTextureArray.h"

#include <glad/gl.h>

#include "glSampler.h"
#include "glTextureView.h"

namespace urhi
{
    void GlTextureArray::set(uint32_t index, grl::Rc<TextureView> texture, grl::Rc<Sampler> sampler)
    {
        const auto* glTexture = static_cast<GlTextureView*>(texture.get());
        const auto* glSampler = static_cast<GlSampler*>(sampler.get());
        const GLuint64 handle = glGetTextureSamplerHandleARB(glTexture->handle(), glSampler->handle());


    }
}

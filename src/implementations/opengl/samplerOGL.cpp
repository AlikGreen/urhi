#include "samplerOGL.h"

#include <cstdio>
#include <glad/gl.h>

#include "convertOGL.h"

namespace Neon::RHI
{
    SamplerOGL::SamplerOGL(const SamplerDescription &description)
    {
        glCreateSamplers(1, &handle);

        glSamplerParameteri(handle, GL_TEXTURE_WRAP_S, static_cast<int>(ConvertOGL::textureWrapToGL(description.wrapMode.x)));
        glSamplerParameteri(handle, GL_TEXTURE_WRAP_T, static_cast<int>(ConvertOGL::textureWrapToGL(description.wrapMode.y)));
        glSamplerParameteri(handle, GL_TEXTURE_MIN_FILTER, static_cast<int>(ConvertOGL::minFilterToGL(description.minFilter, description.mipmapFilter)));
        glSamplerParameteri(handle, GL_TEXTURE_MAG_FILTER, static_cast<int>(ConvertOGL::magFilterToGL(description.magFilter)));
        glSamplerParameterf(handle, GL_TEXTURE_LOD_BIAS, description.lodBias);
        glSamplerParameteri(handle, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    }

    SamplerOGL::~SamplerOGL()
    {
        glDeleteSamplers(1, &handle);
    }

    void SamplerOGL::bind(const uint32_t binding) const
    {
        glBindSampler(binding, handle);
    }
}

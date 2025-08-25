#include "samplerOGL.h"

#include <glad/glad.h>

#include "convertOGL.h"

namespace NRHI
{
    SamplerOGL::SamplerOGL(const SamplerDescription &description)
    {
        glGenSamplers(1, &handle);

        glSamplerParameteri(handle, GL_TEXTURE_WRAP_S, static_cast<int>(ConvertOGL::textureWrapToGL(description.wrapMode.x)));
        glSamplerParameteri(handle, GL_TEXTURE_WRAP_T, static_cast<int>(ConvertOGL::textureWrapToGL(description.wrapMode.y)));
        glSamplerParameteri(handle, GL_TEXTURE_MIN_FILTER, static_cast<int>(ConvertOGL::textureFilterCombineToGL(description.minFilter, description.mipmapFilter)));
        glSamplerParameteri(handle, GL_TEXTURE_MAG_FILTER, static_cast<int>(ConvertOGL::textureFilterCombineToGL(description.magFilter, description.mipmapFilter)));
        glSamplerParameterf(handle, GL_TEXTURE_LOD_BIAS, description.lodBias);
        glSamplerParameteri(handle, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    }

    void SamplerOGL::bind(const uint32_t binding) const
    {
        glBindSampler(binding, handle);
    }
}

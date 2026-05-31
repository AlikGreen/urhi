#include "glSampler.h"

#include "glConvert.h"
#include "glDevice.h"

namespace urhi
{
    GlSampler::GlSampler(GlDevice *device, const SamplerDesc &desc)
        : m_device(device)
    {
        glCreateSamplers(1, &m_handle);

        glSamplerParameteri(m_handle, GL_TEXTURE_MAG_FILTER, GlConvert::filter(desc.magFilter));
        glSamplerParameteri(m_handle, GL_TEXTURE_MIN_FILTER, GlConvert::filter(desc.minFilter, desc.mipmapFilter));

        glSamplerParameteri(m_handle, GL_TEXTURE_WRAP_S, GlConvert::wrapMode(desc.addressModeU));
        glSamplerParameteri(m_handle, GL_TEXTURE_WRAP_T, GlConvert::wrapMode(desc.addressModeV));
        glSamplerParameteri(m_handle, GL_TEXTURE_WRAP_R, GlConvert::wrapMode(desc.addressModeW));

        glSamplerParameterf(m_handle, GL_TEXTURE_MAX_ANISOTROPY, device->maxAnisotropy());
        glSamplerParameterf(m_handle, GL_TEXTURE_LOD_BIAS, desc.lodBias);

        glSamplerParameteri(m_handle, GL_TEXTURE_COMPARE_MODE, desc.enableCompare ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE);
        glSamplerParameteri(m_handle, GL_TEXTURE_COMPARE_FUNC, GlConvert::compareFunc(desc.compareOp));
    }

    GlSampler::~GlSampler()
    {
        glDeleteSamplers(1, &m_handle);
    }
}

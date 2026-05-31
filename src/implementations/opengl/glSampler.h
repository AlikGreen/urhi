#pragma once
#include "sampler.h"
#include "descriptions/samplerDesc.h"

#include <glad/gl.h>

namespace urhi
{
class GlDevice;
class GlSampler final : public Sampler
{
public:
    GlSampler(GlDevice* device, const SamplerDesc& desc);
    ~GlSampler() override;

    [[nodiscard]] GLuint handle() const { return m_handle; }
private:
    GlDevice* m_device;
    GLuint m_handle;
};
}

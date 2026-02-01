#pragma once
#include "sampler.h"
#include "descriptions/samplerDesc.h"

#include <cstdint>
#include <glad/gl.h>

namespace Neon::RHI
{
class SamplerOGL final : public Sampler
{
public:
    explicit SamplerOGL(const SamplerDesc &description);
    ~SamplerOGL();

    void bind(uint32_t binding) const;
private:
    GLuint handle{};
};
}

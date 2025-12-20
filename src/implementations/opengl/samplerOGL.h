#pragma once
#include "sampler.h"
#include "descriptions/samplerDescription.h"

#include <cstdint>
#include <glad/glad.h>

namespace Neon::RHI
{
class SamplerOGL final : public Sampler
{
public:
    explicit SamplerOGL(const SamplerDescription &description);
    ~SamplerOGL();

    void bind(uint32_t binding) const;
private:
    GLuint handle{};
};
}

#pragma once
#include "sampler.h"
#include "descriptions/samplerDescription.h"

#include <glad/glad.h>

namespace NRHI
{
class SamplerOGL : public Sampler
{
public:
    explicit SamplerOGL(const SamplerDescription &description);

    void bind(uint32_t binding) const;
private:
    GLuint handle{};
};
}

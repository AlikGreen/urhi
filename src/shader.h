#pragma once
#include "descriptions/shaderReflection.h"

namespace urhi
{
class Shader
{
public:
    Shader() = default;
    virtual ~Shader() = default;

    Shader(const Shader&) = delete;
    Shader& operator= (const Shader&) = delete;

    virtual ShaderReflection reflection() = 0;
};
}

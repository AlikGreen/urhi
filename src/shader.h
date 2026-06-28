#pragma once
#include "descriptions/shaderReflection.h"
#include "enums/shaderStage.h"

namespace urhi
{
class Shader
{
public:
    Shader() = default;
    virtual ~Shader() = default;

    Shader(const Shader&) = delete;
    Shader& operator= (const Shader&) = delete;

    virtual refl::Data reflection() = 0;
    virtual ShaderStage stage() = 0;
};
}

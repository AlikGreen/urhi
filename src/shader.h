#pragma once
#include "descriptions/shaderReflection.h"

namespace Neon::RHI
{
class Shader
{
public:
    virtual ~Shader() = default;

    virtual void compile() = 0;
    virtual void dispose() = 0;

    virtual ShaderReflection getShaderReflection() = 0;
};
}

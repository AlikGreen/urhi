#pragma once
#include "descriptions/shaderEntryPoint.h"

namespace urhi
{
class Shader
{
public:
    Shader() = default;
    virtual ~Shader() = default;

    Shader(const Shader&) = delete;
    Shader& operator= (const Shader&) = delete;

    virtual ShaderEntryPoint entryPoint() = 0;
};
}

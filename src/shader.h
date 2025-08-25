#pragma once

namespace NRHI
{
class Shader
{
public:
    virtual ~Shader() = default;

    virtual void compile() = 0;
    virtual void dispose() = 0;
    // TODO implement more shader types
};
}

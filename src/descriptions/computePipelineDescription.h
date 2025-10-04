#pragma once

#include "shader.h"
#include "glm/glm.hpp"

namespace Neon::RHI
{
struct ComputePipelineDescription
{
    Shader* shader = nullptr;

    glm::ivec3 threadGroupSize{1};
};
}

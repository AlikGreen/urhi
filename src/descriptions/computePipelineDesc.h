#pragma once
#include <neonCore/neonCore.h>

#include "shader.h"
#include "glm/glm.hpp"

namespace Neon::RHI
{
struct ComputePipelineDesc
{
    Rc<Shader> shader = nullptr;

    glm::ivec3 threadGroupSize{1};
};
}

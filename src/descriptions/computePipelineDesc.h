#pragma once
#include <grl/grl.h>

#include "shader.h"
#include "glm/glm.hpp"

namespace urhi
{
struct ComputePipelineDesc
{
    grl::Rc<Shader> shader = nullptr;
};
}

#pragma once
#include <cstdint>
#include <vector>

#include "shaderReflection.h"
#include "enums/shaderStage.h"

namespace urhi
{
struct ShaderEntryPoint
{
    ShaderStage stage;
    std::string name;
    std::vector<uint32_t> spirvCode;
    refl::Data reflection;
};
}

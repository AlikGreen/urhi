#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "shaderReflection.h"
#include "enums/shaderStage.h"

namespace urhi
{
struct ShaderEntryPoint
{
    std::string name{};
    ShaderStage stage;
    std::vector<uint32_t> spirv;
    ShaderReflection reflection;
};
}

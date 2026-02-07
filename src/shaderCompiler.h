#pragma once
#include <cstdint>
#include <slang.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "descriptions/shaderReflection.h"

namespace urhi
{
    struct ShaderCompileDescription
    {
        std::string path;
        std::string source;
        std::vector<std::string> includePaths;
    };

    struct CompiledShader
    {
        std::vector<uint32_t> spirv;
        ShaderReflection reflection;
    };

    class ShaderCompiler
    {
    public:
        static CompiledShader compile(const ShaderCompileDescription& compileDesc);
    };
}

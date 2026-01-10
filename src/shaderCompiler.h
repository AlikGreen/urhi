#pragma once
#include <cstdint>
#include <slang.h>
#include <string>
#include <vector>


namespace Neon::RHI
{
struct ShaderCompileDescription
{
    std::string source;
    std::string path;
    std::vector<std::string> includePaths;
};

class ShaderCompiler
{
public:
    static std::vector<uint32_t> compile(const ShaderCompileDescription& compileDesc);
};
}

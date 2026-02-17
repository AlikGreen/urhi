#pragma once
#include <cstdint>
#include <slang-com-ptr.h>
#include <slang.h>
#include <string>
#include <vector>

#include "descriptions/shaderEntryPoint.h"
#include "descriptions/shaderReflection.h"

namespace urhi
{
    struct ShaderCompileDesc
    {
        std::string path;
        std::string source;
        std::vector<std::string> includePaths;
        std::string computeEntryPoint;

        bool generateDebugInfo = false;
    };

    class ShaderCompiler
    {
    public:
        static std::vector<ShaderEntryPoint> compile(const ShaderCompileDesc& desc);
    private:
        static Slang::ComPtr<slang::IComponentType> compileAndLink(slang::ISession* session, const ShaderCompileDesc& desc);
        static std::vector<uint32_t> extractSpirv(slang::IComponentType* linkedProgram);
        static ShaderReflection extractReflection(slang::ProgramLayout* layout, slang::EntryPointReflection* entryPoint, const ShaderCompileDesc& desc);

        static void extractVertexInput(slang::EntryPointReflection* entryPoint, const ShaderCompileDesc& desc, ShaderReflection& reflection);
        static void extractResources(slang::ProgramLayout* layout, ShaderReflection& reflection);
        static void extractConstantBuffer(slang::VariableLayoutReflection* param, slang::TypeReflection* type, slang::TypeLayoutReflection* typeLayout, uint32_t set, uint32_t binding, ShaderReflection& reflection);
        static void extractStructuredBuffer(slang::VariableLayoutReflection* param, slang::TypeReflection* type, uint32_t set, uint32_t binding, ShaderReflection& reflection);
        static void extractTexture(slang::VariableLayoutReflection* param, slang::TypeReflection* type, uint32_t set, uint32_t binding, ShaderReflection& reflection);
        static void extractSampler(slang::VariableLayoutReflection* param, uint32_t set, uint32_t binding, ShaderReflection& reflection);

        // Type helpers
        static ShaderReflection::DataType convertDataType(slang::TypeReflection* type);
        static TextureType convertTextureDimension(SlangResourceShape shape);
        static ShaderStage convertStage(SlangStage stage);

        // Struct layout
        static std::vector<ShaderReflection::Member> extractMembers(slang::TypeReflection* structType, slang::TypeLayoutReflection* structTypeLayout);
    };

}

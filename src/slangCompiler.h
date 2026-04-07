#pragma once
#include <slang-com-ptr.h>
#include <string>
#include <vector>

#include "shaderSet.h"

namespace urhi
{
struct ShaderModuleDesc
{
    std::string name;
    std::string path;
    std::string source;
};

struct SlangCompileDesc
{
    std::vector<ShaderModuleDesc> modules;
    std::vector<std::string> entryPoints;

    std::vector<std::string> includePaths;

    std::vector<std::string> typeSpecializations;
};

class SlangCompiler
{
public:
    static ShaderSet compile(const SlangCompileDesc& desc);
private:
    SlangCompiler() = default;
    ShaderSet compileImpl(const SlangCompileDesc& desc);
    void compileAndLink(const SlangCompileDesc& desc);
    void createSession(const SlangCompileDesc& desc);

    [[nodiscard]] std::vector<uint32_t> extractSpirv(uint32_t index) const;

    static ShaderReflection extractReflection(slang::ProgramLayout* layout, size_t entryPointIndex);

    static void extractVertexInput(slang::VariableLayoutReflection* param, ShaderReflection &result);
    static void extractBinding(slang::VariableLayoutReflection *param, ShaderReflection &result);
    static void extractMembers(slang::TypeLayoutReflection *typeLayout, std::vector<ShaderReflection::Member> &members);

    static void extractPushConstants(slang::ProgramLayout *layout, ShaderReflection &result);

    static void extractParameterBlock(slang::VariableLayoutReflection *param, slang::TypeLayoutReflection *typeLayout, ShaderReflection &result);
    static void extractUniformMembers(slang::TypeLayoutReflection *typeLayout, std::vector<ShaderReflection::Member> &members);
    static void extractParameterBlockResources(slang::TypeLayoutReflection *typeLayout, uint32_t set, uint32_t baseBinding, const std::string &prefix, ShaderReflection &result);

    static ShaderReflection::DataType convertDataType(slang::TypeLayoutReflection *typeLayout);
    static ShaderStage convertStage(SlangStage slangStage);
    static TextureType convertTextureType(SlangResourceShape shape);
    static uint32_t getDataTypeByteSize(ShaderReflection::DataType type);

    Slang::ComPtr<slang::ISession> m_session;
    std::vector<Slang::ComPtr<slang::IEntryPoint>> m_entryPoints;
    Slang::ComPtr<slang::IComponentType> m_linked;

};
}

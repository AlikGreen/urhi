#pragma once
#include <slang-com-ptr.h>
#include <string>
#include <vector>
#include <slang.h>

#include "shaderSet.h"

namespace urhi::slang
{
struct Module
{
    std::string name;
    std::string path;
    std::vector<uint8_t> ir;
};

struct Diagnostics
{
    bool hasErrors = false;
    std::string text;
};

struct CompileDesc
{
    std::string moduleName;
    std::string modulePath;
    std::string moduleSource;
    std::vector<std::string> includePaths;
    std::vector<std::string> defines;
    bool enableDebugInfo = false;
    bool optimize = true;
};

struct LinkDesc
{
    std::vector<Module> modules;
    std::vector<std::string> entryPoints;
    std::vector<std::string> typeSpecializations;
};

class Compiler
{
public:
    static Module compileModule(const CompileDesc& desc, Diagnostics* outDiag = nullptr);
    static std::vector<ShaderReflection::Resource> getResources(const std::vector<Module>& modules, Diagnostics* outDiag = nullptr);
    static ShaderSet linkToShaderSet(const LinkDesc& desc, Diagnostics* outDiag = nullptr);

    static ::slang::IGlobalSession* getGlobalSession();
private:
    static Slang::ComPtr<::slang::ISession> getSession();

    static std::tuple<std::vector<Slang::ComPtr<::slang::IEntryPoint>>, Slang::ComPtr<::slang::IComponentType>, Slang::ComPtr<::slang::ISession>> compileAndLink(const LinkDesc &desc, Diagnostics *outDiags);

    static void appendDiagnostics(Diagnostics* outDiag, ::slang::IBlob* diagBlob);
    static void appendDiagnostics(Diagnostics *outDiag, const std::string &text);
    static void appendDiagnostics(Diagnostics *outDiag, const std::string &text, bool hasErrors);

    static Slang::ComPtr<::slang::IModule> loadModuleFromIR(::slang::ISession* session, const Module& module, Diagnostics* diags);


    static [[nodiscard]] std::vector<uint32_t> extractSpirv(const Slang::ComPtr<::slang::IComponentType> &linked, uint32_t index, Diagnostics* outDiags);
    static ShaderReflection extractReflection(::slang::ProgramLayout* layout, size_t entryPointIndex);

    static void extractVertexInput(::slang::VariableLayoutReflection* param, ShaderReflection &result);
    static void extractBinding(::slang::VariableLayoutReflection *param, std::vector<ShaderReflection::Resource>& resources);
    static void extractMembers(::slang::TypeLayoutReflection *typeLayout, std::vector<ShaderReflection::Member>& members);

    static void extractPushConstants(::slang::ProgramLayout *layout, ShaderReflection &result);

    static void extractParameterBlock(::slang::VariableLayoutReflection *param, ::slang::TypeLayoutReflection *typeLayout, std::vector<ShaderReflection::Resource>& resources);
    static void extractUniformMembers(::slang::TypeLayoutReflection *typeLayout, std::vector<ShaderReflection::Member>& members);
    static void extractParameterBlockResources(::slang::TypeLayoutReflection *typeLayout, uint32_t set, uint32_t baseBinding, const std::string &prefix, std::vector<ShaderReflection::Resource>& resources);
};
}

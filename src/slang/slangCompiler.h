#pragma once
#include <expected>
#include <slang-com-ptr.h>
#include <slang.h>
#include <string>
#include <vector>

#include "descriptions/shaderDesc.h"
#include "descriptions/shaderReflection.h"
#include "enums/shaderStage.h"

namespace urhi
{
struct SlangSource
{
     std::string path;
     std::string source;
     std::vector<std::string> includeDirs;
     std::vector<std::pair<std::string, std::string>> defines;
     bool optimize = false;

     static SlangSource fromFile(const std::string_view path)
     {
          return { .path = std::string(path) };
     }

     static SlangSource fromSource(const std::string_view src, const std::string_view name = "inline")
     {
          return { .path = std::string(name), .source = std::string(src) };
     }

     SlangSource& include(std::vector<std::string> dirs) &  { includeDirs = std::move(dirs); return *this; }
     SlangSource  include(std::vector<std::string> dirs) && { includeDirs = std::move(dirs); return std::move(*this); }

     SlangSource& define(std::string key, std::string val = "1") &  { defines.emplace_back(std::move(key), std::move(val)); return *this; }
     SlangSource  define(std::string key, std::string val = "1") && { defines.emplace_back(std::move(key), std::move(val)); return std::move(*this); }

     SlangSource& optimized(const bool on = true) &  { optimize = on; return *this; }
     SlangSource  optimized(const bool on = true) && { optimize = on; return std::move(*this); }
};

struct SlangModule
{
     std::string name;
     std::string path;
     std::vector<uint8_t> ir;
};

struct SlangLinkDesc
{
     std::vector<SlangModule> modules;
     std::vector<std::string> entryPoints;
     std::vector<std::string> typeSpecializations;
};

struct SlangLinkedModule
{
     std::vector<Slang::ComPtr<slang::IEntryPoint>> entryPoints;
     Slang::ComPtr<slang::IComponentType> linkedShader;
     Slang::ComPtr<slang::ISession> session;
     refl::Data reflection;
};

class SlangCompiler
{
public:
     static std::expected<SlangModule, std::string> compile(const SlangSource& source);
     static std::expected<SlangLinkedModule, std::string> link(const SlangLinkDesc& desc);
     static std::expected<ShaderEntryPoint, std::string> extract(const SlangLinkedModule& linked, ShaderStage stage, const std::string& name = "");
private:
     static slang::IGlobalSession* getGlobalSession();
     static Slang::ComPtr<slang::ISession> getSession();

     static std::expected<Slang::ComPtr<slang::IModule>, std::string> loadModuleFromIR(slang::ISession *session, const SlangModule &module);
     static slang::TypeReflection* findType(const std::vector<Slang::ComPtr<slang::IModule>>& modules, const std::string& typeName);
     static std::expected<Slang::ComPtr<slang::IComponentType>, std::string> trySpecialize(const std::vector<Slang::ComPtr<slang::IModule>>& modules, const std::vector<std::string>& typeSpecializations, Slang::ComPtr<slang::IComponentType> component);
     static ShaderStage convertSlangStage(SlangStage stage);

     static refl::Data extractReflection(slang::ProgramLayout* layout, size_t entryPointIndex);

     static void extractVertexInput(slang::VariableLayoutReflection* param, refl::Data& result, uint32_t binding);
     static void extractBinding(slang::VariableLayoutReflection *param, std::vector<refl::Resource>& resources);
     static void extractMembers(slang::TypeLayoutReflection *typeLayout, std::vector<refl::Member>& members);

     static void extractPushConstants(slang::ProgramLayout *layout, refl::Data &result);

     static void extractParameterBlock(slang::VariableLayoutReflection *param, slang::TypeLayoutReflection *typeLayout, std::vector<refl::Resource>& resources);
     static void extractUniformMembers(slang::TypeLayoutReflection *typeLayout, std::vector<refl::Member>& members);
     static void extractParameterBlockResources(slang::TypeLayoutReflection *typeLayout, uint32_t set, uint32_t baseBinding, const std::string &prefix, std::vector<refl::Resource>& resources);
};
}

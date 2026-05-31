#pragma once
#include "shader.h"
#include "glDevice.h"

#include <glad/gl.h>

#include "spirv_glsl.hpp"

namespace urhi
{
class GlDevice;
class GlShader final : public Shader
{
public:
    struct CombinedSamplerInfo;
    struct UboReflection
    {
        std::string  instanceName;
        std::string  blockName;
        uint32_t     binding;
        bool         isPushConstant;
    };

    GlShader(GlDevice* device, const ShaderEntryPoint& entryPoint);
    ~GlShader() override;

    void reflect(spirv_cross::CompilerGLSL &compiler, ShaderStage stage);

    void reflectCombinedSamplers(const spirv_cross::CompilerGLSL &compiler);
    void reflectUbos(spirv_cross::CompilerGLSL &compiler);

    std::string getOrCompileGlsl(spirv_cross::CompilerGLSL &compiler, const std::vector<uint32_t> &spirv) const;

    std::optional<std::string> tryLoadDisk(uint32_t hash) const;
    void saveDisk(uint32_t hash, std::string glsl) const;

    GLuint handle() const { return m_handle; }

    const std::vector<CombinedSamplerInfo>& combinedSamplers() { return m_combinedSamplers; }
    const std::vector<UboReflection>& uniformBlocks() { return m_ubos; }
    std::optional<UboReflection> pushConstant() { return m_pushConstant; }


    ShaderEntryPoint entryPoint() override { return m_entryPoint; }
private:
    static std::unordered_map<uint32_t, std::string> m_glslCache;

    GlDevice* m_device;
    ShaderEntryPoint m_entryPoint;

    GLuint m_handle;

    std::optional<UboReflection> m_pushConstant = std::nullopt;
    std::vector<CombinedSamplerInfo> m_combinedSamplers;
    std::vector<UboReflection> m_ubos{};
public:
    struct CombinedSamplerInfo
    {
        uint32_t    textureUnit;
        std::string combinedName;
        uint32_t    texNameHash;
        uint32_t    samplerNameHash;
    };
};
}

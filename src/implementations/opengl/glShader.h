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
    struct StorageImageInfo;
    struct BufferReflection
    {
        std::string  instanceName;
        uint32_t     binding;
        ResourceAccess access;
    };

    GlShader(GlDevice* device, const ShaderEntryPoint& entryPoint);
    ~GlShader() override;

    void reflect(spirv_cross::CompilerGLSL &compiler, ShaderStage stage);

    void reflectCombinedSamplers(const spirv_cross::CompilerGLSL &compiler);
    void reflectUbos(spirv_cross::CompilerGLSL &compiler);

    std::string getOrCompileGlsl(spirv_cross::CompilerGLSL &compiler, const std::vector<uint32_t> &spirv) const;

    [[nodiscard]] std::optional<std::string> tryLoadDisk(uint32_t hash) const;
    void saveDisk(uint32_t hash, std::string glsl) const;

    [[nodiscard]] GLuint handle() const { return m_handle; }

    const std::vector<CombinedSamplerInfo>& combinedSamplers() { return m_combinedSamplers; }
    const std::vector<StorageImageInfo>& storageImages() { return m_storageImage; }
    const std::vector<BufferReflection>& uboInfos() { return m_ubos; }
    const std::vector<BufferReflection>& ssboInfos() { return m_ssbos; }
    std::optional<BufferReflection> pushConstant() { return m_pushConstant; }


    ShaderEntryPoint entryPoint() override { return m_entryPoint; }

    static constexpr auto kPushConstantBlockName = "PushConstantBlock";
private:
    static constexpr uint32_t compilerVersion = 6;
    static std::unordered_map<uint32_t, std::string> m_glslCache;

    GlDevice* m_device;
    ShaderEntryPoint m_entryPoint;

    GLuint m_handle;

    std::optional<BufferReflection> m_pushConstant = std::nullopt;
    std::vector<CombinedSamplerInfo> m_combinedSamplers;
    std::vector<StorageImageInfo> m_storageImage;
    std::vector<BufferReflection> m_ubos{};
    std::vector<BufferReflection> m_ssbos{};
public:
    struct CombinedSamplerInfo
    {
        uint32_t    textureUnit;
        std::string combinedName;
        uint32_t    texNameHash;
        uint32_t    samplerNameHash;
    };

    struct StorageImageInfo
    {
        uint32_t unit;
        ResourceAccess access;

        std::string name;
    };
};
}

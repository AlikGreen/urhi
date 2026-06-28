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
    GlShader(GlDevice* device, const ShaderEntryPoint& ep);
    ~GlShader() override;

    [[nodiscard]] GLuint handle() const { return m_handle; }

    refl::Data reflection() override { return m_reflection; }
    ShaderStage stage() override { return m_stage; }

    static constexpr auto kPushConstantBlockName = "PushConstantBlock";
private:
    static constexpr uint32_t compilerVersion = 6;
    static std::unordered_map<uint32_t, std::string> m_glslCache;

    [[nodiscard]] std::string getOrCompileGlsl(const std::vector<uint32_t> &spirv) const;
    [[nodiscard]] std::optional<std::string> tryLoadDisk(uint32_t hash) const;

    void saveDisk(uint32_t hash, std::string glsl) const;

    GlDevice* m_device;
    refl::Data m_reflection;
    ShaderStage m_stage;

    GLuint m_handle;

};
}

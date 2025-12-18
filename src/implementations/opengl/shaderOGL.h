#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <glad/glad.h>

#include "shader.h"
#include "enums/shaderType.h"

namespace Neon::RHI
{
class ShaderOGL final : public Shader
{
public:
    explicit ShaderOGL(const std::unordered_map<ShaderType, std::vector<uint32_t>> &shadersSpirv);

    void compile() override;
    void dispose() override;

    void bind() const;

    ShaderReflection getShaderReflection() override; // TODO: implement

    uint32_t getUBOLocation(    const std::string &name) const;
    uint32_t getSSBOLocation(   const std::string &name) const;
    uint32_t getSamplerLocation(const std::string &name) const;
    uint32_t getImageLocation(  const std::string &name) const;
private:
    struct ShaderStage
    {
        std::string filePath;
        const char* source;
        uint32_t    shaderType;
        uint32_t    shaderHandle;
    };

    struct ShaderBindingReflection
    {
        std::unordered_map<std::string, GLuint> uboBinding{};
        std::unordered_map<std::string, GLuint> ssboBinding{};
        std::unordered_map<std::string, GLuint> samplerUnit{};
        std::unordered_map<std::string, GLuint> imageUnit{};
    };

    static std::string spirvToGlsl(const std::vector<uint32_t> &spirv, ShaderBindingReflection& reflection);

    ShaderBindingReflection reflection{};
    std::unordered_map<ShaderType, std::vector<uint32_t>> shadersSpirv;
    std::vector<GLuint> shaderHandles;
    GLuint handle{};
};
}

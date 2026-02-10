#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <glad/gl.h>

#include "shader.h"
#include "shaderCompiler.h"
#include "spirv_common.hpp"
#include "spirv_cross.hpp"
#include "glm/vec3.hpp"

namespace urhi
{
class ShaderOGL final : public Shader
{
public:
    explicit ShaderOGL(CompiledShader shader);
    ~ShaderOGL() override;

    void compile() override;
    void dispose() override;

    void bind() const;

    ShaderReflection getShaderReflection() override;

    uint32_t getUBOLocation(    const std::string &name) const;
    uint32_t getSSBOLocation(   const std::string &name) const;
    uint32_t getTextureLocation(const std::string &name) const;
    uint32_t getSamplerLocation(const std::string &name) const;
    uint32_t getImageLocation(  const std::string &name) const;

    glm::uvec3 getThreadGroupSize() const;
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
        std::unordered_map<std::string, GLuint> textureUnit{};
        std::unordered_map<std::string, GLuint> imageUnit{};
    };

    static ShaderReflection::DataType spirvTypeToDataType(const spirv_cross::SPIRType& type);
    static GLenum executionModelToStage(spv::ExecutionModel model);

    ShaderBindingReflection m_internalReflection{};
    ShaderReflection m_reflection{};
    CompiledShader m_compiledShader;
    std::vector<GLuint> m_shaderHandles;
    GLuint m_handle{};
    bool m_compiled = false;
};
}

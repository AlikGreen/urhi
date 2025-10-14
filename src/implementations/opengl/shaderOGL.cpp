#include "shaderOGL.h"

#include <ranges>
#include <neonLog/neonLog.h>

#include "convertOGL.h"
#include "spirv_glsl.hpp"


namespace Neon::RHI
{
    ShaderOGL::ShaderOGL(const std::unordered_map<ShaderType, std::vector<uint32_t>> &shadersSpirv) : shadersSpirv(shadersSpirv) {  }

    std::string ShaderOGL::spirvToGlsl(const std::vector<uint32_t> &spirv, ShaderBindingReflection& reflection)
    {
        spirv_cross::CompilerGLSL compiler(spirv);
        compiler.build_combined_image_samplers();

        auto resources = compiler.get_shader_resources();

        for (const auto& ub : resources.uniform_buffers)
        {
            std::string name = compiler.get_name(ub.base_type_id);
            if (name.empty()) name = ub.name;
            const uint32_t binding = compiler.get_decoration(ub.id, spv::DecorationBinding);
            reflection.uboBinding[name] = binding;
        }

        for (const auto& sb : resources.storage_buffers)
        {
            std::string name = compiler.get_name(sb.id);
            if (name.empty()) name = sb.name;
            const uint32_t binding = compiler.get_decoration(sb.id, spv::DecorationBinding);
            reflection.ssboBinding[name] = binding;
        }

        for (const auto& s : resources.sampled_images)
        {
            std::string name = compiler.get_name(s.id);
            if (name.empty()) name = s.name;
            const uint32_t unit = compiler.get_decoration(s.id, spv::DecorationBinding);
            reflection.samplerUnit[name] = unit;
        }

        for (const auto& img : resources.storage_images)
        {
            std::string name = compiler.get_name(img.id);
            if (name.empty()) name = img.name;
            const uint32_t unit = compiler.get_decoration(img.id, spv::DecorationBinding);
            reflection.imageUnit[name] = unit;
        }

        spirv_cross::CompilerGLSL::Options options;
        options.version = 460;
        options.es = false;
        options.vulkan_semantics = false;
        options.separate_shader_objects = true;

        compiler.set_common_options(options);

        return compiler.compile();
    }

    void ShaderOGL::compile()
    {
        for (const auto& [type, shader]: shadersSpirv)
        {
            GLuint shaderHandle = glCreateShader(ConvertOGL::shaderTypeToGL(type));
            shaderHandles.push_back(shaderHandle);
            std::string shaderSource = spirvToGlsl(shader, reflection);
            const char* shaderSourceCStr = shaderSource.c_str();
            glShaderSource(shaderHandle, 1, &shaderSourceCStr, nullptr);
            glCompileShader(shaderHandle);

            int  success;
            char infoLog[512];
            glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
            glGetShaderInfoLog(shaderHandle, 512, nullptr, infoLog);

            Debug::ensure(success, "Shader compilation error: {}", infoLog);
        }

        handle = glCreateProgram();
        for (const GLuint shader: shaderHandles)
        {
            glAttachShader(handle, shader);
        }

        glLinkProgram(handle);

        for (const GLuint shader: shaderHandles)
        {
            glDeleteShader(shader);
        }
    }

    void ShaderOGL::dispose()
    {

    }

    void ShaderOGL::bind() const
    {
        glUseProgram(handle);
    }

    ShaderReflection ShaderOGL::getShaderReflection()
    {
        Debug::ensure(false, "Not implemented");
        return {};
    }

    uint32_t ShaderOGL::getUBOLocation(const std::string &name) const
    {
        Debug::ensure(reflection.uboBinding.contains(name), "Shader does not contain a uniform buffer with the name {}", name);
        return reflection.uboBinding.at(name);
    }

    uint32_t ShaderOGL::getSSBOLocation(const std::string &name) const
    {
        Debug::ensure(reflection.ssboBinding.contains(name), "Shader does not contain a storage buffer with the name {}", name);
        return reflection.ssboBinding.at(name);
    }

    uint32_t ShaderOGL::getSamplerLocation(const std::string &name) const
    {
        Debug::ensure(reflection.samplerUnit.contains(name), "Shader does not contain a sampler with the name {}", name);
        return reflection.samplerUnit.at(name);
    }

    uint32_t ShaderOGL::getImageLocation(const std::string &name) const
    {
        Debug::ensure(reflection.imageUnit.contains(name), "Shader does not contain an image with the name {}", name);
        return reflection.imageUnit.at(name);
    }
}

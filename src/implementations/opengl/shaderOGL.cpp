#include "shaderOGL.h"

#include <ranges>
#include <neonLog/neonLog.h>

#include "convertOGL.h"
#include "spirv_glsl.hpp"


namespace Neon::RHI
{
    ShaderOGL::ShaderOGL(const std::unordered_map<ShaderType, std::vector<uint32_t>> &shadersSpirv) : shadersSpirv(shadersSpirv) {  }

    ShaderOGL::~ShaderOGL()
    {
        glDeleteProgram(handle);
    }


    std::string ShaderOGL::spirvToGlsl(const std::vector<uint32_t> &spirv)
    {
        spirv_cross::CompilerGLSL compiler(spirv);
        compiler.build_combined_image_samplers();

        auto resources = compiler.get_shader_resources();

        for (const auto& ub : resources.uniform_buffers)
        {
            std::string name = compiler.get_name(ub.base_type_id);
            if (name.empty()) name = ub.name;
            const uint32_t binding = compiler.get_decoration(ub.id, spv::DecorationBinding);
            internalReflection.uboBinding[name] = binding;

            const spirv_cross::SPIRType &structType = compiler.get_type(ub.base_type_id);
            const size_t blockSize = compiler.get_declared_struct_size(structType);

            ShaderUniformBlock block;
            block.name = name;
            block.size = blockSize;

            for (uint32_t m = 0; m < structType.member_types.size(); ++m)
            {
                ShaderUniformBlockMember member;
                member.name          = compiler.get_member_name(ub.base_type_id, m);
                member.offset        = compiler.get_member_decoration(ub.base_type_id, m, spv::DecorationOffset);
                member.size          = static_cast<uint32_t>(compiler.get_declared_struct_member_size(structType, m));
                const spirv_cross::SPIRType& type = compiler.get_type(structType.member_types[m]);
                member.baseType      = toType(type);
                block.members.push_back(member);
            }

            reflection.uniformBlocks.push_back(block);
        }

        for (const auto& sb : resources.storage_buffers)
        {
            std::string name = compiler.get_name(sb.id);
            if (name.empty()) name = sb.name;
            const uint32_t binding = compiler.get_decoration(sb.id, spv::DecorationBinding);
            internalReflection.ssboBinding[name] = binding;
        }

        for (const auto& s : resources.sampled_images)
        {
            std::string name = compiler.get_name(s.id);
            if (name.empty()) name = s.name;
            const uint32_t unit = compiler.get_decoration(s.id, spv::DecorationBinding);
            internalReflection.samplerUnit[name] = unit;

            ShaderSampler sampler;
            sampler.name = name;
            reflection.samplers.push_back(sampler);
        }

        for (const auto& img : resources.storage_images)
        {
            std::string name = compiler.get_name(img.id);
            if (name.empty()) name = img.name;
            const uint32_t unit = compiler.get_decoration(img.id, spv::DecorationBinding);
            internalReflection.imageUnit[name] = unit;
        }

        spirv_cross::CompilerGLSL::Options options;
        options.version = 460;
        options.es = false;
        options.vulkan_semantics = false;
        options.separate_shader_objects = true;

        compiler.set_common_options(options);

        return compiler.compile();
    }

    ShaderBaseType ShaderOGL::toType(const spirv_cross::SPIRType& typeId)
    {
        switch(typeId.basetype)
        {
            case spirv_cross::SPIRType::Float:
                return ShaderBaseType::Float;
            case spirv_cross::SPIRType::Int:
                return ShaderBaseType::Int;
            case spirv_cross::SPIRType::UInt:
                return ShaderBaseType::UInt;
            case spirv_cross::SPIRType::Boolean:
                return ShaderBaseType::Bool;
            default:
                return ShaderBaseType::Unknown;
        }
    }

    void ShaderOGL::compile()
    {
        Debug::ensure(!compiled, "Shader already compiled");
        compiled = true;

        for (const auto& [type, shader]: shadersSpirv)
        {
            GLuint shaderHandle = glCreateShader(ConvertOGL::shaderTypeToGL(type));
            shaderHandles.push_back(shaderHandle);
            std::string shaderSource = spirvToGlsl(shader);
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
        return reflection;
    }

    uint32_t ShaderOGL::getUBOLocation(const std::string &name) const
    {
        Debug::ensure(internalReflection.uboBinding.contains(name), "Shader does not contain a uniform buffer with the name {}", name);
        return internalReflection.uboBinding.at(name);
    }

    uint32_t ShaderOGL::getSSBOLocation(const std::string &name) const
    {
        Debug::ensure(internalReflection.ssboBinding.contains(name), "Shader does not contain a storage buffer with the name {}", name);
        return internalReflection.ssboBinding.at(name);
    }

    uint32_t ShaderOGL::getSamplerLocation(const std::string &name) const
    {
        Debug::ensure(internalReflection.samplerUnit.contains(name), "Shader does not contain a sampler with the name {}", name);
        return internalReflection.samplerUnit.at(name);
    }

    uint32_t ShaderOGL::getImageLocation(const std::string &name) const
    {
        Debug::ensure(internalReflection.imageUnit.contains(name), "Shader does not contain an image with the name {}", name);
        return internalReflection.imageUnit.at(name);
    }
}

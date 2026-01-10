#include "shaderOGL.h"

#include <ranges>
#include <utility>
#include <neonLog/neonLog.h>

#include "convertOGL.h"
#include "spirv_glsl.hpp"


namespace Neon::RHI
{
    ShaderOGL::ShaderOGL(const std::vector<uint32_t> &spirv) : m_spirv(spirv)
    {  }

    ShaderOGL::~ShaderOGL()
    {
        glDeleteProgram(m_handle);
    }


    ShaderReflection ShaderOGL::reflect(const spirv_cross::Compiler& compiler)
    {
        ShaderReflection reflection{};
        auto resources = compiler.get_shader_resources();

        for (const auto& ub : resources.uniform_buffers)
        {
            ShaderReflection::Resource resource{};
            resource.type = ShaderReflection::ResourceType::ConstantBuffer;

            resource.name = compiler.get_name(ub.base_type_id);
            if (resource.name.empty()) resource.name = ub.name;

            const uint32_t binding = compiler.get_decoration(ub.id, spv::DecorationBinding);
            m_internalReflection.uboBinding[resource.name] = binding;

            const spirv_cross::SPIRType &structType = compiler.get_type(ub.base_type_id);
            const size_t blockSize = compiler.get_declared_struct_size(structType);

            resource.totalSize = blockSize;

            for (uint32_t m = 0; m < structType.member_types.size(); ++m)
            {
                ShaderReflection::Member member;
                member.name          = compiler.get_member_name(ub.base_type_id, m);
                member.offset        = compiler.get_member_decoration(ub.base_type_id, m, spv::DecorationOffset);
                member.size          = static_cast<uint32_t>(compiler.get_declared_struct_member_size(structType, m));
                const spirv_cross::SPIRType& type = compiler.get_type(structType.member_types[m]);
                member.type      = spirvTypeToDataType(type);
                resource.members.push_back(member);
            }

            reflection.resources.push_back(resource);
        }

        for (const auto& sb : resources.storage_buffers)
        {
            std::string name = compiler.get_name(sb.id);
            if (name.empty()) name = sb.name;
            const uint32_t binding = compiler.get_decoration(sb.id, spv::DecorationBinding);
            m_internalReflection.ssboBinding[name] = binding;
        }

        for (const auto& s : resources.sampled_images)
        {
            std::string name = compiler.get_name(s.id);
            if (name.empty()) name = s.name;
            const uint32_t unit = compiler.get_decoration(s.id, spv::DecorationBinding);
            m_internalReflection.samplerUnit[name] = unit;

            ShaderReflection::Resource resource;
            resource.type = ShaderReflection::ResourceType::Sampler;
            resource.name = name;
            reflection.resources.push_back(resource);
        }

        for (const auto& img : resources.storage_images)
        {
            std::string name = compiler.get_name(img.id);
            if (name.empty()) name = img.name;
            const uint32_t unit = compiler.get_decoration(img.id, spv::DecorationBinding);
            m_internalReflection.imageUnit[name] = unit;
        }

        return reflection;
    }

    GLenum ShaderOGL::executionModelToStage(const spv::ExecutionModel model)
    {
        switch (model)
        {
            case spv::ExecutionModelVertex:
                return GL_VERTEX_SHADER;
            case spv::ExecutionModelFragment:
                return GL_FRAGMENT_SHADER;
            case spv::ExecutionModelGLCompute:
                return GL_COMPUTE_SHADER;
            default:
                return GL_NONE;
        }
    }

    ShaderReflection::DataType ShaderOGL::spirvTypeToDataType(const spirv_cross::SPIRType& type)
    {
        if (type.basetype == spirv_cross::SPIRType::Struct)
            return ShaderReflection::DataType::Struct;

        const bool isFloat = type.basetype == spirv_cross::SPIRType::Float;
        const bool isInt = type.basetype == spirv_cross::SPIRType::Int;
        const bool isUInt = type.basetype == spirv_cross::SPIRType::UInt;

        if (type.columns > 1)
        {
            if (type.columns == 3 && type.vecsize == 3)
                return ShaderReflection::DataType::Mat3;
            if (type.columns == 4 && type.vecsize == 4)
                return ShaderReflection::DataType::Mat4;
        }

        if (isFloat)
        {
            switch (type.vecsize)
            {
                default: return ShaderReflection::DataType::Float;
                case 2: return ShaderReflection::DataType::Float2;
                case 3: return ShaderReflection::DataType::Float3;
                case 4: return ShaderReflection::DataType::Float4;
            }
        }
        if (isInt)
        {
            switch (type.vecsize)
            {
                default: return ShaderReflection::DataType::Int;
                case 2: return ShaderReflection::DataType::Int2;
                case 3: return ShaderReflection::DataType::Int3;
                case 4: return ShaderReflection::DataType::Int4;
            }
        }
        if (isUInt)
        {
            switch (type.vecsize)
            {
                default: return ShaderReflection::DataType::UInt;
                case 2: return ShaderReflection::DataType::UInt2;
                case 3: return ShaderReflection::DataType::UInt3;
                case 4: return ShaderReflection::DataType::UInt4;
            }
        }

        return ShaderReflection::DataType::Float;
    }

    void ShaderOGL::compile()
    {
        Debug::ensure(!m_compiled, "Shader already compiled");
        m_compiled = true;
        spirv_cross::CompilerGLSL compiler(m_spirv);

        auto entryPoints = compiler.get_entry_points_and_stages();

        for (auto& entry : entryPoints)
        {
            compiler.set_entry_point(entry.name, entry.execution_model);

            spirv_cross::CompilerGLSL::Options options;
            options.version = 450;
            options.es = false;
            options.separate_shader_objects = true;
            options.enable_420pack_extension = true;
            compiler.set_common_options(options);

            m_reflection = reflect(compiler);

            std::string glslSource = compiler.compile();

            GLuint shaderHandle = glCreateShader(executionModelToStage(entry.execution_model));
            m_shaderHandles.push_back(shaderHandle);
            const char* shaderSourceCStr = glslSource.c_str();
            glShaderSource(shaderHandle, 1, &shaderSourceCStr, nullptr);
            glCompileShader(shaderHandle);

            int  success;
            char infoLog[512];
            glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &success);
            glGetShaderInfoLog(shaderHandle, 512, nullptr, infoLog);

            Debug::ensure(success, "Shader compilation error: {}", infoLog);
        }

        m_handle = glCreateProgram();
        for (const GLuint shader: m_shaderHandles)
        {
            glAttachShader(m_handle, shader);
        }

        glLinkProgram(m_handle);

        for (const GLuint shader: m_shaderHandles)
        {
            glDeleteShader(shader);
        }
    }

    void ShaderOGL::dispose()
    {

    }

    void ShaderOGL::bind() const
    {
        glUseProgram(m_handle);
    }

    ShaderReflection ShaderOGL::getShaderReflection()
    {
        return m_reflection;
    }

    uint32_t ShaderOGL::getUBOLocation(const std::string &name) const
    {
        Debug::ensure(m_internalReflection.uboBinding.contains(name), "Shader does not contain a uniform buffer with the name {}", name);
        return m_internalReflection.uboBinding.at(name);
    }

    uint32_t ShaderOGL::getSSBOLocation(const std::string &name) const
    {
        Debug::ensure(m_internalReflection.ssboBinding.contains(name), "Shader does not contain a storage buffer with the name {}", name);
        return m_internalReflection.ssboBinding.at(name);
    }

    uint32_t ShaderOGL::getSamplerLocation(const std::string &name) const
    {
        Debug::ensure(m_internalReflection.samplerUnit.contains(name), "Shader does not contain a sampler with the name {}", name);
        return m_internalReflection.samplerUnit.at(name);
    }

    uint32_t ShaderOGL::getImageLocation(const std::string &name) const
    {
        Debug::ensure(m_internalReflection.imageUnit.contains(name), "Shader does not contain an image with the name {}", name);
        return m_internalReflection.imageUnit.at(name);
    }
}

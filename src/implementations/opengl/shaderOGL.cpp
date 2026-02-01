#include "shaderOGL.h"

#include <ranges>
#include <neonLog/neonLog.h>

#include "convertOGL.h"
#include "spirv_glsl.hpp"


namespace Neon::RHI
{
    ShaderOGL::ShaderOGL(const CompiledShader &shader) : m_compiledShader(shader)
    {  }

    ShaderOGL::~ShaderOGL()
    {
        glDeleteProgram(m_handle);

        GLuint block_index = glGetUniformBlockIndex(m_handle, "EntryPointParams_std430");

        if (block_index != GL_INVALID_INDEX)
        {
            // Get the size of the uniform block
            GLint block_size;
            glGetActiveUniformBlockiv(m_handle, block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &block_size);

            // Create UBO
            GLuint ubo;
            glGenBuffers(1, &ubo);
            glBindBuffer(GL_UNIFORM_BUFFER, ubo);
            glBufferData(GL_UNIFORM_BUFFER, block_size, nullptr, GL_DYNAMIC_DRAW);

            // Bind to a binding point (e.g., 1, since 0 is used by CameraUniforms)
            GLuint binding_point = 1;
            glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, ubo);
            glUniformBlockBinding(m_handle, block_index, binding_point);
        }
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
        spirv_cross::CompilerGLSL compiler(m_compiledShader.spirv);

        auto entryPoints = compiler.get_entry_points_and_stages();

        m_reflection = m_compiledShader.reflection;

        for (auto& entry : entryPoints)
        {
            compiler.set_entry_point(entry.name, entry.execution_model);

            spirv_cross::CompilerGLSL::Options options;
            options.version = 450;
            options.es = false;
            options.vulkan_semantics = false;
            options.separate_shader_objects = true;
            options.enable_420pack_extension = true;
            options.emit_push_constant_as_uniform_buffer = true;
            compiler.set_common_options(options);

            compiler.build_combined_image_samplers();

            auto combined = compiler.get_combined_image_samplers();
            for (auto& c : combined)
            {
                const uint32_t binding = compiler.get_decoration(c.image_id, spv::DecorationBinding);
                compiler.set_decoration(c.combined_id, spv::DecorationBinding, binding);
            }

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

        for(const auto& resource : m_reflection.resources)
        {
            if(resource.type == ShaderReflection::ResourceType::Sampler)
                m_internalReflection.samplerUnit[resource.name] = resource.binding;
            if(resource.type == ShaderReflection::ResourceType::Texture)
                m_internalReflection.textureUnit[resource.name] = resource.binding;
            if(resource.type == ShaderReflection::ResourceType::ConstantBuffer)
                m_internalReflection.uboBinding[resource.name] = resource.binding;
            if(resource.type == ShaderReflection::ResourceType::StorageImage)
                m_internalReflection.imageUnit[resource.name] = resource.binding;
            if(resource.type == ShaderReflection::ResourceType::StorageBuffer)
                m_internalReflection.ssboBinding[resource.name] = resource.binding;
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

    uint32_t ShaderOGL::getTextureLocation(const std::string &name) const
    {
        Debug::ensure(m_internalReflection.textureUnit.contains(name), "Shader does not contain a texture with the name {}", name);
        return m_internalReflection.textureUnit.at(name);
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

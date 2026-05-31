#include "glPipeline.h"


#include "clogr.h"
#include "glConvert.h"
#include "glDevice.h"
#include "validation.h"

namespace urhi
{
    GlPipeline::GlPipeline(GlDevice* device, const GraphicsPipelineDesc &desc)
        : m_device(device), m_primitiveType(desc.primitiveType)
    {
        createProgram(desc.shaders);

        glUseProgram(m_shaderProgram);

        for(const auto shader : desc.shaders)
        {
            auto glShader = static_cast<GlShader*>(shader.get());
            extractReflection(glShader);
        }

        glUseProgram(0);

        glCreateVertexArrays(1, &m_vao);

        GlShader* vertexShader = nullptr;
        for(const auto& shader : desc.shaders)
        {
            if(shader->entryPoint().stage == ShaderStage::Vertex)
            {
                vertexShader = static_cast<GlShader*>(shader.get());
                break;
            }
        }

        URHI_VALIDATE(vertexShader != nullptr, "Missing vertex shader - Graphics pipeline must contain a vertex shader.");

        const auto input = vertexShader->entryPoint().reflection.vertexInput;

        for (const auto& attr : input.attributes)
        {
            URHI_VALIDATE(attr.type != ShaderReflection::DataType::Mat3 &&
                          attr.type != ShaderReflection::DataType::Mat4 &&
                          attr.type != ShaderReflection::DataType::Struct &&
                          attr.type != ShaderReflection::DataType::Unknown,
                          "Mat3, Mat4, Struct and Unknown are not valid vertex attribute types");

            glEnableVertexArrayAttrib(m_vao, attr.location);
            glVertexArrayAttribBinding(m_vao, attr.location, attr.binding);

            if (GlConvert::isIntegerType(attr.type))
            {
                glVertexArrayAttribIFormat(m_vao, attr.location,
                    GlConvert::componentCount(attr.type),
                    GlConvert::vertexBaseType(attr.type),
                    attr.offset);
            }
            else
            {
                glVertexArrayAttribFormat(m_vao, attr.location,
                    GlConvert::componentCount(attr.type),
                    GL_FLOAT,
                    GL_FALSE,
                    attr.offset);
            }
        }

        m_vertexStrides.resize(input.bindings.size());
        for (auto& binding : input.bindings)
            m_vertexStrides[binding.binding] = binding.stride;
    }

    GlPipeline::GlPipeline(GlDevice *device, const ComputePipelineDesc &desc)
        : m_device(device)
    {
        createProgram({desc.shader});

        glUseProgram(m_shaderProgram);
        extractReflection(static_cast<GlShader*>(desc.shader.get()));
        glUseProgram(0);
    }

    void GlPipeline::createProgram(const std::vector<grl::Rc<Shader>> &shaders)
    {
        m_shaderProgram = glCreateProgram();

        for(const auto shader : shaders)
        {
            const auto glShader = static_cast<GlShader*>(shader.get());
            glAttachShader(m_shaderProgram, glShader->handle());
        }

        glLinkProgram(m_shaderProgram);
    }

    void GlPipeline::extractReflection(GlShader* shader)
    {
        for (auto& info : shader->combinedSamplers())
        {
            const GLint loc = glGetUniformLocation(m_shaderProgram, info.combinedName.c_str());

            // Set the sampler uniform to point at the right texture unit now
            // so won't have to set again
            if (loc >= 0)
                glUniform1i(loc, info.textureUnit);

            CombinedSamplerUnit entry { info.textureUnit, loc };

            m_bindings[info.texNameHash].push_back(entry);
            m_bindings[info.samplerNameHash].push_back(entry);
        }

        for (auto& info : shader->uniformBlocks())
        {
            const GLuint blockIndex = glGetUniformBlockIndex(m_shaderProgram, info.blockName.c_str());
            if (blockIndex == GL_INVALID_INDEX) continue;

            uint32_t hash = grl::Hash::fnv1a32(info.instanceName);
            glUniformBlockBinding(m_shaderProgram, blockIndex, info.binding); // set binding point
            m_bufferBindings[hash] = info.binding;
        }

        if (shader->pushConstant().has_value())
        {
            GLuint blockIndex = glGetUniformBlockIndex(m_shaderProgram,
                shader->pushConstant()->blockName.c_str());

            URHI_VALIDATE(blockIndex != GL_INVALID_INDEX,
                "Push constant block '{}' not found in shader — check the name matches what SPIR-V Cross emitted",
                shader->pushConstant()->blockName);

            // Fixed binding point for push constants — always 0
            constexpr GLuint PUSH_CONSTANT_BINDING = 0;

            // blockIndex is just a stepping stone, only needed for this call
            glUniformBlockBinding(m_shaderProgram, blockIndex, PUSH_CONSTANT_BINDING);

            URHI_VALIDATE(m_pushConstantBinding < 0 || m_pushConstantBinding == PUSH_CONSTANT_BINDING,
                "Invalid push constants - you have multiple different push constants in different shader stages.");

            m_pushConstantBinding = PUSH_CONSTANT_BINDING;
        }
    }

    uint32_t GlPipeline::vertexStride(const uint32_t bindingIndex) const
    {
        URHI_VALIDATE(bindingIndex < m_vertexStrides.size(), "Invalid binding index");
        return m_vertexStrides[bindingIndex];
    }

    GlPipeline::~GlPipeline()
    {
        glDeleteProgram(m_shaderProgram);
    }

    void GlPipeline::bind() const
    {
        if(m_vao != 0)
            glBindVertexArray(m_vao);

        glUseProgram(m_shaderProgram);

        // TODO set all defaults eg scissor and viewport
    }
}

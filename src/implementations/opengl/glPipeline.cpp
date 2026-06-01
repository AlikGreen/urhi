#include "glPipeline.h"


#include "clogr.h"
#include "glConvert.h"
#include "glDevice.h"
#include "validation.h"

namespace urhi
{
    GlPipeline::GlPipeline(GlDevice* device, const GraphicsPipelineDesc &desc)
        : m_device(device), m_primitiveType(desc.primitiveType), m_colorAttachments(desc.colorAttachments)
    {
        m_depthFunc = GlConvert::compareOp(desc.depthState.compareOp);
        m_cullFaceMode = GlConvert::cullMode(desc.rasterizerState.cullMode);
        m_polygonMode = GlConvert::fillMode(desc.rasterizerState.fillMode);

        m_enableDepthTest = desc.depthState.enableDepthTest;
        m_enableDepthWrite = desc.depthState.enableDepthWrite;

        m_enableScissorTest = desc.rasterizerState.enableScissorTest;
        // m_enableStencilTest = desc.rasterizerState.enableStencilTest;

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
        glUseProgram(m_shaderProgram);

        if(m_vao == 0) return;

        glBindVertexArray(m_vao);

        for(size_t i = 0; i < m_colorAttachments.size(); i++)
        {
            const auto& attachment = m_colorAttachments[i];

            if(!attachment.blend.enableBlend)
            {
                glDisablei(GL_BLEND, i);
                continue;
            }

            glEnablei(GL_BLEND, i);

            glBlendEquationSeparatei(
                i,
                GlConvert::blendOp(attachment.blend.colorOp),
                GlConvert::blendOp(attachment.blend.alphaOp)
            );

            glBlendFuncSeparatei(
                i,
                GlConvert::blendFactor(attachment.blend.srcColorFactor),
                GlConvert::blendFactor(attachment.blend.dstColorFactor),
                GlConvert::blendFactor(attachment.blend.srcAlphaFactor),
                GlConvert::blendFactor(attachment.blend.dstAlphaFactor)
            );

            glColorMaski(
                i,
                hasFlag(attachment.blend.writeMask, ColorWriteMask::R),
                hasFlag(attachment.blend.writeMask, ColorWriteMask::G),
                hasFlag(attachment.blend.writeMask, ColorWriteMask::B),
                hasFlag(attachment.blend.writeMask, ColorWriteMask::A)
            );

        }

        if(m_enableDepthTest)
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(m_depthFunc);
            glDepthMask(m_enableDepthWrite);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
        }

        if(m_cullFaceMode != GL_NONE)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(m_cullFaceMode);
            glFrontFace(GL_CCW);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }

        if(m_enableScissorTest)
            glEnable(GL_SCISSOR_TEST);
        else
            glDisable(GL_SCISSOR_TEST);


        glPolygonMode(GL_FRONT_AND_BACK, m_polygonMode);

        if(m_enableStencilTest)
        {
            glEnable(GL_STENCIL_TEST);
            // glStencilFuncSeparate(GL_FRONT, m_stencilFuncFront, m_stencilRefFront, m_stencilMaskFront);
            // glStencilFuncSeparate(GL_BACK,  m_stencilFuncBack,  m_stencilRefBack,  m_stencilMaskBack);
            // glStencilOpSeparate(GL_FRONT, m_stencilOpFailFront, m_stencilOpZFailFront, m_stencilOpZPassFront);
            // glStencilOpSeparate(GL_BACK,  m_stencilOpFailBack,  m_stencilOpZFailBack,  m_stencilOpZPassBack);
        }
        else
        {
            glDisable(GL_STENCIL_TEST);
        }
    }
}

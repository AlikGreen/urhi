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

        for(const auto& shader : shaders)
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

            // Set uniform now so don't have to do per frame
            if (loc >= 0)
                glUniform1i(loc, info.textureUnit);

            CombinedSamplerUnit entry { info.textureUnit, loc };

            m_textureBindings[info.texNameHash].push_back(entry);
            m_samplerBindings[info.samplerNameHash].push_back(entry);
        }

        for (auto& info : shader->storageImages())
        {
            const GLint loc = glGetUniformLocation(m_shaderProgram, info.name.c_str());

            if (loc >= 0)
                glUniform1i(loc, info.unit);

            uint32_t hash = NameRegistry::getHash(info.name);
            m_imageBindings[hash] = ComputeResourceInfo{ static_cast<int>(info.unit), info.access, 0 };
        }

        resolveBlockBindings();

        int maxBufferBinding = -1;

        for (auto& info : shader->uboInfos())
        {
            uint32_t hash = NameRegistry::getHash(info.instanceName);
            m_bufferBindings[hash] = { static_cast<int>(info.binding), info.access, GL_UNIFORM_BUFFER };
            maxBufferBinding = std::max(maxBufferBinding, static_cast<int>(info.binding));
        }

        for (auto& info : shader->ssboInfos())
        {
            uint32_t hash = NameRegistry::getHash(info.instanceName);
            m_bufferBindings[hash] = { static_cast<int>(info.binding), info.access, GL_SHADER_STORAGE_BUFFER };
            maxBufferBinding = std::max(maxBufferBinding, static_cast<int>(info.binding));
        }

        if (shader->pushConstant().has_value())
        {
            const GLuint blockIndex = glGetUniformBlockIndex(m_shaderProgram, GlShader::kPushConstantBlockName);
            URHI_VALIDATE(blockIndex != GL_INVALID_INDEX,
                "Push constant block not found in shader");

            if (m_pushConstantBinding >= 0)
            {
                URHI_VALIDATE(shader->pushConstant()->instanceName == m_pushConstantInstanceName,
                    "Mismatched push constant blocks across shader stages: '{}' vs '{}'",
                    shader->pushConstant()->instanceName, m_pushConstantInstanceName);

                glUniformBlockBinding(m_shaderProgram, blockIndex, m_pushConstantBinding);
            }
            else
            {
                const int slot = maxBufferBinding + 1;
                glUniformBlockBinding(m_shaderProgram, blockIndex, slot);
                m_pushConstantBinding = slot;
                m_pushConstantInstanceName = shader->pushConstant()->instanceName;
            }
        }

        for (const auto& res : shader->entryPoint().reflection.resources)
        {
            uint32_t nameHash = NameRegistry::getHash(res.name);
            if (res.isBuffer())
            {
                if (!m_bufferBindings.contains(nameHash))
                    m_bufferBindings[nameHash] = { OPTIMIZED_OUT, ResourceAccess::ReadOnly };
            }
            else if (res.isTexture() || res.type == ShaderReflection::ResourceType::Sampler)
            {
                if (!m_textureBindings.contains(nameHash)) m_textureBindings[nameHash] = {};
                if (!m_samplerBindings.contains(nameHash)) m_samplerBindings[nameHash] = {};
            }
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

    const std::vector<GlPipeline::CombinedSamplerUnit>* GlPipeline::textureBinding(uint32_t nameHash) const
    {
        if (const auto it = m_textureBindings.find(nameHash); it != m_textureBindings.end())
            return &it->second;
        return nullptr;
    }

    const std::vector<GlPipeline::CombinedSamplerUnit>* GlPipeline::samplerBinding(uint32_t nameHash) const
    {
        if (const auto it = m_samplerBindings.find(nameHash); it != m_samplerBindings.end())
            return &it->second;
        return nullptr;
    }

    GlPipeline::ComputeResourceInfo GlPipeline::bufferBinding(const uint32_t nameHash)
    {
        const auto it = m_bufferBindings.find(nameHash);
        if(it != m_bufferBindings.end())
            return it->second;

        return { INVALID_TYPO, ResourceAccess::ReadOnly };
    }

    GlPipeline::ComputeResourceInfo GlPipeline::imageBinding(const uint32_t nameHash)
    {
        const auto it = m_imageBindings.find(nameHash);
        if(it != m_imageBindings.end())
            return it->second;

        return { INVALID_TYPO, ResourceAccess::ReadOnly };
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

        // if(m_cullFaceMode != GL_NONE)
        // {
        //     glEnable(GL_CULL_FACE);
        //     glCullFace(m_cullFaceMode);
        //     glFrontFace(GL_CCW);
        // }
        // else
        // {
        //     glDisable(GL_CULL_FACE);
        // }

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

    void GlPipeline::resolveBlockBindings()
    {
        GLint count = 0;

        glGetProgramInterfaceiv(m_shaderProgram, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &count);

        for (GLint i = 0; i < count; i++)
        {
            GLenum prop = GL_BUFFER_BINDING;
            GLint binding = 0;
            glGetProgramResourceiv(m_shaderProgram, GL_SHADER_STORAGE_BLOCK,
                                   i, 1, &prop, 1, nullptr, &binding);

            glShaderStorageBlockBinding(m_shaderProgram, i, binding);
        }

        glGetProgramInterfaceiv(m_shaderProgram, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &count);

        for (GLint i = 0; i < count; i++)
        {
            GLenum prop = GL_BUFFER_BINDING;
            GLint binding = 0;
            glGetProgramResourceiv(m_shaderProgram, GL_UNIFORM_BLOCK,
                                   i, 1, &prop, 1, nullptr, &binding);
            glUniformBlockBinding(m_shaderProgram, i, binding);
        }
    }
}

#include "commandListOGL.h"

#include "pipelineOGL.h"

#include <cstring>

#include "bufferOGL.h"
#include "convertOGL.h"
#include "debug.h"
#include "deviceOGL.h"
#include "samplerOGL.h"
#include "textureOGL.h"
#include "textureViewOGL.h"

namespace Neon::RHI
{
    void CommandListOGL::executeCommands()
    {
        for (const auto& command : m_commands)
        {
            command();
        }
    }

    CommandListOGL::CommandListOGL(DeviceOGL *device)
        : m_device(device) {  }

    void CommandListOGL::begin()
    {
        m_commands.clear();
    }

    void CommandListOGL::setUniformBuffer(const std::string& name, const Rc<Buffer>& buffer)
    {
        Debug::ensure(buffer != nullptr, "setUniformBuffer: null buffer");

        m_commands.emplace_back([buffer, name, this]
        {
            const uint32_t binding = getPipeline()->getShader()->getUBOLocation(name);
            const auto* uniformBufferOGL = dynamic_cast<BufferOGL*>(buffer.get());
            Debug::ensure(uniformBufferOGL != nullptr, "setUniformBuffer: buffer is not BufferOGL");
            Debug::ensure(uniformBufferOGL->getTarget() == GL_UNIFORM_BUFFER, "Buffer being set as Uniform Buffer was not created as GL_UNIFORM_BUFFER");
            uniformBufferOGL->bindBase(binding);
        });
    }

    void CommandListOGL::setTexture(const std::string& name, const Rc<TextureView>& texture)
    {
        Debug::ensure(texture != nullptr, "setTexture: null texture view");

        m_commands.emplace_back([name, texture, this]
        {
            const uint32_t binding = getPipeline()->getShader()->getTextureLocation(name);
            const auto* texViewOGL = dynamic_cast<const TextureViewOGL*>(texture.get());
            Debug::ensure(texViewOGL != nullptr, "setTexture: texture view is not TextureViewOGL");
            texViewOGL->bind(binding);
        });
    }

    void CommandListOGL::setSampler(const std::string& name, const Rc<Sampler>& sampler)
    {
        Debug::ensure(sampler != nullptr, "setSampler: null sampler");

        m_commands.emplace_back([name, sampler, this]
        {
            const uint32_t binding = getPipeline()->getShader()->getSamplerLocation(name);
            const auto* samplerOGL = dynamic_cast<const SamplerOGL*>(sampler.get());
            Debug::ensure(samplerOGL != nullptr, "setSampler: sampler is not SamplerOGL");
            samplerOGL->bind(binding);
        });
    }

    void CommandListOGL::setImage(const std::string &name, const Rc<TextureView> &texture, ImageAccess access)
    {
        Debug::ensure(texture != nullptr, "setImage: null texture view");

        m_commands.emplace_back([name, texture, this, access]
        {
            const uint32_t binding = getPipeline()->getShader()->getImageLocation(name);
            const auto* texViewOGL = dynamic_cast<const TextureViewOGL*>(texture.get());
            Debug::ensure(texViewOGL != nullptr, "setImage: texture view is not TextureViewOGL");
            texViewOGL->bindImage(binding, access);
        });
    }

    void CommandListOGL::beginRenderPass(const RenderPassDesc &desc)
    {
        m_commands.emplace_back([desc, this]
        {
            m_renderPassDesc = desc;
            const GLuint fbo = m_device->getOrCreateFb(desc);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            for(int i = 0; i < desc.colorAttachments.size(); i++)
            {
                const auto& attachment = desc.colorAttachments[i];

                if(i == 0)
                {
                    glViewport(0, 0, attachment.texture->getWidth(), attachment.texture->getHeight());
                }

                if (attachment.loadOp == LoadOp::Clear)
                {
                    glClearNamedFramebufferfv(fbo, GL_COLOR, i, &attachment.clearColor.r);
                }
            }

            if(desc.depthAttachment.texture != nullptr)
            {
                if (desc.depthAttachment.depthLoadOp == LoadOp::Clear)
                {
                    glDisable(GL_SCISSOR_TEST);
                    glDepthMask(GL_TRUE);
                    glEnable(GL_DEPTH_TEST);
                    glClearNamedFramebufferfv(fbo, GL_DEPTH, 0, &desc.depthAttachment.clearDepth);
                }
            }
        });
    }

    void CommandListOGL::endRenderPass()
    {
        m_commands.emplace_back([this]
        {
            std::vector<GLenum> invalidateAttachments;

            for (size_t i = 0; i < m_renderPassDesc.colorAttachments.size(); i++)
            {
                if (m_renderPassDesc.colorAttachments[i].storeOp == StoreOp::DontCare)
                {
                    invalidateAttachments.push_back(GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(i));
                }
            }

            if (m_renderPassDesc.depthAttachment.texture != nullptr)
            {
                if (m_renderPassDesc.depthAttachment.depthStoreOp == StoreOp::DontCare)
                {
                    invalidateAttachments.push_back(GL_DEPTH_ATTACHMENT);
                }
            }

            if (!invalidateAttachments.empty())
            {
                glInvalidateFramebuffer(GL_FRAMEBUFFER,
                                       static_cast<GLsizei>(invalidateAttachments.size()),
                                       invalidateAttachments.data());
            }
        });
    }

    void CommandListOGL::generateMipmaps(const Rc<Texture>& texture)
    {
        Debug::ensure(texture != nullptr, "generateMipmaps: null texture");

        m_commands.emplace_back([texture]
        {
            const auto* texOGL = dynamic_cast<TextureOGL*>(texture.get());
            Debug::ensure(texOGL != nullptr, "generateMipmaps: texture is not TextureOGL");
            texOGL->generateMipmaps();
        });
    }

    void CommandListOGL::setPipeline(const Rc<Pipeline>& pipeline)
    {
        Debug::ensure(pipeline != nullptr, "setPipeline: null pipeline");

        m_commands.emplace_back([this, pipeline]
        {
            this->m_pipeline = std::dynamic_pointer_cast<PipelineOGL>(pipeline);
            Debug::ensure(this->m_pipeline != nullptr, "setPipeline: pipeline is not PipelineOGL");
            this->m_pipeline->bind();
        });
    }

    void CommandListOGL::setVertexBuffer(const uint32_t index, const Rc<Buffer>& vertexBuffer)
    {
        Debug::ensure(vertexBuffer != nullptr, "setVertexBuffer: null vertex buffer");

        m_commands.emplace_back([this, vertexBuffer]
        {
            const auto* oglVertexBuffer = dynamic_cast<BufferOGL*>(vertexBuffer.get());
            Debug::ensure(oglVertexBuffer != nullptr, "setVertexBuffer: buffer is not BufferOGL");
            Debug::ensure(oglVertexBuffer->getTarget() == GL_ARRAY_BUFFER, "Buffer being set as Vertex Buffer was not created as GL_ARRAY_BUFFER");
            oglVertexBuffer->bind();

            const auto& attributes = getPipeline()->getVertexAttributes();
            for (const auto& attr : attributes)
            {
                if (attr.type == GL_INT || attr.type == GL_UNSIGNED_INT)
                {
                    glVertexAttribIPointer(attr.index, attr.size, attr.type, attr.stride, attr.pointer);
                }
                else
                {
                    glVertexAttribPointer(attr.index, attr.size, attr.type, GL_FALSE, attr.stride, attr.pointer);
                }
                glEnableVertexAttribArray(attr.index);
            }
        });
    }

    void CommandListOGL::setIndexBuffer(const Rc<Buffer>& indexBuffer, const IndexFormat indexFormat)
    {
        Debug::ensure(indexBuffer != nullptr, "setIndexBuffer: null index buffer");

        m_commands.emplace_back([indexBuffer, indexFormat, this]
        {
            this->m_indexFormat = indexFormat;
            const auto* indexBufferOGL = dynamic_cast<BufferOGL*>(indexBuffer.get());
            Debug::ensure(indexBufferOGL != nullptr, "setIndexBuffer: buffer is not BufferOGL");
            Debug::ensure(indexBufferOGL->getTarget() == GL_ELEMENT_ARRAY_BUFFER, "Buffer being set as Index Buffer was not created as GL_ELEMENT_ARRAY_BUFFER");
            indexBufferOGL->bind();
        });
    }

    void CommandListOGL::setScissor(ScissorRect rect)
    {
        m_commands.emplace_back([rect, this]
        {
            const int x = rect.x;
            const int y = static_cast<GLint>(m_renderPassDesc.colorAttachments[0].texture->getHeight() - (rect.y + rect.height)); // FIXME
            const int w = rect.width;
            const int h = rect.height;
            glScissor(x, y, w, h);
        });
    }

    void CommandListOGL::updateTexture(const Rc<Texture>& texture, const TextureUploadDesc& desc)
    {
        Debug::ensure(texture != nullptr, "texture is null");

        m_commands.emplace_back([texture, desc]
        {
            const auto* textureOGL = dynamic_cast<TextureOGL*>(texture.get());
            Debug::ensure(textureOGL != nullptr, "updateTexture: texture is not TextureOGL");
            textureOGL->setData(desc);
        });
    }

    void CommandListOGL::reserveBuffer(const Rc<Buffer>& buffer, size_t size)
    {
        Debug::ensure(buffer != nullptr, "null buffer");
        Debug::ensure(size > 0, "size must be > 0");

        m_commands.emplace_back([buffer, size]
        {
            auto* bufferOGL = dynamic_cast<BufferOGL*>(buffer.get());
            Debug::ensure(bufferOGL != nullptr, "buffer is not BufferOGL");
            bufferOGL->reserveSpace(size);
        });
    }

    void CommandListOGL::dispatch(const glm::ivec3& numGroups)
    {
        Debug::ensure(numGroups.x > 0 && numGroups.y > 0 && numGroups.z > 0, "numGroups must be positive");

        m_commands.emplace_back([numGroups]
        {
            glDispatchCompute(numGroups.x, numGroups.y, numGroups.z);
        });
    }

    void CommandListOGL::resourceBarrier(const Rc<Texture> &texture, ImageAccess nextAccess)
    {
        m_commands.emplace_back([]
        {
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        });
    }

    void CommandListOGL::addCustomCommand(const std::function<void()> &command)
    {
        m_commands.push_back(command);
    }

    void CommandListOGL::drawImpl(const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t firstVertex,
                                  const uint32_t firstInstance)
    {
        Debug::ensure(vertexCount > 0, "vertexCount must be > 0");

        m_commands.emplace_back([vertexCount, instanceCount, firstVertex, firstInstance]
        {
            glDrawArraysInstancedBaseInstance(GL_TRIANGLES,
                static_cast<GLint>(firstVertex),
                static_cast<GLsizei>(vertexCount),
                static_cast<GLsizei>(instanceCount),
                firstInstance);
        });
    }

    void CommandListOGL::drawIndexedImpl(const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex,
                                         const int vertexOffset, const uint32_t firstInstance)
    {
        Debug::ensure(indexCount > 0, "indexCount must be > 0");

        m_commands.emplace_back([indexCount, instanceCount, firstIndex, vertexOffset, firstInstance, this]
        {
            const GLenum indexType = ConvertOGL::indexFormatToGL(m_indexFormat);
            const uint32_t indexSize = ConvertOGL::indexFormatToSize(m_indexFormat);
            Debug::ensure(indexType != 0, "invalid index format");

            glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES,
                static_cast<GLsizei>(indexCount),
                indexType,
                reinterpret_cast<void*>(static_cast<uintptr_t>(firstIndex) * indexSize),
                static_cast<GLsizei>(instanceCount),
                vertexOffset,
                firstInstance);
        });
    }

    void CommandListOGL::updateBufferImpl(const Rc<Buffer>& buffer, void *data, uint32_t size)
    {
        Debug::ensure(buffer != nullptr, "null buffer");
        Debug::ensure(data != nullptr, "null data pointer");
        Debug::ensure(size > 0, "size must be > 0");

        std::vector dataCopy(static_cast<uint8_t*>(data), static_cast<uint8_t*>(data) + size);

        m_commands.emplace_back([buffer, dataCopy = std::move(dataCopy), size]
        {
            const auto* bufferOGL = dynamic_cast<BufferOGL*>(buffer.get());
            Debug::ensure(bufferOGL != nullptr, "buffer is not BufferOGL");
            bufferOGL->uploadData(dataCopy.data(), size);
        });
    }

    void CommandListOGL::readTextureImpl(const Rc<TextureView> &texture, const TextureReadDesc &desc, size_t destSize, void *dest)
    {
        Debug::ensure(texture != nullptr, "texture is null");

        m_commands.emplace_back([texture, desc, dest, destSize]
        {
            const auto* textureOGL = dynamic_cast<TextureViewOGL*>(texture.get());
            Debug::ensure(textureOGL != nullptr, "texture is not TextureOGL");
            textureOGL->getData(desc, destSize, dest);
        });
    }

    const Rc<PipelineOGL>& CommandListOGL::getPipeline() const
    {
        Debug::ensure(m_pipeline != nullptr, "no pipeline is bound");
        return m_pipeline;
    }
}
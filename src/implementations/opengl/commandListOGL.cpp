#include "commandListOGL.h"

#include "frameBufferOGL.h"
#include "pipelineOGL.h"

#include <cstring>

#include "bufferOGL.h"
#include "convertOGL.h"
#include "debug.h"
#include "samplerOGL.h"
#include "textureOGL.h"
#include "textureViewOGL.h"

namespace Neon::RHI
{
    void CommandListOGL::executeCommands()
    {
        for (const auto& command: commands)
        {
            command();
        }
    }

    void CommandListOGL::begin()
    {
        commands.clear();
    }

    void CommandListOGL::setUniformBuffer(const std::string& name, const Rc<Buffer>& buffer)
    {
        commands.emplace_back([buffer, name, this]
        {
            const uint32_t binding = getPipeline()->getShader()->getUBOLocation(name);
            const auto* uniformBufferOGL = dynamic_cast<BufferOGL*>(buffer.get());
            Debug::ensure(uniformBufferOGL->getTarget() == GL_UNIFORM_BUFFER, "Buffer being set as Uniform Buffer was not created as a Uniform Buffer");
            uniformBufferOGL->bindBase(binding);
        });
    }

    void CommandListOGL::setTexture(const std::string& name, const Rc<TextureView>& texture)
    {
        commands.emplace_back([name, texture, this]
        {
            const uint32_t binding = getPipeline()->getShader()->getSamplerLocation(name);
            dynamic_cast<const TextureViewOGL*>(texture.get())->bind(binding);
        });
    }

    void CommandListOGL::setSampler(const std::string& name, const Rc<Sampler>& sampler)
    {
        commands.emplace_back([name, sampler, this]
        {
            const uint32_t binding = getPipeline()->getShader()->getSamplerLocation(name);
            dynamic_cast<const SamplerOGL*>(sampler.get())->bind(binding);
        });
    }

    void CommandListOGL::generateMipmaps(const Rc<Texture>& texture)
    {
        commands.emplace_back([texture]
        {
            dynamic_cast<TextureOGL*>(texture.get())->generateMipmaps();
        });
    }

    void CommandListOGL::setPipeline(const Rc<Pipeline>& pipeline)
    {
        commands.emplace_back([this, pipeline]
        {
            this->pipeline = std::dynamic_pointer_cast<PipelineOGL>(pipeline);
            this->pipeline->bind();
        });
    }

    void CommandListOGL::setFramebuffer(const Rc<Framebuffer>& frameBuffer)
    {
        commands.emplace_back([frameBuffer, this]
        {
            framebuffer = std::dynamic_pointer_cast<FramebufferOGL>(frameBuffer);
            framebuffer->bind();
        });
    }

    void CommandListOGL::setVertexBuffer(const uint32_t index, const Rc<Buffer>& vertexBuffer)
    {
        commands.emplace_back([this, vertexBuffer]
        {
            const auto* oglVertexBuffer = dynamic_cast<BufferOGL*>(vertexBuffer.get());
            Debug::ensure(oglVertexBuffer->getTarget() == GL_ARRAY_BUFFER, "Buffer being set as Vertex Buffer was not created as a Vertex Buffer");
            oglVertexBuffer->bind();

            const auto& attributes = pipeline->getVertexAttributes();
            for (const auto& attr : attributes)
            {
                if (attr.type == GL_INT || attr.type == GL_UNSIGNED_INT)
                {
                    glVertexAttribIPointer(attr.index, attr.size, attr.type, attr.stride, attr.pointer);
                } else
                {
                    glVertexAttribPointer(attr.index, attr.size, attr.type, GL_FALSE, attr.stride, attr.pointer);
                }
                glEnableVertexAttribArray(attr.index);
            }
        });
    }

    void CommandListOGL::setIndexBuffer(const Rc<Buffer>& indexBuffer, const IndexFormat indexFormat)
    {
        commands.emplace_back([indexBuffer, indexFormat, this]
        {
            this->indexFormat = indexFormat;
            const auto* indexBufferOGL = dynamic_cast<BufferOGL*>(indexBuffer.get());
            Debug::ensure(indexBufferOGL->getTarget() == GL_ELEMENT_ARRAY_BUFFER, "Buffer being set as Index Buffer was not created as an Index Buffer");
            indexBufferOGL->bind();
        });
    }


    void CommandListOGL::clearColorTarget(const uint32_t target, const glm::vec4 color)
    {
        commands.emplace_back([target, color]
        {
            const float clearColor[4] = {color.r, color.g, color.b, color.a};
            glClearBufferfv(GL_COLOR, static_cast<int>(target), clearColor);
        });
    }

    void CommandListOGL::clearDepthStencil(const float value)
    {
        commands.emplace_back([value]
        {
            glClearDepth(value);
            glClear(GL_DEPTH_BUFFER_BIT);
        });
    }

    void CommandListOGL::setScissor(ScissorRect rect)
    {
        commands.emplace_back([rect, this]
        {
            Debug::ensure(framebuffer != nullptr, "Must have framebuffer bound to use setScissor(ScissorRect rect)");

            const int x = rect.x;
            const int y = static_cast<GLint>(framebuffer->getHeight() - (rect.y + rect.height));
            const int w = rect.width;
            const int h = rect.height;
            glScissor(x, y, w, h);
        });
    }

    void CommandListOGL::updateTexture(const Rc<Texture>& texture, TextureUploadDescription uploadDescription)
    {
        commands.emplace_back([texture, uploadDescription]
        {
            const auto* textureOGL = dynamic_cast<TextureOGL*>(texture.get());
            textureOGL->setData(uploadDescription);
        });
    }

    void CommandListOGL::reserveBuffer(const Rc<Buffer>& buffer, size_t size)
    {
        commands.emplace_back([buffer, size]
        {
            auto* bufferOGL = dynamic_cast<BufferOGL*>(buffer.get());
            bufferOGL->reserveSpace(size);
        });
    }

    void CommandListOGL::dispatch(const glm::ivec3& numGroups)
    {
        commands.emplace_back([numGroups]
        {
            glDispatchCompute(numGroups.x, numGroups.y, numGroups.z);
        });
    }

    void CommandListOGL::drawImpl(const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t firstVertex,
                                  const uint32_t firstInstance)
    {
        commands.emplace_back([vertexCount, instanceCount, firstVertex, firstInstance]
        {
            glDrawArraysInstancedBaseInstance(GL_TRIANGLES, static_cast<int>(firstVertex), static_cast<int>(vertexCount), static_cast<int>(instanceCount), firstInstance);
        });
    }

    void CommandListOGL::drawIndexedImpl(const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex,
                                         const int vertexOffset, const uint32_t firstInstance)
    {
        commands.emplace_back([indexCount, instanceCount, firstIndex, vertexOffset, firstInstance, this]
        {
            const GLenum indexType = ConvertOGL::indexFormatToGL(indexFormat);
            const uint32_t indexSize = ConvertOGL::indexFormatToSize(indexFormat);
            glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, static_cast<int>(indexCount), indexType, reinterpret_cast<void *>(firstIndex * indexSize), static_cast<int>(instanceCount), vertexOffset, firstInstance);
        });
    }

    void CommandListOGL::updateBufferImpl(const Rc<Buffer>& buffer, void *data, uint32_t size)
    {
        std::vector dataCopy(static_cast<uint8_t*>(data), static_cast<uint8_t*>(data) + size);

        commands.emplace_back([buffer, dataCopy = std::move(dataCopy), size]
        {
            const auto* bufferOGL = dynamic_cast<BufferOGL*>(buffer.get());
            bufferOGL->uploadData(dataCopy.data(), size);
        });
    }

    const Rc<PipelineOGL>& CommandListOGL::getPipeline() const
    {
        Debug::ensure(pipeline != nullptr, "Pipeline has not been set");
        return pipeline;
    }
}

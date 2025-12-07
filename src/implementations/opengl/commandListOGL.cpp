#include "commandListOGL.h"

#include "frameBufferOGL.h"
#include "pipelineOGL.h"

#include <cstring>

#include "bufferOGL.h"
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

    void CommandListOGL::setUniformBuffer(const std::string& name, Buffer* buffer)
    {
        commands.emplace_back([buffer, name, this]
        {
            const uint32_t binding = getPipeline()->getShader()->getUBOLocation(name);
            const auto* uniformBufferOGL = dynamic_cast<BufferOGL*>(buffer);
            Debug::ensure(uniformBufferOGL->getTarget() == GL_UNIFORM_BUFFER, "Buffer being set as Uniform Buffer was not created as a Uniform Buffer");
            uniformBufferOGL->bindBase(binding);
        });
    }

    void CommandListOGL::setTexture(const std::string& name, TextureView* texture)
    {
        commands.emplace_back([name, texture, this]
        {
            const uint32_t binding = getPipeline()->getShader()->getSamplerLocation(name);
            dynamic_cast<TextureViewOGL*>(texture)->bind(binding);
        });
    }

    void CommandListOGL::setSampler(const std::string& name, Sampler* sampler)
    {
        commands.emplace_back([name, sampler, this]
        {
            const uint32_t binding = getPipeline()->getShader()->getSamplerLocation(name);
            dynamic_cast<SamplerOGL*>(sampler)->bind(binding);
        });
    }

    void CommandListOGL::generateMipmaps(Texture *texture)
    {
        commands.emplace_back([texture]
        {
            dynamic_cast<TextureOGL*>(texture)->generateMipmaps();
        });
    }

    void CommandListOGL::setPipeline(Pipeline* pipeline)
    {
        commands.emplace_back([this, pipeline]
        {
            this->pipeline = dynamic_cast<PipelineOGL*>(pipeline);
            this->pipeline->bind();
        });
    }

    void CommandListOGL::setFramebuffer(Framebuffer* frameBuffer)
    {
        commands.emplace_back([frameBuffer]
        {
            dynamic_cast<FramebufferOGL*>(frameBuffer)->bind();
        });
    }

    void CommandListOGL::setVertexBuffer(const uint32_t index, Buffer* vertexBuffer)
    {
        commands.emplace_back([this, vertexBuffer]
        {
            const auto* oglVertexBuffer = dynamic_cast<BufferOGL*>(vertexBuffer);
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

    void CommandListOGL::setIndexBuffer(Buffer* indexBuffer, const IndexFormat indexFormat)
    {
        commands.emplace_back([indexBuffer]
        {
            const auto* indexBufferOGL = dynamic_cast<BufferOGL*>(indexBuffer);
            Debug::ensure(indexBufferOGL->getTarget() == GL_ELEMENT_ARRAY_BUFFER, "Buffer being set as Index Buffer was not created as an Index Buffer");
            indexBufferOGL->bind();
        });
    }


    void CommandListOGL::clearColorTarget(const uint32_t target, const glm::vec4 color)
    {
        commands.emplace_back([target, color]
        {
            glClearColor(color.x, color.y, color.z, color.w);
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
        commands.emplace_back([rect]
        {
            glScissor(rect.x, rect.y, rect.width, rect.height);
        });
    }

    void CommandListOGL::updateTexture(Texture* texture, TextureUploadDescription uploadDescription)
    {
        commands.emplace_back([texture, uploadDescription]
        {
            const auto* textureOGL = dynamic_cast<TextureOGL*>(texture);
            textureOGL->setData(uploadDescription);
        });
    }

    void CommandListOGL::reserveBuffer(Buffer* buffer, size_t size)
    {
        commands.emplace_back([buffer, size]
        {
            auto* bufferOGL = dynamic_cast<BufferOGL*>(buffer);
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
        commands.emplace_back([indexCount, instanceCount, firstIndex, vertexOffset, firstInstance]
        {
            glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, static_cast<int>(indexCount), GL_UNSIGNED_INT, reinterpret_cast<void *>(firstIndex), static_cast<int>(instanceCount), vertexOffset, firstInstance);
        });
    }

    void CommandListOGL::updateBufferImpl(Buffer* buffer, void *data, uint32_t size)
    {
        std::vector dataCopy(static_cast<uint8_t*>(data), static_cast<uint8_t*>(data) + size);

        commands.emplace_back([buffer, dataCopy = std::move(dataCopy), size]
        {
            const auto* bufferOGL = dynamic_cast<BufferOGL*>(buffer);
            bufferOGL->uploadData(dataCopy.data(), size);
        });
    }

    PipelineOGL* CommandListOGL::getPipeline() const
    {
        Debug::ensure(pipeline != nullptr, "Pipeline has not been set");
        return pipeline;
    }
}

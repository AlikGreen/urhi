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
        for (const auto& command : commands)
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
        Debug::ensure(buffer != nullptr, "setUniformBuffer: null buffer");

        commands.emplace_back([buffer, name, this]
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

        commands.emplace_back([name, texture, this]
        {
            const uint32_t binding = getPipeline()->getShader()->getSamplerLocation(name);
            const auto* texViewOGL = dynamic_cast<const TextureViewOGL*>(texture.get());
            Debug::ensure(texViewOGL != nullptr, "setTexture: texture view is not TextureViewOGL");
            texViewOGL->bind(binding);
        });
    }

    void CommandListOGL::setSampler(const std::string& name, const Rc<Sampler>& sampler)
    {
        Debug::ensure(sampler != nullptr, "setSampler: null sampler");

        commands.emplace_back([name, sampler, this]
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

        commands.emplace_back([name, texture, this, access]
        {
            const uint32_t binding = getPipeline()->getShader()->getImageLocation(name);
            const auto* texViewOGL = dynamic_cast<const TextureViewOGL*>(texture.get());
            Debug::ensure(texViewOGL != nullptr, "setImage: texture view is not TextureViewOGL");
            texViewOGL->bindImage(binding, access);
        });
    }

    void CommandListOGL::generateMipmaps(const Rc<Texture>& texture)
    {
        Debug::ensure(texture != nullptr, "generateMipmaps: null texture");

        commands.emplace_back([texture]
        {
            const auto* texOGL = dynamic_cast<TextureOGL*>(texture.get());
            Debug::ensure(texOGL != nullptr, "generateMipmaps: texture is not TextureOGL");
            texOGL->generateMipmaps();
        });
    }

    void CommandListOGL::setPipeline(const Rc<Pipeline>& pipeline)
    {
        Debug::ensure(pipeline != nullptr, "setPipeline: null pipeline");

        commands.emplace_back([this, pipeline]
        {
            this->pipeline = std::dynamic_pointer_cast<PipelineOGL>(pipeline);
            Debug::ensure(this->pipeline != nullptr, "setPipeline: pipeline is not PipelineOGL");
            this->pipeline->bind();
        });
    }

    void CommandListOGL::setFramebuffer(const Rc<Framebuffer>& frameBuffer)
    {
        Debug::ensure(frameBuffer != nullptr, "setFramebuffer: null framebuffer");

        commands.emplace_back([frameBuffer, this]
        {
            glViewport(0, 0, static_cast<int>(frameBuffer->getWidth()), static_cast<int>(frameBuffer->getHeight()));

            framebuffer = std::dynamic_pointer_cast<FramebufferOGL>(frameBuffer);
            Debug::ensure(framebuffer != nullptr, "setFramebuffer: framebuffer is not FramebufferOGL");
            framebuffer->bind();
        });
    }

    void CommandListOGL::setVertexBuffer(const uint32_t index, const Rc<Buffer>& vertexBuffer)
    {
        Debug::ensure(vertexBuffer != nullptr, "setVertexBuffer: null vertex buffer");

        commands.emplace_back([this, vertexBuffer]
        {
            const auto* oglVertexBuffer = dynamic_cast<BufferOGL*>(vertexBuffer.get());
            Debug::ensure(oglVertexBuffer != nullptr, "setVertexBuffer: buffer is not BufferOGL");
            Debug::ensure(oglVertexBuffer->getTarget() == GL_ARRAY_BUFFER, "Buffer being set as Vertex Buffer was not created as GL_ARRAY_BUFFER");
            oglVertexBuffer->bind();

            const auto& attributes = pipeline->getVertexAttributes();
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

        commands.emplace_back([indexBuffer, indexFormat, this]
        {
            this->indexFormat = indexFormat;
            const auto* indexBufferOGL = dynamic_cast<BufferOGL*>(indexBuffer.get());
            Debug::ensure(indexBufferOGL != nullptr, "setIndexBuffer: buffer is not BufferOGL");
            Debug::ensure(indexBufferOGL->getTarget() == GL_ELEMENT_ARRAY_BUFFER, "Buffer being set as Index Buffer was not created as GL_ELEMENT_ARRAY_BUFFER");
            indexBufferOGL->bind();
        });
    }

    void CommandListOGL::clearColorTarget(const uint32_t target, const glm::vec4 color)
    {
        commands.emplace_back([target, color]
        {
            const float clearColor[4] = { color.r, color.g, color.b, color.a };
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
            Debug::ensure(framebuffer != nullptr, "setScissor called without a framebuffer bound");

            const int x = rect.x;
            const int y = static_cast<GLint>(framebuffer->getHeight() - (rect.y + rect.height));
            const int w = rect.width;
            const int h = rect.height;
            glScissor(x, y, w, h);
        });
    }

    void CommandListOGL::updateTexture(const Rc<Texture>& texture, const TextureUploadDescription& uploadDescription)
    {
        Debug::ensure(texture != nullptr, "updateTexture: null texture");

        commands.emplace_back([texture, uploadDescription]
        {
            const auto* textureOGL = dynamic_cast<TextureOGL*>(texture.get());
            Debug::ensure(textureOGL != nullptr, "updateTexture: texture is not TextureOGL");
            textureOGL->setData(uploadDescription);
        });
    }

    void CommandListOGL::reserveBuffer(const Rc<Buffer>& buffer, size_t size)
    {
        Debug::ensure(buffer != nullptr, "reserveBuffer: null buffer");
        Debug::ensure(size > 0, "reserveBuffer: size must be > 0");

        commands.emplace_back([buffer, size]
        {
            auto* bufferOGL = dynamic_cast<BufferOGL*>(buffer.get());
            Debug::ensure(bufferOGL != nullptr, "reserveBuffer: buffer is not BufferOGL");
            bufferOGL->reserveSpace(size);
        });
    }

    void CommandListOGL::dispatch(const glm::ivec3& numGroups)
    {
        Debug::ensure(numGroups.x > 0 && numGroups.y > 0 && numGroups.z > 0, "dispatch: numGroups must be positive");

        commands.emplace_back([numGroups]
        {
            glDispatchCompute(numGroups.x, numGroups.y, numGroups.z);
        });
    }

    void CommandListOGL::resourceBarrier(const Rc<Texture> &texture, ImageAccess nextAccess)
    {
        commands.emplace_back([]
        {
            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        });
    }

    void CommandListOGL::drawImpl(const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t firstVertex,
                                  const uint32_t firstInstance)
    {
        Debug::ensure(vertexCount > 0, "drawImpl: vertexCount must be > 0");

        commands.emplace_back([vertexCount, instanceCount, firstVertex, firstInstance]
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
        Debug::ensure(indexCount > 0, "drawIndexedImpl: indexCount must be > 0");

        commands.emplace_back([indexCount, instanceCount, firstIndex, vertexOffset, firstInstance, this]
        {
            const GLenum indexType = ConvertOGL::indexFormatToGL(indexFormat);
            const uint32_t indexSize = ConvertOGL::indexFormatToSize(indexFormat);
            Debug::ensure(indexType != 0, "drawIndexedImpl: invalid index format");

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
        Debug::ensure(buffer != nullptr, "updateBufferImpl: null buffer");
        Debug::ensure(data != nullptr, "updateBufferImpl: null data pointer");
        Debug::ensure(size > 0, "updateBufferImpl: size must be > 0");

        std::vector<uint8_t> dataCopy(static_cast<uint8_t*>(data), static_cast<uint8_t*>(data) + size);

        commands.emplace_back([buffer, dataCopy = std::move(dataCopy), size]
        {
            const auto* bufferOGL = dynamic_cast<BufferOGL*>(buffer.get());
            Debug::ensure(bufferOGL != nullptr, "updateBufferImpl: buffer is not BufferOGL");
            bufferOGL->uploadData(dataCopy.data(), size);
        });
    }

    const Rc<PipelineOGL>& CommandListOGL::getPipeline() const
    {
        Debug::ensure(pipeline != nullptr, "getPipeline called but no pipeline is bound");
        return pipeline;
    }
}
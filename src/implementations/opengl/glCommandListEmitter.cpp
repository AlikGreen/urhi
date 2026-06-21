#include "glCommandListEmitter.h"

#include <ranges>
#include <unordered_set>
#include <glad/gl.h>

#include "clogr.h"
#include "DrawIndexedIndirectCommand.h"
#include "glBuffer.h"
#include "glContext.h"
#include "urhiToString.h"
#include "validation.h"
#include "glConvert.h"
#include "glDevice.h"
#include "glPipeline.h"
#include "glTextureView.h"
#include "glSampler.h"

namespace urhi
{
    GlCommandListEmitter::GlCommandListEmitter(GlDevice *device, const grl::Rc<CommandStream> &cmdStream)
            : m_device(device), m_cmdStream(cmdStream)
    { }

    void GlCommandListEmitter::emit(const CmdBeginRenderPass &c)
    {
        m_barrierBits = 0;
        uint32_t fbo = m_device->getOrCreateFramebuffer(c.desc);

        for(size_t i = 0; i < c.desc.colorAttachments.size(); i++)
        {
            auto& attachment = c.desc.colorAttachments[i];
            if(attachment.loadOp != LoadOp::Clear) continue;

            std::visit([i, fbo]<typename T>(T&& val)
            {
                using ValueType = std::remove_cvref_t<T>;

                if constexpr (std::is_same_v<ValueType, ClearColorFloat>)
                    glClearNamedFramebufferfv(fbo, GL_COLOR, i, &val.r);
                if constexpr (std::is_same_v<ValueType, ClearColorInt>)
                    glClearNamedFramebufferiv(fbo, GL_COLOR, i, &val.r);
                if constexpr (std::is_same_v<ValueType, ClearColorUint>)
                    glClearNamedFramebufferuiv(fbo, GL_COLOR, i, &val.r);
            },
            attachment.clearValue);
        }

        if(c.desc.depthAttachment.has_value() && c.desc.depthAttachment->loadOp == LoadOp::Clear)
            glClearNamedFramebufferfv(fbo, GL_DEPTH, 0, &c.desc.depthAttachment->clearDepth);

        m_renderPassActive = true;
        m_currentRenderPassDesc = c.desc;
        m_boundPipeline = nullptr;


        GlTextureView* mainTex{};
        if(c.desc.colorAttachments.size() > 0)
            mainTex = static_cast<GlTextureView*>(c.desc.colorAttachments[0].target.get());
        else if(c.desc.depthAttachment.has_value())
            mainTex = static_cast<GlTextureView*>(c.desc.depthAttachment->target.get());

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        const uint32_t width = mainTex->texture()->width();
        m_renderPassHeight = mainTex->texture()->height();


        glViewport(
            c.desc.renderArea.x,
            c.desc.renderArea.y,
            c.desc.renderArea.width != 0 ? c.desc.renderArea.width : width,
            c.desc.renderArea.height != 0 ? c.desc.renderArea.height : m_renderPassHeight
        );

        glScissor(
            c.desc.renderArea.x,
            c.desc.renderArea.y,
            c.desc.renderArea.width != 0 ? c.desc.renderArea.width : width,
            c.desc.renderArea.height != 0 ? c.desc.renderArea.height : m_renderPassHeight
        );
    }

    void GlCommandListEmitter::emit(const CmdReadbackTexture &c)
    {
        const auto& desc = c.desc;
        const auto glTex = static_cast<GlTexture*>(desc.texture.get());

        URHI_VALIDATE(desc.texture != nullptr,
            "Texture must not be null");

        URHI_VALIDATE(desc.mipLevel < glTex->mipLevelCount(),
            "Readback mip level {} out of range for texture with {} mip levels",
            desc.mipLevel, glTex->mipLevelCount());
        URHI_VALIDATE(desc.x + desc.width  <= glTex->width(desc.mipLevel),  "Readback region out of bounds - x offset ({}) + width ({}) is not less than texture width ({})", desc.x, desc.width, glTex->width(desc.mipLevel));
        URHI_VALIDATE(desc.y + desc.height <= glTex->height(desc.mipLevel), "Readback region out of bounds - y offset ({}) + height ({}) is not less than texture height ({})", desc.y, desc.height, glTex->height(desc.mipLevel));
        URHI_VALIDATE(desc.z + desc.depth  <= glTex->depth(desc.mipLevel),  "Readback region out of bounds - z offset ({}) + depth ({}) is not less than texture depth ({})", desc.z, desc.depth, glTex->depth(desc.mipLevel));

        const auto glRequest = static_cast<GlReadbackRequest*>(c.request.get());

        const uint32_t bufferSize = desc.width * desc.height * desc.depth * bytesPerPixel(glTex->format());

        glRequest->m_size = bufferSize;

        glCreateBuffers(1, &glRequest->m_buffer);
        glNamedBufferStorage(glRequest->m_buffer, bufferSize, nullptr, GL_MAP_READ_BIT);

        glBindBuffer(GL_PIXEL_PACK_BUFFER, glRequest->m_buffer);
        glGetTextureSubImage(
            glTex->handle(),
            desc.mipLevel,
            desc.x, desc.y, desc.z,
            desc.width, desc.height, desc.depth,
            GlConvert::pixelFormat(glTex->format()),
            GlConvert::pixelType(glTex->format()),
            bufferSize,
            nullptr
        );
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

        glRequest->m_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }

    void GlCommandListEmitter::emit(const CmdReadbackBuffer &c)
    {
        const auto desc = c.desc;
        const auto glBuffer = static_cast<GlBuffer*>(desc.buffer.get());

        URHI_VALIDATE(glBuffer != nullptr, "Buffer must not be null");

        URHI_VALIDATE(desc.offset < glBuffer->size(),
            "Readback region out of bounds - offset ({}) cannot be greater than size of buffer ({})",
            desc.offset, glBuffer->size());

        const auto glRequest = static_cast<GlReadbackRequest*>(c.request.get());

        glRequest->m_size = c.desc.size;

        glCreateBuffers(1, &glRequest->m_buffer);
        glNamedBufferStorage(glRequest->m_buffer, c.desc.size, nullptr, GL_MAP_READ_BIT);
        glCopyNamedBufferSubData(glBuffer->handle(), glRequest->m_buffer, desc.offset, 0, desc.size);
        glRequest->m_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }

    void GlCommandListEmitter::emit(const CmdUpdateBuffer &c)
    {
        const void* data = m_cmdStream->getData(c.dataOffset);
        const auto glBuffer = static_cast<GlBuffer*>(c.buffer.get());
        glNamedBufferSubData(glBuffer->handle(), c.dstOffset, c.size, data); // TODO dont make dynamic draw and eventually move to using glNamedBufferStorage
    }

    void GlCommandListEmitter::emit(const CmdUpdateTexture &c)
    {
        auto& desc = c.desc;
        const void* data = m_cmdStream->getData(c.dataOffset);

        const auto glTex = static_cast<GlTexture*>(desc.texture.get());

        switch(desc.texture->type())
        {
            case TextureType::Texture1D:
                glTextureSubImage1D(
                    glTex->handle(),
                    desc.mipLevel,
                    desc.x,
                    desc.width,
                    GlConvert::pixelFormat(glTex->format()),
                    GlConvert::pixelType(glTex->format()),
                    data
                );
                break;
            case TextureType::TextureCube:
            case TextureType::Texture2D:
                glTextureSubImage2D(
                    glTex->handle(),
                    desc.mipLevel,
                    desc.x, desc.y,
                    desc.width, desc.height,
                    GlConvert::pixelFormat(glTex->format()),
                    GlConvert::pixelType(glTex->format()),
                    data
                );
                break;
            case TextureType::Texture3D:
            case TextureType::Texture2DArray:
            case TextureType::TextureCubeArray:
                glTextureSubImage3D(
                    glTex->handle(),
                    desc.mipLevel,
                    desc.x, desc.y, desc.z,
                    desc.width, desc.height, desc.height,
                    GlConvert::pixelFormat(glTex->format()),
                    GlConvert::pixelType(glTex->format()),
                    data
                );
                break;
            default:
                break;
        }
    }

    void GlCommandListEmitter::emit(const CmdFillBuffer &c)
    {
        auto glBuffer = static_cast<GlBuffer*>(c.buffer.get());
        glClearNamedBufferSubData(
            glBuffer->handle(),
            GL_R32UI,
            c.offset,
            c.size,
            GL_RED,
            GL_UNSIGNED_INT,
            &c.value
        );
    }

    void GlCommandListEmitter::emit(const CmdCopyBuffer &c)
    {
        auto srcBuf = static_cast<GlBuffer*>(c.src.get());
        auto dstBuf = static_cast<GlBuffer*>(c.dst.get());
        glCopyNamedBufferSubData(
            srcBuf->handle(),
            dstBuf->handle(),
            c.srcOffset,
            c.dstOffset,
            c.size);
    }

    void GlCommandListEmitter::emit(const CmdSetBuffer &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline set - set sampler require a pipeline to be bound first");

        const auto info = m_boundPipeline->bufferBinding(c.nameHash);

        if (info.binding == GlPipeline::OPTIMIZED_OUT)
            return;

        URHI_VALIDATE(info.binding != m_boundPipeline->pushConstantBinding(),
        "Buffer '{}' is binding to the push constant slot - binding index collision",
        NameRegistry::getName(c.nameHash));

        URHI_VALIDATE(info.binding != GlPipeline::INVALID_TYPO, "Binding a resource ('{}') that the shader does not use or declare.", NameRegistry::getName(c.nameHash));

        const auto glBuffer = static_cast<GlBuffer*>(c.buffer.get());

        if(info.access != ResourceAccess::WriteOnly)
        {
            if(!glBuffer->pendingComputeWrite)
            {
                m_barrierBits |= GlConvert::bufferBarrierBit(glBuffer->usage());
                glBuffer->pendingComputeWrite = false;
            }
        }

        if(info.access != ResourceAccess::ReadOnly)
        {
            glBuffer->pendingComputeWrite = true;
        }

        glBindBufferBase(info.target, info.binding, glBuffer->handle());

        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline set - set sampler require a pipeline to be bound first");
    }

    void GlCommandListEmitter::emit(const CmdSetImage &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr,
        "No pipeline set - setImage requires a pipeline to be bound first");

        const auto info = m_boundPipeline->imageBinding(c.nameHash);
        if (info.binding == GlPipeline::OPTIMIZED_OUT) return;

        URHI_VALIDATE(info.binding != GlPipeline::INVALID_TYPO, "Image '{}' not declared in shader", NameRegistry::getName(c.nameHash));

        auto* glTex = static_cast<GlTextureView*>(c.texture.get());

        // Same two-flag pattern as buffers
        if(info.access != ResourceAccess::WriteOnly)
        {
            if(!glTex->pendingComputeWrite)
            {
                m_barrierBits |= GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;
                glTex->pendingComputeWrite = false;
            }
        }
        if (info.access != ResourceAccess::ReadOnly) glTex->pendingComputeWrite = true;

        // glBindImageTexture needs format and access mode
        GLenum access = GlConvert::resourceAccess(info.access);


        glBindImageTexture(
            info.binding,
            glTex->handle(),
            0,
            GL_FALSE,
            0,
            access,
            GlConvert::internalFormat(glTex->format()));
    }

    void GlCommandListEmitter::emit(const CmdSetTexture &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline bound");

        const auto glView = static_cast<GlTextureView*>(c.texture.get());

        const auto units = m_boundPipeline->textureBinding(c.nameHash);

        URHI_VALIDATE(units, "Texture ({}) not found in shader", NameRegistry::getName(c.nameHash));

        for (const auto& unit : *units)
        {
            glBindTextureUnit(unit.unit, glView->handle());
        }

        if(glView->pendingComputeWrite)
            m_barrierBits |= GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;
    }

    void GlCommandListEmitter::emit(const CmdSetSampler &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline bound");

        const auto glSampler = static_cast<GlSampler*>(c.sampler.get());

        const auto units = m_boundPipeline->samplerBinding(c.nameHash);

        URHI_VALIDATE(units, "Sampler ({}) not found in shader", NameRegistry::getName(c.nameHash));

        for (const auto& unit : *units)
        {
            glBindSampler(unit.unit, glSampler->handle());
        }
    }

    void GlCommandListEmitter::emit(const CmdPushConstants &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline set - push constants require a pipeline to be bound first");
        URHI_VALIDATE(c.size != 0, "No data uploaded");

        const int binding = m_boundPipeline->pushConstantBinding();

        URHI_VALIDATE(binding >= 0, "This shader attached to the bound pipeline has no push constants");

        const GLuint buffer = m_device->pushConstantsUbo();
        const void* data = m_cmdStream->getData(c.offset);

        glNamedBufferSubData(buffer, 0, c.size, data);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, buffer);
    }

    void GlCommandListEmitter::emit(const CmdSetVertexBuffer &c)
    {
        const auto glBuffer = static_cast<GlBuffer*>(c.buffer.get());

        glVertexArrayVertexBuffer(
            m_boundPipeline->vao(),
            c.index,
            glBuffer->handle(),
            0,
            m_boundPipeline->vertexStride(c.index)
        );
    }

    void GlCommandListEmitter::emit(const CmdSetIndexBuffer &c)
    {
        const auto glBuffer = static_cast<GlBuffer*>(c.buffer.get());
        m_currentIndexFormat = c.format;

        glVertexArrayElementBuffer(m_boundPipeline->vao(), glBuffer->handle());
    }

    void GlCommandListEmitter::emit(const CmdSetScissor &c)
    {
        URHI_VALIDATE(c.rect.x >= 0, "Scissor offset x ({}) is invalid - x offset must be >= 0", c.rect.x);
        URHI_VALIDATE(c.rect.y >= 0, "Scissor offset y ({}) is invalid - y offset must be >= 0", c.rect.y);

        // glScissor(c.rect.x, m_renderPassHeight - (c.rect.y + c.rect.height), c.rect.width, c.rect.height);
        glScissor(c.rect.x, c.rect.y, c.rect.width, c.rect.height);
    }

    void GlCommandListEmitter::emit(const CmdSetViewport &c) const
    {
        glViewport(c.viewport.x, c.viewport.y, c.viewport.width, c.viewport.height);
    }

    void GlCommandListEmitter::emit(const CmdGenerateMips &c)
    {
        const auto glTex = static_cast<GlTexture*>(c.texture.get());
        glGenerateTextureMipmap(glTex->handle());
    }

    void GlCommandListEmitter::emit(const CmdBlitTexture &c) const
    {
        auto srcTex = static_cast<GlTexture*>(c.desc.src.get());

        GLuint srcFb = srcTex->handle() == 0 ? 0 : m_device->blitReadFbo();
        if(srcTex->handle() != 0)
            glNamedFramebufferTexture(srcFb, GL_COLOR_ATTACHMENT0, srcTex->handle(), c.desc.srcMipLevel);

        auto dstTex = static_cast<GlTexture*>(c.desc.dst.get());

        GLuint dstFb = dstTex->handle() == 0 ? 0 : m_device->blitWriteFbo();
        if(dstTex->handle() != 0)
            glNamedFramebufferTexture(dstFb, GL_COLOR_ATTACHMENT0, dstTex->handle(), c.desc.dstMipLevel);

        int srcX0 = c.desc.srcOffset.x;
        int srcY0 = c.desc.srcOffset.y;

        int dstX0 = c.desc.dstOffset.x;
        int dstY0 = c.desc.dstOffset.y;

        int srcX1 = c.desc.srcOffset.x + c.desc.srcExtent.x;
        if(srcX1 == 0) srcX1 = srcTex->width(0);
        int srcY1 = c.desc.srcOffset.y + c.desc.srcExtent.y;
        if(srcY1 == 0) srcY1 = srcTex->height(0);

        int dstX1 = c.desc.dstOffset.x + c.desc.dstExtent.x;
        if(dstX1 == 0) dstX1 = dstTex->width(0);
        int dstY1 = c.desc.dstOffset.y + c.desc.dstExtent.y;
        if(dstY1 == 0) dstY1 = dstTex->height(0);

        if(dstFb != 0)
        {
            glBlitNamedFramebuffer(
                srcFb, dstFb,
                srcX0, srcY0,
                srcX1, srcY1,
                dstX0, dstY0,
                dstX1, dstY1,
                GL_COLOR_BUFFER_BIT,
                GlConvert::filter(c.desc.filter)
            );
        }else
        {
            glBlitNamedFramebuffer(
                srcFb, dstFb,
                srcX0, srcY1,  // <-- swapped
                srcX1, srcY0,  // <-- swapped
                dstX0, dstY0,
                dstX1, dstY1,
                GL_COLOR_BUFFER_BIT,
                GlConvert::filter(c.desc.filter)
            );
        }
    }

    void GlCommandListEmitter::emit(const CmdEndRenderPass &c)
    {
        glBindVertexArray(0);

        glDisable(GL_SCISSOR_TEST);

        glDepthMask(GL_TRUE);
        glDisable(GL_DEPTH_TEST);

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        glDisable(GL_BLEND);

        glStencilMask(0xFFFFFFFF);
        glDisable(GL_STENCIL_TEST);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_CULL_FACE);

        m_renderPassActive = false;
        m_boundPipeline = nullptr;
    }

    void GlCommandListEmitter::emit(const CmdBeginComputePass &c)
    {
        m_barrierBits = 0;
        m_computePassActive = true;
        m_boundPipeline = nullptr;
    }

    void GlCommandListEmitter::emit(const CmdEndComputePass &c)
    {
        m_computePassActive = false;
        m_boundPipeline = nullptr;
    }

    void GlCommandListEmitter::emit(const CmdBeginCommandBuffer &c)
    {

    }

    void GlCommandListEmitter::emit(const CmdSetGraphicsPipeline &c)
    {
        URHI_VALIDATE(c.pipeline != nullptr, "Failed to set pipeline - Pipeline must not be null");
        m_boundPipeline = std::static_pointer_cast<GlPipeline>(c.pipeline);

        m_boundPipeline->bind();
    }

    void GlCommandListEmitter::emit(const CmdSetComputePipeline &c)
    {
        URHI_VALIDATE(c.pipeline != nullptr, "Failed to set pipeline - Pipeline must not be null");
        m_boundPipeline = std::static_pointer_cast<GlPipeline>(c.pipeline);

        m_boundPipeline->bind();
    }

    void GlCommandListEmitter::emit(const CmdDrawIndexed &c)
    {
        if(m_barrierBits != 0)
            glMemoryBarrier(m_barrierBits);

        glDrawElementsInstancedBaseVertexBaseInstance(
            GlConvert::primitiveType(m_boundPipeline->primitiveType()),
            c.indexCount,
            GlConvert::indexFormat(m_currentIndexFormat),
            reinterpret_cast<void*>(c.firstIndex * GlConvert::sizeOf(m_currentIndexFormat)),
            c.instanceCount,
            c.vertexOffset,
            c.firstInstance);
    }

    void GlCommandListEmitter::emit(const CmdDraw &c)
    {
        if(m_barrierBits != 0)
            glMemoryBarrier(m_barrierBits);

        glDrawArraysInstancedBaseInstance(
            GlConvert::primitiveType(m_boundPipeline->primitiveType()),
            c.firstVertex,
            c.vertexCount,
            c.instanceCount,
            c.firstInstance);
    }

    void GlCommandListEmitter::emit(const CmdMultiDrawIndexedIndirect &c)
    {
        if(m_barrierBits != 0)
            glMemoryBarrier(m_barrierBits);

        const auto commandBuffer = static_cast<GlBuffer*>(c.commandsBuffer.get());

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer->handle());

        glMultiDrawElementsIndirect(
            GlConvert::primitiveType(m_boundPipeline->primitiveType()),
            GlConvert::indexFormat(m_currentIndexFormat),
            reinterpret_cast<void*>(c.startCommandIndex * sizeof(DrawIndexedIndirectCommand)),
            c.count,
            sizeof(DrawIndexedIndirectCommand));

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    }

    void GlCommandListEmitter::emit(const CmdMultiDrawIndexedIndirectCount &c)
    {
        if(m_barrierBits != 0)
            glMemoryBarrier(m_barrierBits);

        const auto commandBuffer = static_cast<GlBuffer*>(c.commandsBuffer.get());
        const auto countsBuffer = static_cast<GlBuffer*>(c.countsBuffer.get());

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer->handle());
        glBindBuffer(GL_PARAMETER_BUFFER, countsBuffer->handle());

        glMultiDrawElementsIndirectCount(
            GlConvert::primitiveType(m_boundPipeline->primitiveType()),
            GlConvert::indexFormat(m_currentIndexFormat),
            reinterpret_cast<void*>(c.startCommandIndex * sizeof(DrawIndexedIndirectCommand)),
            c.countIndex * sizeof(uint32_t),
            c.maxDrawCount,
            sizeof(DrawIndexedIndirectCommand));

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        glBindBuffer(GL_PARAMETER_BUFFER, 0);
    }

    void GlCommandListEmitter::emit(const CmdMultiDrawIndirect &c)
    {
        if(m_barrierBits != 0)
            glMemoryBarrier(m_barrierBits);

        const auto commandBuffer = static_cast<GlBuffer*>(c.commandsBuffer.get());

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer->handle());

        glMultiDrawArraysIndirect(
            GlConvert::primitiveType(m_boundPipeline->primitiveType()),
            reinterpret_cast<void*>(c.startCommandIndex * sizeof(DrawIndexedIndirectCommand)),
            c.count,
            sizeof(DrawIndexedIndirectCommand));

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    }

    void GlCommandListEmitter::emit(const CmdMultiDrawIndirectCount &c)
    {
        if(m_barrierBits != 0)
            glMemoryBarrier(m_barrierBits);

        const auto commandBuffer = static_cast<GlBuffer*>(c.commandsBuffer.get());
        const auto countsBuffer = static_cast<GlBuffer*>(c.countsBuffer.get());

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commandBuffer->handle());
        glBindBuffer(GL_PARAMETER_BUFFER, countsBuffer->handle());

        glMultiDrawArraysIndirectCount(
            GlConvert::primitiveType(m_boundPipeline->primitiveType()),
            reinterpret_cast<void*>(c.startCommandIndex * sizeof(DrawIndexedIndirectCommand)),
            c.countIndex * sizeof(uint32_t),
            c.maxDrawCount,
            sizeof(DrawIndexedIndirectCommand));

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        glBindBuffer(GL_PARAMETER_BUFFER, 0);
    }

    void GlCommandListEmitter::emit(const CmdDispatchCompute &c) const
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline set - compute dispatch requires a compute pipeline to be set first.");
        URHI_VALIDATE(m_computePassActive, "No compute pass active - You must start a compute pass before dispatching compute work.");

        if(m_barrierBits != 0)
            glMemoryBarrier(m_barrierBits);

        glDispatchCompute(c.groupsX, c.groupsY, c.groupsZ);
    }

    void GlCommandListEmitter::emit(const CmdDispatchComputeIndirect &c) const
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline set - compute dispatch requires a compute pipeline to be set first.");
        URHI_VALIDATE(m_computePassActive, "No compute pass active - You must start a compute pass before dispatching compute work.");

        if(m_barrierBits != 0)
            glMemoryBarrier(m_barrierBits);

        auto glBuffer = static_cast<GlBuffer*>(c.buffer.get());
        glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, glBuffer->handle());
        glDispatchComputeIndirect(c.offset);
        glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, 0);
    }
}

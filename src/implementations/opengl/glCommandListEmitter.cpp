#include "glCommandListEmitter.h"

#include <ranges>
#include <unordered_set>
#include <glad/gl.h>

#include "clogr.h"
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


    void GlCommandListEmitter::emit(const CmdDrawIndexed &c)
    {
        glDrawElementsInstancedBaseVertexBaseInstance(
            GlConvert::primitiveType(m_boundPipeline->primitiveType()),
            c.indexCount,
            GL_UNSIGNED_INT,
            (void*)c.firstIndex,
            c.instanceCount,
            c.vertexOffset,
            c.firstInstance);
    }

    void GlCommandListEmitter::emit(const CmdDraw &c)
    {
        glDrawArraysInstancedBaseInstance(
            GlConvert::primitiveType(m_boundPipeline->primitiveType()),
            c.firstVertex,
            c.firstVertex,
            c.instanceCount,
            c.firstInstance);
    }

    void GlCommandListEmitter::emit(const CmdBeginRenderPass &c)
    {
        uint32_t fbo = m_device->getOrCreateFramebuffer(c.desc);

        for(size_t i = 0; i < c.desc.colorAttachments.size(); i++)
        {
            auto& attachment = c.desc.colorAttachments[i];


            std::visit([i, fbo]<typename T>(T&& val)
            {
                if constexpr (std::is_same_v<T, ClearColorFloat>)
                    glClearNamedFramebufferfv(fbo, GL_COLOR, i, &val.r);
                if constexpr (std::is_same_v<T, ClearColorInt>)
                    glClearNamedFramebufferiv(fbo, GL_COLOR, i, &val.r);
                if constexpr (std::is_same_v<T, ClearColorUint>)
                    glClearNamedFramebufferuiv(fbo, GL_COLOR, i, &val.r);
            },
            attachment.clearValue);
        }

        if(c.desc.depthAttachment.has_value())
            glClearNamedFramebufferfv(fbo, GL_DEPTH, 0, &c.desc.depthAttachment->clearDepth);

        m_renderPassActive = true;
        m_currentRenderPassDesc = c.desc;
        m_boundPipeline = nullptr;

        int vpWidth = c.desc.renderArea.width;
        int vpHeight = c.desc.renderArea.height;
        int vpX = c.desc.renderArea.x;
        int vpY = c.desc.renderArea.y;

        if(vpWidth == 0 && vpHeight == 0)
        {
            GlTextureView* glTex{};
            if(c.desc.colorAttachments.size() > 0)
                glTex = static_cast<GlTextureView*>(c.desc.colorAttachments[0].target.get());
            else if(c.desc.depthAttachment.has_value())
                glTex = static_cast<GlTextureView*>(c.desc.depthAttachment->target.get());
            else
                return;

            vpWidth = glTex->texture()->width() - vpX;
            vpHeight = glTex->texture()->height() - vpY;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(vpX, vpY, vpWidth, vpHeight);

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
        glGetTextureImage(glTex->handle(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bufferSize, nullptr);
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
        const void* data = m_cmdStream->getData(c.offset);
        const auto glBuffer = static_cast<GlBuffer*>(c.buffer.get());
        glNamedBufferData(glBuffer->handle(), c.size, data, GlConvert::bufferUsage(glBuffer->usage())); // TODO dont make dynamic draw and eventually move to using glNamedBufferStorage
    }

    void GlCommandListEmitter::emit(const CmdUpdateTexture &c)
    {
        auto& desc = c.desc;
        const void* data = m_cmdStream->getData(c.offset);

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

    void GlCommandListEmitter::emit(const CmdSetBuffer &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline set - set sampler require a pipeline to be bound first");

        const int index = m_boundPipeline->bufferBinding(c.nameHash);
        URHI_VALIDATE(index >= 0, "Binding a resource that the shader doesnt use - you should not bind resources that are not used in the shader");

        const auto glBuffer = static_cast<GlBuffer*>(c.buffer.get());

        glBindBufferBase(GlConvert::bufferTarget(glBuffer->usage()), index, glBuffer->handle());
    }

    void GlCommandListEmitter::emit(const CmdSetTexture &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline bound");

        const auto glView = static_cast<GlTextureView*>(c.texture.get());

        const auto units = m_boundPipeline->samplerUnits(c.nameHash);
        if (units.empty())
        {
            URHI_WARNING(false, "Sampler not found in shader");
            return;
        }

        for (const auto& unit : units)
        {
            glBindTextureUnit(unit.unit, glView->handle());
        }
    }

    void GlCommandListEmitter::emit(const CmdSetSampler &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline bound");

        const auto glSampler = static_cast<GlSampler*>(c.sampler.get());

        const auto units = m_boundPipeline->samplerUnits(c.nameHash);
        if (units.empty())
        {
            URHI_WARNING(false, "Sampler not found in shader");
            return;
        }

        for (const auto& unit : units)
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

        glVertexArrayElementBuffer(m_boundPipeline->vao(), glBuffer->handle());
    }

    void GlCommandListEmitter::emit(const CmdSetScissor &c)
    {
        URHI_VALIDATE(c.rect.x >= 0, "Scissor offset x ({}) is invalid - x offset must be >= 0", c.rect.x);
        URHI_VALIDATE(c.rect.y >= 0, "Scissor offset y ({}) is invalid - y offset must be >= 0", c.rect.y);

        glScissor(c.rect.x, c.rect.y, c.rect.width, c.rect.height);
    }

    void GlCommandListEmitter::emit(const CmdSetViewport &c)
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

        int srcX1 = c.desc.srcOffset.x + c.desc.srcExtent.x;
        if(srcX1 == 0) srcX1 = srcTex->width(0);
        int srcY1 = c.desc.srcOffset.y + c.desc.srcExtent.y;
        if(srcY1 == 0) srcY1 = srcTex->height(0);

        int dstX1 = c.desc.srcOffset.x + c.desc.srcExtent.x;
        if(dstX1 == 0) dstX1 = dstTex->width(0);
        int dstY1 = c.desc.srcOffset.y + c.desc.srcExtent.y;
        if(dstY1 == 0) dstY1 = dstTex->height(0);

        glBlitNamedFramebuffer(
            srcFb, dstFb,
            c.desc.srcOffset.x,
            c.desc.srcOffset.y,
            srcX1,
            srcY1,
            c.desc.dstOffset.x,
            c.desc.dstOffset.y,
            dstX1,
            dstY1,
            GL_COLOR_BUFFER_BIT,
            GlConvert::filter(c.desc.filter)
        );
    }

    void GlCommandListEmitter::emit(const CmdDispatchCompute &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline set - compute dispatch requires a compute pipeline to be set first.");
        URHI_VALIDATE(m_computePassActive, "No compute pass active - You must start a compute pass before dispatching compute work.");

        glDispatchCompute(c.groupsX, c.groupsY, c.groupsZ);
    }

    void GlCommandListEmitter::emit(const CmdEndRenderPass &c)
    {
        m_renderPassActive = false;
        m_boundPipeline = nullptr;
    }

    void GlCommandListEmitter::emit(const CmdBeginComputePass &c)
    {
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
}

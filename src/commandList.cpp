#include "commandList.h"

#include "commandStream.h"
#include "validation.h"

namespace urhi
{
    CommandList::CommandList()
    {
        m_commands = grl::makeRc<CommandStream>();
        m_computePass = grl::Box<ComputePass>(new ComputePass(m_commands));
        m_renderPass = grl::Box<RenderPass>(new RenderPass(m_commands));
    }

    void CommandList::begin()
    {
        m_commands->clear();
        URHI_VALIDATE(!m_inUse, "Trying to begin same command list twice without submitting");
        m_inUse = true;
        m_commands->emplace(CmdBeginCommandBuffer{});
    }

    RenderPass& CommandList::beginRenderPass(const RenderPassDesc &desc)
    {
        m_commands->emplace(CmdBeginRenderPass{desc});
        return *m_renderPass;
    }

    ComputePass& CommandList::beginComputePass()
    {
        m_commands->emplace(CmdBeginComputePass{});
        return *m_computePass;
    }

    void CommandList::updateTexture(const TextureUploadDesc &desc)
    {
        const uint32_t size = desc.width * desc.height * desc.depth * desc.layerCount * bytesPerPixel(desc.texture->format());
        const uint32_t offset = m_commands->copyData(desc.data, size);
        m_commands->emplace(CmdUpdateTexture{ desc, offset, size });
    }

    void CommandList::generateMipmaps(const grl::Rc<Texture> &texture)
    {
        m_commands->emplace(CmdGenerateMips{texture});
    }

    void CommandList::blitTexture(const BlitTextureDesc &desc)
    {
        m_commands->emplace(CmdBlitTexture{desc});
    }

    void CommandList::updateBuffer(const grl::Rc<Buffer> &buffer, const void* data, const uint32_t size, const uint32_t offset)
    {
        const uint32_t dataOffset = m_commands->copyData(data, size);
        m_commands->emplace(CmdUpdateBuffer{ buffer, dataOffset, offset, size });
    }

    void CommandList::copyBuffer(const grl::Rc<Buffer> &src, const grl::Rc<Buffer> &dst, uint32_t size, uint32_t srcOffset, uint32_t dstOffset)
    {
        size = size != ~0u ? size : std::min(src->size(), dst->size());
        m_commands->emplace(CmdCopyBuffer{ src, dst, size, srcOffset, dstOffset });
    }

    grl::Rc<ReadbackRequest> CommandList::readback(const TextureReadbackDesc &desc)
    {
        auto request = createReadback();
        m_commands->emplace(CmdReadbackTexture{request, desc});
        return request;
    }

    grl::Rc<ReadbackRequest> CommandList::readback(const BufferReadbackDesc &desc)
    {
        auto request = createReadback();
        m_commands->emplace(CmdReadbackBuffer{request, desc});
        return request;
    }
}

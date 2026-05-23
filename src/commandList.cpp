#include "commandList.h"

#include "commandStream.h"
#include "validation.h"

namespace urhi
{
    CommandList::CommandList()
        : m_renderPass(m_commands)
    {
        m_commands = grl::makeRc<CommandStream>();
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
        return m_renderPass;
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

    void CommandList::updateBuffer(const grl::Rc<Buffer> &buffer, const void* data, const uint32_t size)
    {
        const uint32_t offset = m_commands->copyData(data, size);
        m_commands->emplace(CmdUpdateBuffer{ buffer, offset, size });
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

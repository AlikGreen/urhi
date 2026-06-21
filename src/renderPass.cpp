#include "renderPass.h"

#include "commandStream.h"
#include "DrawIndexedIndirectCommand.h"

namespace urhi
{
    void RenderPass::setPipeline(const grl::Rc<Pipeline> &pipeline)
    {
        m_commands->emplace(CmdSetGraphicsPipeline{pipeline});
    }

    void RenderPass::setBuffer(const std::string_view name, const grl::Rc<Buffer> &buffer)
    {
        m_commands->emplace(CmdSetBuffer{NameRegistry::getHash(name), buffer});
    }

    void RenderPass::setTexture(const std::string_view name, const grl::Rc<TextureView> &texture)
    {
        m_commands->emplace(CmdSetTexture{NameRegistry::getHash(name), texture});
    }

    void RenderPass::setSampler(const std::string_view name, const grl::Rc<Sampler> &sampler)
    {
        m_commands->emplace(CmdSetSampler{NameRegistry::getHash(name), sampler});
    }

    void RenderPass::pushConstants(const void *data, const uint32_t size)
    {
        const uint32_t offset = m_commands->copyData(data, size);
        m_commands->emplace(CmdPushConstants{ offset, size });
    }

    void RenderPass::setVertexBuffer(const uint32_t index, const grl::Rc<Buffer> &buffer)
    {
        m_commands->emplace(CmdSetVertexBuffer{index, buffer});
    }

    void RenderPass::setIndexBuffer(const grl::Rc<Buffer> &buffer, const IndexFormat format)
    {
        m_commands->emplace(CmdSetIndexBuffer{buffer, format});
    }

    void RenderPass::setScissor(const Rect2D rect)
    {
        m_commands->emplace(CmdSetScissor{rect});
    }

    void RenderPass::setViewport(const Viewport &viewport)
    {
        m_commands->emplace(CmdSetViewport{viewport});
    }

    void RenderPass::draw(const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t firstVertex, const uint32_t firstInstance)
    {
        m_commands->emplace(CmdDraw{vertexCount, instanceCount, firstVertex, firstInstance});
    }

    void RenderPass::drawIndexed(const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex, const int vertexOffset, const uint32_t firstInstance)
    {
        m_commands->emplace(CmdDrawIndexed{indexCount, instanceCount, firstIndex, vertexOffset, firstInstance});
    }

    void RenderPass::multiDrawIndirect(const grl::Rc<Buffer> &commandsBuffer, uint32_t count, uint32_t startCommandIndex)
    {
        m_commands->emplace(CmdMultiDrawIndirect{commandsBuffer, startCommandIndex, count});
    }

    void RenderPass::multiDrawIndirectCount(const grl::Rc<Buffer> &commandsBuffer, const grl::Rc<Buffer> &countsBuffer, uint32_t startCommandIndex, uint32_t countIndex, uint32_t maxDrawCount)
    {
        if(maxDrawCount == ~0u)
            maxDrawCount = (commandsBuffer->size() / sizeof(DrawIndirectCommand)) - startCommandIndex;

        m_commands->emplace(CmdMultiDrawIndirectCount{commandsBuffer, startCommandIndex, countsBuffer, countIndex, maxDrawCount});
    }

    void RenderPass::multiDrawIndexedIndirect(const grl::Rc<Buffer> &commandsBuffer, const uint32_t count, const uint32_t startCommandIndex)
    {
        m_commands->emplace(CmdMultiDrawIndexedIndirect{commandsBuffer, startCommandIndex, count});
    }

    void RenderPass::multiDrawIndexedIndirectCount(const grl::Rc<Buffer> &commandsBuffer, const grl::Rc<Buffer> &countsBuffer, const uint32_t startCommandIndex, const uint32_t countIndex, uint32_t maxDrawCount)
    {
        if(maxDrawCount == ~0u)
            maxDrawCount = (commandsBuffer->size() / sizeof(DrawIndexedIndirectCommand)) - startCommandIndex;

        m_commands->emplace(CmdMultiDrawIndexedIndirectCount{commandsBuffer, startCommandIndex, countsBuffer, countIndex, maxDrawCount});
    }

    void RenderPass::end()
    {
        m_commands->emplace(CmdEndRenderPass{});
    }

    RenderPass::RenderPass(const grl::Rc<CommandStream> &commands)
        : m_commands(commands) {  }
}

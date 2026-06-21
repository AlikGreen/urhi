#pragma once
#include <grl/mem.h>

#include "buffer.h"
#include "commandStream.h"
#include "pipeline.h"
#include "sampler.h"
#include "textureView.h"
#include "descriptions/rect2D.h"
#include "descriptions/viewport.h"
#include "enums/indexFormat.h"

namespace urhi
{
class RenderPass final
{
public:
    RenderPass(const RenderPass&)            = delete;
    RenderPass& operator=(const RenderPass&) = delete;
    RenderPass(RenderPass&&)                 = delete;
    RenderPass& operator=(RenderPass&&)      = delete;

    void setPipeline(const grl::Rc<Pipeline>& pipeline);

    void setBuffer(const HashedName &name, const grl::Rc<Buffer>& buffer) const
    {
        m_commands->emplace(CmdSetBuffer{name.hash, buffer});
    }

    void setTexture(const HashedName &name, const grl::Rc<TextureView>& texture) const
    {
        m_commands->emplace(CmdSetTexture{name.hash, texture});
    }

    void setSampler(const HashedName &name, const grl::Rc<Sampler>& sampler) const
    {
        m_commands->emplace(CmdSetSampler{name.hash, sampler});
    }

    void setBuffer(std::string_view name, const grl::Rc<Buffer>& buffer);
    void setTexture(std::string_view name, const grl::Rc<TextureView>& texture);
    void setSampler(std::string_view name, const grl::Rc<Sampler>& sampler);

    template<typename T>
    void pushConstants(T& data) { pushConstants(&data, sizeof(T)); }
    void pushConstants(const void* data, uint32_t size);

    void setVertexBuffer(uint32_t index, const grl::Rc<Buffer>& buffer);
    void setIndexBuffer(const grl::Rc<Buffer>& buffer, IndexFormat format);

    void setScissor(Rect2D rect);
    void setViewport(const Viewport &viewport);

    void draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0);
    void drawIndexed(uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int vertexOffset = 0, uint32_t firstInstance = 0);

    void multiDrawIndirect(const grl::Rc<Buffer>& commandsBuffer, uint32_t count, uint32_t startCommandIndex = 0);
    void multiDrawIndirectCount(const grl::Rc<Buffer>& commandsBuffer, const grl::Rc<Buffer>& countsBuffer, uint32_t startCommandIndex = 0, uint32_t countIndex = 0, uint32_t maxDrawCount = ~0u);
    void multiDrawIndexedIndirect(const grl::Rc<Buffer>& commandsBuffer, uint32_t count, uint32_t startCommandIndex = 0);
    void multiDrawIndexedIndirectCount(const grl::Rc<Buffer>& commandsBuffer, const grl::Rc<Buffer>& countsBuffer, uint32_t startCommandIndex = 0, uint32_t countIndex = 0, uint32_t maxDrawCount = ~0u);

    void end();
private:
    friend class CommandList;
    explicit RenderPass(const grl::Rc<CommandStream> &commands);

    grl::Rc<CommandStream> m_commands;
};
}

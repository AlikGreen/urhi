#pragma once
#include <type_traits>
#include <vector>

#include "pipeline.h"
#include "readbackRequest.h"
#include "sampler.h"
#include "descriptions/blitTextureDesc.h"
#include "descriptions/bufferReadbackDesc.h"
#include "descriptions/renderPassDesc.h"
#include "descriptions/textureReadbackDesc.h"
#include "descriptions/textureUploadDesc.h"
#include "descriptions/viewport.h"
#include "enums/indexFormat.h"

namespace urhi
{
struct CmdBeginRenderPass  { RenderPassDesc desc; };
struct CmdBeginComputePass {};
struct CmdEndRenderPass    {};
struct CmdEndComputePass   {};

struct CmdBeginCommandBuffer   {};

struct CmdUpdateTexture   { TextureUploadDesc desc; uint32_t offset; uint32_t size; }; // offset is offset into command lists data vector
struct CmdUpdateBuffer    { grl::Rc<Buffer> buffer; uint32_t offset; uint32_t size; }; // offset is offset into command lists data vector
struct CmdGenerateMips    { grl::Rc<Texture> texture; };
struct CmdBlitTexture     { BlitTextureDesc desc; };

struct CmdReadbackTexture { grl::Rc<ReadbackRequest> request; TextureReadbackDesc desc; };
struct CmdReadbackBuffer  { grl::Rc<ReadbackRequest> request; BufferReadbackDesc desc; };

struct CmdSetGraphicsPipeline { grl::Rc<Pipeline> pipeline; };
struct CmdSetComputePipeline  { grl::Rc<Pipeline> pipeline; };

struct CmdSetTexture { uint32_t nameHash; grl::Rc<TextureView> texture; };
struct CmdSetSampler { uint32_t nameHash; grl::Rc<Sampler> sampler; };
struct CmdSetBuffer  { uint32_t nameHash; grl::Rc<Buffer> buffer; };

struct CmdPushConstants    { uint32_t offset; uint32_t size; };

struct CmdSetVertexBuffer  { uint32_t index; grl::Rc<Buffer> buffer; };
struct CmdSetIndexBuffer   { grl::Rc<Buffer> buffer; IndexFormat format; };

struct CmdSetScissor       { Rect2D rect; };
struct CmdSetViewport      { Viewport viewport; };

struct CmdDrawIndexed      { uint32_t indexCount; uint32_t instanceCount; uint32_t firstIndex; int vertexOffset; uint32_t firstInstance; };
struct CmdDraw             { uint32_t vertexCount; uint32_t instanceCount; uint32_t firstVertex; uint32_t firstInstance; };
struct CmdDispatchCompute  { uint32_t groupsX; uint32_t groupsY; uint32_t groupsZ; };

    using Command = std::variant<
        CmdBeginCommandBuffer,
        CmdBeginRenderPass, CmdBeginComputePass,
        CmdEndRenderPass, CmdEndComputePass,
        CmdUpdateTexture, CmdUpdateBuffer,
        CmdGenerateMips, CmdBlitTexture,
        CmdReadbackTexture, CmdReadbackBuffer,
        CmdSetGraphicsPipeline,
        CmdSetComputePipeline,
        CmdSetTexture, CmdSetSampler, CmdSetBuffer,
        CmdSetVertexBuffer, CmdSetIndexBuffer,
        CmdPushConstants, CmdSetScissor, CmdSetViewport,
        CmdDrawIndexed, CmdDraw, CmdDispatchCompute
    >;

struct HashedName
{
    uint32_t hash;
    std::string_view text;

    template <std::size_t N>
    consteval HashedName(const char (&str)[N])
        : hash(grl::Hash::fnv1a32(str)), text(str, N - 1)
        {  }
};


class CommandStream
{
public:
    void push(Command command) { m_commands.push_back(std::move(command)); }
    void emplace(Command&& command) { m_commands.push_back(std::move(command)); }

    uint32_t copyData(const void* src, const uint32_t size)
    {
        const uint32_t offset = static_cast<uint32_t>(m_data.size());
        m_data.resize(offset + size);
        std::memcpy(m_data.data() + offset, src, size);
        return offset;
    }

    const void* getData(const uint32_t offset) const
    {
        return m_data.data() + offset;
    }

    void clear()
    {
        m_commands.clear();
        m_data.clear();
    }

    std::vector<Command>& commands() { return m_commands; }
private:
    std::vector<Command> m_commands;
    std::vector<std::byte> m_data;
};
}

#pragma once
#include "commandList.h"
#include <vulkan/vulkan.hpp>

#include "VkReadbackRequest.h"
#include "enums/queueType.h"

namespace urhi
{
class VkDevice;
class VkCommandListPool;

struct CmdBeginRenderPass  { RenderPassDesc desc; };
struct CmdBeginComputePass { RenderPassDesc desc; };
struct CmdEndRenderPass    {};
struct CmdEndComputePass   {};

struct CmdBeginCommandBuffer   {};

struct CmdUpdateTexture   { TextureUploadDesc desc; std::vector<uint8_t> data; };
struct CmdUpdateBuffer    { grl::Rc<Buffer> buffer; std::vector<uint8_t> data; };
struct CmdGenerateMips    { grl::Rc<Texture> texture; };

struct CmdReadbackTexture    { grl::Rc<VkReadbackRequest> request; TextureReadbackDesc desc; };

struct CmdSetPipeline      { grl::Rc<Pipeline> pipeline; vk::PipelineBindPoint bindPoint; };

struct CmdSetTexture       { std::string name; grl::Rc<TextureView> texture; };
struct CmdSetSampler       { std::string name; grl::Rc<Sampler> sampler; };
struct CmdSetImage         { std::string name; grl::Rc<TextureView> texture; ResourceAccess access; };

struct CmdSetUniformBuffer { std::string name; grl::Rc<Buffer> buffer; };
struct CmdSetStorageBuffer { std::string name; grl::Rc<Buffer> buffer; };
struct CmdPushConstants    { std::vector<uint8_t> data; };

struct CmdSetVertexBuffer  { uint32_t slot; grl::Rc<Buffer> buffer; };
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
    CmdGenerateMips, CmdReadbackTexture,
    CmdSetPipeline,
    CmdSetTexture, CmdSetSampler, CmdSetImage,
    CmdSetVertexBuffer, CmdSetIndexBuffer,
    CmdSetUniformBuffer, CmdSetStorageBuffer,
    CmdPushConstants, CmdSetScissor, CmdSetViewport,
    CmdDrawIndexed, CmdDraw, CmdDispatchCompute
>;

class VkCommandList final : public CommandList
{
public:
    explicit VkCommandList(QueueType queueType);
    void begin() override;

    grl::Rc<RenderPass> beginRenderPass(const RenderPassDesc &desc) override;
    grl::Rc<ComputePass> beginComputePass() override;

    void updateTexture(const TextureUploadDesc &desc) override;
    void generateMipmaps(const grl::Rc<Texture> &texture) override;

    grl::Rc<ReadbackRequest> readback(const TextureReadbackDesc& desc) override;
protected:
    void updateBufferImpl(const grl::Rc<Buffer> &buffer, void *data, uint32_t size) override;
private:
    friend class VkCommandListPool;
    friend class VkRenderPass;
    friend class VkComputePass;
    friend class VkDevice;

    QueueType m_queueType;
    grl::Rc<VkCommandListPool> m_pool;

    std::vector<Command> m_commands{};
    std::vector<grl::Rc<VkReadbackRequest>> m_readbackRequests{};
};
}


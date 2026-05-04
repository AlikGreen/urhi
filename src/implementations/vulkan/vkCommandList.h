#pragma once
#include "commandList.h"
#include <vulkan/vulkan.hpp>

#include "vkCommandQueue.h"
#include "vkReadbackRequest.h"
#include "vkSubmissionContext.h"
#include "enums/queueType.h"

namespace urhi
{
class VkDevice;
class VkCommandListPool;

struct CmdBeginRenderPass  { grl::Box<RenderPassDesc> desc; };
struct CmdBeginComputePass {};
struct CmdEndRenderPass    {};
struct CmdEndComputePass   {};

struct CmdBeginCommandBuffer   {};

struct CmdUpdateTexture   { grl::Box<TextureUploadDesc> desc; std::vector<uint8_t> data; };
struct CmdUpdateBuffer    { grl::Rc<Buffer> buffer; std::vector<uint8_t> data; };
struct CmdGenerateMips    { grl::Rc<Texture> texture; };
struct CmdBlitTexture     { grl::Rc<BlitTextureDesc> desc; };

struct CmdReadbackTexture { grl::Rc<VkReadbackRequest> request; grl::Box<TextureReadbackDesc> desc; };
struct CmdReadbackBuffer  { grl::Rc<VkReadbackRequest> request; BufferReadbackDesc desc; };

struct CmdSetPipeline     { grl::Rc<Pipeline> pipeline; vk::PipelineBindPoint bindPoint; };

struct CmdSetTexture { std::string name; grl::Rc<TextureView> texture; };
struct CmdSetSampler { std::string name; grl::Rc<Sampler> sampler; };
struct CmdSetBuffer  { std::string name; grl::Rc<Buffer> buffer; };

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
    CmdGenerateMips,
    CmdReadbackTexture, CmdReadbackBuffer,
    CmdBlitTexture, CmdSetPipeline,
    CmdSetTexture, CmdSetSampler, CmdSetBuffer,
    CmdSetVertexBuffer, CmdSetIndexBuffer,
    CmdPushConstants, CmdSetScissor, CmdSetViewport,
    CmdDrawIndexed, CmdDraw, CmdDispatchCompute
>;

class VkCommandList final : public CommandList
{
public:
    explicit VkCommandList(VkDevice* device, VkCommandQueue* queue, VkSubmissionContext* submissionContext);
    void begin() override;

    grl::Rc<RenderPass> beginRenderPass(const RenderPassDesc &desc) override;
    grl::Rc<ComputePass> beginComputePass() override;

    void updateTexture(const TextureUploadDesc &desc) override;
    void generateMipmaps(const grl::Rc<Texture> &texture) override;
    void blitTexture(const BlitTextureDesc& desc) override;

    grl::Rc<ReadbackRequest> readback(const TextureReadbackDesc& desc) override;
    grl::Rc<ReadbackRequest> readback(const BufferReadbackDesc &desc) override;

    [[nodiscard]] VkSubmissionContext* submissionContext() const { return m_submissionContext; }
    [[nodiscard]] VkCommandQueue* queue() const { return m_queue; }
    [[nodiscard]] const std::vector<Command>& commands() const { return m_commands; }
protected:
    void updateBufferImpl(const grl::Rc<Buffer> &buffer, void *data, uint32_t size) override;
private:
    VkDevice* m_device;
    VkSubmissionContext* m_submissionContext;
    VkCommandQueue* m_queue;

    std::vector<Command> m_commands{};
    std::vector<grl::Rc<VkReadbackRequest>> m_readbackRequests{};
};
}


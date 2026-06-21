#pragma once
#include "vkCommandList.h"
#include "vkCommandListTracker.h"

namespace urhi
{
class VkPipeline;
class VkCommandListEmitter
{
public:
    VkCommandListEmitter(VkDevice* device, VkCommandListTracker tracker, vk::CommandBuffer cmd, VkCommandQueue* commandQueue, uint64_t submitValue, const grl::Rc<CommandStream> &cmdStream);
    void endRecording();

    void emit(const CmdBeginRenderPass& c);
    void emit(const CmdEndRenderPass& c);

    void emit(const CmdBeginComputePass& c);
    void emit(const CmdEndComputePass& c);

    void emit(const CmdBeginCommandBuffer& c);

    void emit(const CmdSetGraphicsPipeline& c);
    void emit(const CmdSetComputePipeline& c);

    void emit(const CmdDrawIndexed& c);
    void emit(const CmdDraw& c);

    void emit(const CmdMultiDrawIndexedIndirect& c);
    void emit(const CmdMultiDrawIndexedIndirectCount& c);

    void emit(const CmdMultiDrawIndirect& c);
    void emit(const CmdMultiDrawIndirectCount& c);

    void emit(const CmdReadbackTexture& c);
    void emit(const CmdReadbackBuffer& c);

    void emit(const CmdUpdateBuffer& c);
    void emit(const CmdFillBuffer& c);
    void emit(const CmdCopyBuffer& c);
    void emit(const CmdUpdateTexture& c);

    void emit(const CmdSetBuffer& c);
    void emit(const CmdSetImage& c);
    void emit(const CmdSetTexture& c);
    void emit(const CmdSetSampler& c);

    void emit(const CmdPushConstants& c);

    void emit(const CmdSetVertexBuffer& c);
    void emit(const CmdSetIndexBuffer& c);

    void emit(const CmdSetScissor& c);
    void emit(const CmdSetViewport& c);

    void emit(const CmdGenerateMips& c);
    void emit(const CmdBlitTexture& c) const;

    void emit(const CmdDispatchCompute& c);
    void emit(const CmdDispatchComputeIndirect& c);
private:
    struct ResourceBinding;

    void pushDescriptors();

    VkDevice* m_device;
    vk::CommandBuffer m_cmd;
    VkCommandListTracker m_tracker;

    uint64_t m_submitValue;
    VkCommandQueue* m_commandQueue;

    grl::Rc<CommandStream> m_cmdStream;

    bool m_renderPassActive = false;
    bool m_computePassActive = false;
    RenderPassDesc m_currentRenderPassDesc;
    grl::Rc<VkPipeline> m_boundPipeline;
    vk::PipelineBindPoint m_boundPipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    std::unordered_map<uint32_t, ResourceBinding> m_currentBindings;

    uint32_t m_idx = 0;


    struct ResourceBinding
    {
        uint32_t set;
        uint32_t binding;
        vk::DescriptorType type;

        bool isImage;
        vk::DescriptorImageInfo imageInfo;
        vk::DescriptorBufferInfo bufferInfo;
    };
};
}

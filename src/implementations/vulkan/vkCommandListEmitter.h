#pragma once
#include "vkCommandList.h"
#include "vkCommandListTracker.h"
#include "descriptions/shaderReflection.h"

namespace urhi
{
class VkPipeline;
class VkCommandListEmitter
{
public:
    explicit VkCommandListEmitter(VkDevice* device, QueueType queueType, uint64_t submitValue, vk::Semaphore timeline, vk::CommandBuffer cmd, VkCommandListTracker tracker, grl::Rc<VkLinearStagingAllocator> stagingAllocator);

    void emit(const CmdBeginRenderPass& c);
    void emit(const CmdEndRenderPass& c);

    void emit(const CmdBeginComputePass& c);
    void emit(const CmdEndComputePass& c);

    void emit(const CmdBeginCommandBuffer& c);

    void emit(const CmdSetPipeline& c);

    void emit(const CmdDrawIndexed& c);
    void emit(const CmdDraw& c);

    void emit(const CmdReadbackTexture& c);

    void emit(const CmdUpdateBuffer& c);
    void emit(const CmdUpdateTexture& c);

    void emit(const CmdSetUniformBuffer& c);
    void emit(const CmdSetStorageBuffer& c);
    void emit(const CmdPushConstants& c);

    void emit(const CmdSetTexture& c);
    void emit(const CmdSetSampler& c);
    void emit(const CmdSetImage& c);

    void emit(const CmdSetVertexBuffer& c);
    void emit(const CmdSetIndexBuffer& c);

    void emit(const CmdSetScissor& c);
    void emit(const CmdSetViewport& c);

    void emit(const CmdGenerateMips& c);
    void emit(const CmdBlitTexture& c) const;

    void emit(const CmdDispatchCompute& c);
private:
    struct BoundResource;

    void pushDescriptors();

    VkDevice* m_device;
    vk::CommandBuffer m_cmd;
    VkCommandListTracker m_tracker;

    uint64_t m_submitValue;
    vk::Semaphore m_timeline;
    QueueType m_queueType;

    grl::Rc<VkLinearStagingAllocator> m_stagingAllocator;

    bool m_isRendering;
    RenderPassDesc m_currentRenderPassDesc;
    grl::Rc<VkPipeline> m_boundPipeline;
    vk::PipelineBindPoint m_boundPipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    std::unordered_map<std::string, BoundResource> m_boundResources;

    uint32_t m_idx = 0;

    struct BoundResource
    {
        ShaderReflection::ResourceType type;
        vk::DescriptorBufferInfo bufferInfo;
        vk::DescriptorImageInfo imageInfo;
    };
};
}

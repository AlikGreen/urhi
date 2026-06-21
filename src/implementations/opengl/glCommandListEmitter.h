#pragma once
#include "glCommandList.h"

namespace urhi
{
class GlPipeline;
class GlCommandListEmitter
{
public:
    GlCommandListEmitter(GlDevice* device, const grl::Rc<CommandStream> &cmdStream);

    void emit(const CmdBeginRenderPass& c);
    void emit(const CmdEndRenderPass& c);

    void emit(const CmdBeginComputePass& c);
    void emit(const CmdEndComputePass& c);

    void emit(const CmdBeginCommandBuffer& c);

    void emit(const CmdSetGraphicsPipeline& c);
    void emit(const CmdSetComputePipeline& c);

    void emit(const CmdReadbackTexture& c);
    void emit(const CmdReadbackBuffer& c);

    void emit(const CmdUpdateBuffer& c);
    void emit(const CmdUpdateTexture& c);
    void emit(const CmdFillBuffer& c);
    void emit(const CmdCopyBuffer& c);

    void emit(const CmdSetBuffer& c);
    void emit(const CmdSetImage& c);
    void emit(const CmdSetTexture& c);
    void emit(const CmdSetSampler& c);

    void emit(const CmdPushConstants& c);

    void emit(const CmdSetVertexBuffer& c);
    void emit(const CmdSetIndexBuffer& c);

    void emit(const CmdSetScissor& c);
    void emit(const CmdSetViewport& c) const;

    void emit(const CmdGenerateMips& c);
    void emit(const CmdBlitTexture& c) const;

    void emit(const CmdDrawIndexed& c);
    void emit(const CmdDraw& c);

    void emit(const CmdMultiDrawIndexedIndirect& c);
    void emit(const CmdMultiDrawIndexedIndirectCount& c);

    void emit(const CmdMultiDrawIndirect& c);
    void emit(const CmdMultiDrawIndirectCount& c);

    void emit(const CmdDispatchCompute& c) const;
    void emit(const CmdDispatchComputeIndirect& c) const;
private:
    struct ResourceBinding;

    GlDevice* m_device;
    grl::Rc<CommandStream> m_cmdStream;

    bool m_renderPassActive = false;
    bool m_computePassActive = false;

    RenderPassDesc m_currentRenderPassDesc;
    grl::Rc<GlPipeline> m_boundPipeline;
    IndexFormat m_currentIndexFormat{};

    GLenum m_barrierBits = 0;

    uint32_t m_renderPassHeight = 0;
};
}

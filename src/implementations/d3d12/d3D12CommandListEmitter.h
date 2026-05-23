#pragma once
#include "commandStream.h"
#include "d3D12Helper.h"

namespace urhi
{
class D3D12CommandListEmitter
{
public:
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

    void emit(const CmdReadbackTexture& c);
    void emit(const CmdReadbackBuffer& c);

    void emit(const CmdUpdateBuffer& c);
    void emit(const CmdUpdateTexture& c);

    void emit(const CmdSetBuffer& c);
    void emit(const CmdSetTexture& c);
    void emit(const CmdSetSampler& c);

    void emit(const CmdPushConstants& c);

    void emit(const CmdSetVertexBuffer& c);
    void emit(const CmdSetIndexBuffer& c);

    void emit(const CmdSetScissor& c);
    void emit(const CmdSetViewport& c);

    void emit(const CmdGenerateMips& c);
    void emit(const CmdBlitTexture& c);

    void emit(const CmdDispatchCompute& c);
private:
    ID3D12GraphicsCommandList10* m_cmd;
};
}

#include "d3D12CommandListEmitter.h"

namespace urhi
{
    void D3D12CommandListEmitter::emit(const CmdBeginRenderPass &c)
    {

        // D3D12_CPU_DESCRIPTOR_HANDLE  rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
        // rtvHandle.ptr = rtvHandle.ptr + (frameIndex * rtvDescriptorSize);
        // m_cmd->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
    }
}

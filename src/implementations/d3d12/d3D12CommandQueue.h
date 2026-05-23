#pragma once
#include "d3D12Helper.h"
#include "d3D12SubmissionContext.h"

namespace urhi
{
class D3D12CommandQueue
{
public:
    D3D12CommandQueue(D3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
    D3D12SubmissionContext& acquireSubmissionContext();

    ID3D12CommandQueue* queue() const { return m_queue.Get(); }
    ID3D12Fence* fence() const { return m_fence.Get(); }

    void submit(const grl::Rc<D3D12CommandList> &cmdList);
private:
    D3D12Device* m_device;
    D3D12_COMMAND_LIST_TYPE m_type;

    ComPtr<ID3D12CommandQueue> m_queue;
    ComPtr<ID3D12Fence> m_fence;

    uint64_t m_submitValue = 1;

    static constexpr uint32_t kMaxInFlight = 4;

    uint32_t m_nextSubmissionIndex = 0;
    std::array<D3D12SubmissionContext, kMaxInFlight> m_submissionContexts{};
};
}

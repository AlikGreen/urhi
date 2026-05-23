#pragma once
#include "d3D12Device.h"
#include "d3D12Helper.h"
#include "d3D12LinearCommandAllocator.h"

namespace urhi
{
class D3D12SubmissionContext
{
public:
    D3D12SubmissionContext() = default;
    void init(D3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
    void reset();

    D3D12CommandListAllocator& commandAllocator() { return m_commandListAllocator; }

    uint64_t lastSubmitValue() const { return m_lastSubmitValue; }
    void submit(const uint64_t value) { m_lastSubmitValue = value; }
private:
    D3D12CommandListAllocator m_commandListAllocator;

    uint64_t m_lastSubmitValue = 0;
};
}

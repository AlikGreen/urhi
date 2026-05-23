#include "d3D12SubmissionContext.h"

namespace urhi
{
    void D3D12SubmissionContext::init(D3D12Device* device, const D3D12_COMMAND_LIST_TYPE type)
    {
        m_commandListAllocator.init(device, type);
    }

    void D3D12SubmissionContext::reset()
    {

    }
}

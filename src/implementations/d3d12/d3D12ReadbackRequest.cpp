#include "d3D12ReadbackRequest.h"

namespace urhi
{
    bool D3D12ReadbackRequest::isReady() const
    {
        return false;
    }

    void D3D12ReadbackRequest::wait() const
    {
    }

    const void * D3D12ReadbackRequest::data() const
    {
        return nullptr;
    }

    size_t D3D12ReadbackRequest::size() const
    {
        return 0;
    }
}

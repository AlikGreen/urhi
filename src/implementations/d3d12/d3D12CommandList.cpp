#include "d3D12CommandList.h"

#include "d3D12ReadbackRequest.h"

namespace urhi
{
    D3D12CommandList::D3D12CommandList(const QueueType queueType)
        : m_queueType(queueType)
    {

    }

    grl::Rc<ReadbackRequest> D3D12CommandList::createReadback()
    {
        return grl::makeRc<D3D12ReadbackRequest>();
    }
}

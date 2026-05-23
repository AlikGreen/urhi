#include "d3D12LinearCommandAllocator.h"

namespace urhi
{
    void D3D12CommandListAllocator::init(D3D12Device *device, const D3D12_COMMAND_LIST_TYPE type)
    {
        m_device = device;
        m_type = type;

        const auto hr = device->device()->CreateCommandAllocator(type, IID_PPV_ARGS(&m_allocator));
        URHI_VALIDATE(SUCCEEDED(hr), "Failed to create command allocator");
    }


    ComPtr<ID3D12GraphicsCommandList> D3D12CommandListAllocator::acquireCommandList()
    {
        if(m_commandList != nullptr)
        {
            m_allocator.Reset();
            const auto hr = m_commandList->Reset(m_allocator.Get(), nullptr);
            URHI_VALIDATE(SUCCEEDED(hr), "Failed to reset command list");
            return m_commandList;
        }

        const auto hr = m_device->device()->CreateCommandList(
                0,
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                m_allocator.Get(),
                nullptr,
                IID_PPV_ARGS(&m_commandList));

        URHI_VALIDATE(SUCCEEDED(hr), "Failed to allocate command list.");

        return m_commandList;
    }
}

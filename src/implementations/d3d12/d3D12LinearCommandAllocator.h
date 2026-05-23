#pragma once
#include "d3D12Device.h"
#include "d3D12Helper.h"

namespace urhi
{
class D3D12CommandListAllocator
{
public:
    void init(D3D12Device* device, D3D12_COMMAND_LIST_TYPE type);

    ComPtr<ID3D12GraphicsCommandList> acquireCommandList();
private:
    D3D12Device* m_device{};

    D3D12_COMMAND_LIST_TYPE m_type{};
    ComPtr<ID3D12CommandAllocator> m_allocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
};
}
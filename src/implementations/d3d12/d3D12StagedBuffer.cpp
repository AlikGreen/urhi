#include "d3D12StagedBuffer.h"

namespace urhi
{
    D3D12StagedBuffer::D3D12StagedBuffer(D3D12Device *device, const BufferDesc& desc)
        : D3D12Buffer(device, desc.size)
    {
        D3D12_RESOURCE_DESC resourceDesc;
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Alignment = 0;
        resourceDesc.Width = m_size;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        D3D12MA::ALLOCATION_DESC allocationDesc = {};
        allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

        const auto hr = m_device->allocator()->CreateResource(
            &allocationDesc,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            &m_allocation,
            IID_NULL, nullptr);

        URHI_VALIDATE(SUCCEEDED(hr), "Failed to create buffer");

        m_buffer = m_allocation->GetResource();
    }

    D3D12StagedBuffer::~D3D12StagedBuffer()
    {
        m_device->destroy(m_life,
        [buffer = m_buffer, alloc = m_allocation](D3D12Device*)
        {
            buffer->Release();
            alloc->Release();
        });
    }
}

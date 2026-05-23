#include "d3D12MappedBuffer.h"

namespace urhi
{
    D3D12MappedBuffer::D3D12MappedBuffer(D3D12Device* device, const BufferDesc& desc)
        : D3D12Buffer(device, desc.size)
    {
        D3D12_HEAP_PROPERTIES heapProps;
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProps.CreationNodeMask = 1;
        heapProps.VisibleNodeMask = 1;


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
        allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

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

    D3D12MappedBuffer::~D3D12MappedBuffer()
    {
        m_device->destroy(m_life,
        [buffer = m_buffer, alloc = m_allocation](D3D12Device*)
        {
            buffer->Release();
            alloc->Release();
        });
    }
}

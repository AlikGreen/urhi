#pragma once
#include "d3D12Buffer.h"
#include "d3D12Device.h"

namespace urhi
{
class D3D12MappedBuffer final : public D3D12Buffer
{
public:
    D3D12MappedBuffer(D3D12Device* device, const BufferDesc& desc);
    ~D3D12MappedBuffer() override;
};
}

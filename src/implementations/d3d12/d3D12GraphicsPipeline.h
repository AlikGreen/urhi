#pragma once

#include "pipeline.h"
#include "descriptions/graphicsPipelineDesc.h"
#include "d3D12Helper.h"

namespace urhi
{
class D3D12Device;

class D3D12GraphicsPipeline final : public Pipeline
{
public:
    D3D12GraphicsPipeline(D3D12Device* device, const GraphicsPipelineDesc& desc);
    ID3D12RootSignature* createRootSignature(const GraphicsPipelineDesc& desc) const;
private:
    D3D12Device* m_device;
};
}

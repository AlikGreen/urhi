#pragma once
#include "device.h"
#include "d3D12Helper.h"
#include "descriptions/deviceDesc.h"

namespace urhi
{
class D3D12Device final : public Device
{
public:
    D3D12Device(const DeviceDesc& desc);
    grl::Rc<Pipeline> createPipeline(const GraphicsPipelineDesc &desc) override;
    grl::Rc<Pipeline> createPipeline(const ComputePipelineDesc &desc) override;

    grl::Rc<CommandList> acquireCommandList(QueueType queueType) override;

    grl::Rc<Texture> createTexture(const TextureDesc &desc) override;
    grl::Rc<Sampler> createSampler(const SamplerDesc &desc) override;
    grl::Rc<TextureView> createTextureView(const TextureViewDesc &desc) override;

    grl::Rc<Shader> createShader(const ShaderEntryPoint &entryPoint) override;

    grl::Rc<Buffer> createBuffer(const BufferDesc &desc) override;

    void submit(const grl::Rc<CommandList> &commandList) override;
private:
    ComPtr<ID3D12Device> m_device;
};
}

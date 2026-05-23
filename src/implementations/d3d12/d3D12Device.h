#pragma once
#include "device.h"
#include "d3D12Helper.h"
#include "d3D12Lifetime.h"
#include "descriptions/deviceDesc.h"

namespace urhi
{
    class D3D12CommandList;
    class D3D12Context;
    class D3D12CommandQueue;

class D3D12Device final : public Device
{
public:
    explicit D3D12Device(const DeviceDesc& desc, D3D12Context* context);
    ~D3D12Device() override;

    ID3D12Device* device() const { return m_device.Get(); }

    grl::Rc<Pipeline> createPipeline(const GraphicsPipelineDesc &desc) override;
    grl::Rc<Pipeline> createPipeline(const ComputePipelineDesc &desc) override;

    grl::Rc<CommandList> acquireCommandList(QueueType queueType) override;

    grl::Rc<Texture> createTexture(const TextureDesc &desc) override;
    grl::Rc<Sampler> createSampler(const SamplerDesc &desc) override;
    grl::Rc<TextureView> createTextureView(const TextureViewDesc &desc) override;

    grl::Rc<Shader> createShader(const ShaderEntryPoint &entryPoint) override;

    grl::Rc<Buffer> createBuffer(const BufferDesc &desc) override;

    void submit(const grl::Rc<CommandList> &commandList) override;

    void waitIdle() override;

    void destroy(D3D12Lifetime life, std::function<void(D3D12Device*)> callback) { m_destroyQueue.emplace_back(life, callback); }

    D3D12CommandQueue* queue(QueueType type) const { return m_queues[static_cast<uint8_t>(type)].get(); }
    D3D12MA::Allocator* allocator() const { return m_allocator; }
    D3D12Context* context() const { return m_context; }
private:
    void tryCollectGarbage();

    D3D12Context* m_context;

    ComPtr<ID3D12Device> m_device;
    ComPtr<IDXGIAdapter4> m_adapter;
    D3D12MA::Allocator* m_allocator;

    struct QueuedDestroy
    {
        D3D12Lifetime life;
        std::function<void(D3D12Device*)> cb;
    };

    std::vector<QueuedDestroy> m_destroyQueue{};
    std::array<grl::Box<D3D12CommandQueue>, static_cast<size_t>(QueueType::Transfer)> m_queues;
    std::vector<grl::Rc<D3D12CommandList>> m_commandLists{};
};
}

#include "d3D12Device.h"

#include "d3D12CommandList.h"
#include "d3D12CommandQueue.h"
#include "d3D12Context.h"
#include "d3D12GraphicsPipeline.h"
#include "d3D12MappedBuffer.h"
#include "d3D12Shader.h"
#include "d3D12StagedBuffer.h"
#include "d3D12Texture.h"
#include "D3D12TextureView.h"


namespace urhi
{
    D3D12Device::D3D12Device(const DeviceDesc &desc, D3D12Context* context)
        : m_context(context)
    {
        auto hr = context->dxgiFactory()->EnumAdapterByGpuPreference(
           0,
           DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
           IID_PPV_ARGS(&m_adapter)
        );

        URHI_VALIDATE(SUCCEEDED(hr), "Failed to find valid adapter");

        hr = D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
        URHI_VALIDATE(SUCCEEDED(hr), "Unable to create D3D12 device");

#if defined(URHI_ENABLE_VALIDATION)
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
        hr = m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData));
        URHI_VALIDATE(SUCCEEDED(hr), "Device does not support D3D_ROOT_SIGNATURE_VERSION_1_1");
#endif

        D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
        allocatorDesc.pDevice = m_device.Get();
        allocatorDesc.pAdapter = m_adapter.Get();
        allocatorDesc.Flags = static_cast<D3D12MA::ALLOCATOR_FLAGS>(D3D12MA_RECOMMENDED_ALLOCATOR_FLAGS);

        hr = D3D12MA::CreateAllocator(&allocatorDesc, &m_allocator);
        URHI_VALIDATE(SUCCEEDED(hr), "Failed to create d3d12 memory allocation allocator");

        m_queues[0] = grl::makeBox<D3D12CommandQueue>(this, D3D12_COMMAND_LIST_TYPE_DIRECT);
        m_queues[1] = grl::makeBox<D3D12CommandQueue>(this, D3D12_COMMAND_LIST_TYPE_COMPUTE);
        m_queues[2] = grl::makeBox<D3D12CommandQueue>(this, D3D12_COMMAND_LIST_TYPE_COPY);
    }

    D3D12Device::~D3D12Device()
    {
        m_allocator->Release();
    }

    grl::Rc<Pipeline> D3D12Device::createPipeline(const GraphicsPipelineDesc &desc)
    {
        return grl::makeRc<D3D12GraphicsPipeline>(this, desc);
    }

    grl::Rc<Pipeline> D3D12Device::createPipeline(const ComputePipelineDesc &desc)
    {
        return nullptr; // TODO implement
    }

    grl::Rc<Texture> D3D12Device::createTexture(const TextureDesc &desc)
    {
        return grl::makeRc<D3D12Texture>(this, desc);
    }

    grl::Rc<Sampler> D3D12Device::createSampler(const SamplerDesc &desc)
    {
        return nullptr; // TODO implement
    }

    grl::Rc<TextureView> D3D12Device::createTextureView(const TextureViewDesc &desc)
    {
        return grl::makeRc<D3D12TextureView>(this, desc);
    }

    grl::Rc<Shader> D3D12Device::createShader(const ShaderEntryPoint &entryPoint)
    {
        return grl::makeRc<D3D12Shader>(this, entryPoint);
    }

    grl::Rc<Buffer> D3D12Device::createBuffer(const BufferDesc &desc)
    {
        URHI_VALIDATE(desc.size != 0, "Invalid buffer size ({}) - buffer size must be greater than 0 and less than vram available", desc.size);
        URHI_VALIDATE(desc.usage != BufferUsage::None, "Invalid buffer usage - buffer usage must not be BufferUsage::None");
        URHI_VALIDATE(!(hasFlag(desc.usage, BufferUsage::Static) && hasFlag(desc.usage, BufferUsage::Dynaimic)), "Invalid buffer usage - buffer usage cannot have BufferUsage::Dynamic and BufferUsage::Static");

        if ((hasFlag(desc.usage, BufferUsage::Uniform) && !hasFlag(desc.usage, BufferUsage::Static)) || hasFlag(desc.usage, BufferUsage::Dynaimic))
            return grl::makeRc<D3D12MappedBuffer>(this, desc);

        return grl::makeRc<D3D12StagedBuffer>(this, desc);
    }

    grl::Rc<CommandList> D3D12Device::acquireCommandList(QueueType queueType)
    {
        for(auto cmd : m_commandLists)
        {
            if(!cmd->inUse())
                return cmd;
        }

        URHI_WARNING(m_commandLists.size() > 16, "You have more than 16 actively recording command lists. You may be forgetting to submit them.");

        m_commandLists.emplace_back(grl::makeRc<D3D12CommandList>(queueType));
        return m_commandLists.back();
    }

    void D3D12Device::submit(const grl::Rc<CommandList> &commandList)
    {
        const auto dxCmd = std::static_pointer_cast<D3D12CommandList>(commandList);
        dxCmd->submit();
        auto* commandQueue = m_queues.at(static_cast<size_t>(dxCmd->queueType())).get();
        commandQueue->submit(dxCmd);

        tryCollectGarbage();
    }

    void D3D12Device::waitIdle()
    {

    }

    void D3D12Device::tryCollectGarbage()
    {
        if(m_destroyQueue.empty()) return;

        for (size_t i = 0; i < m_destroyQueue.size(); i++ )
        {
            auto& [lifetime, callback] = m_destroyQueue[i];

            if(!lifetime.lastQueue)
            {
                callback(this);
                m_destroyQueue[i] = std::move(m_destroyQueue.back());
                m_destroyQueue.pop_back();
                i--;
            }else
            {
                const uint64_t completeValue = lifetime.lastQueue->fence()->GetCompletedValue();

                if(lifetime.lastSubmit <= completeValue)
                {
                    callback(this);
                    m_destroyQueue[i] = std::move(m_destroyQueue.back());
                    m_destroyQueue.pop_back();
                    i--;
                }
            }
        }

        URHI_WARNING(m_destroyQueue.size() <= 512, "Too many resource destroys queued - {} destroys queued, you may have a memory leak", m_destroyQueue.size());
    }
}

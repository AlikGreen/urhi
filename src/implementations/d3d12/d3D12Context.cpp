#include "d3D12Context.h"

#include "clogr.h"
#include "d3D12CommandQueue.h"
#include "d3D12Device.h"
#include "d3D12Swapchain.h"
#include "d3D12Window.h"

namespace urhi
{
    D3D12Context::D3D12Context(const ContextDesc& desc)
        : m_cachePath(desc.cachePath)
    {
        uint32_t factoryFlags = 0;

        if(desc.debug)
        {
            ComPtr<ID3D12Debug1> debug;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
            {
                debug->EnableDebugLayer();
                debug->SetEnableGPUBasedValidation(true);
            }
            factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
        }

        const auto hr = CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_factory));
        URHI_VALIDATE(SUCCEEDED(hr), "Failed to create factory");
    }

    grl::Rc<Device> D3D12Context::createDevice(const DeviceDesc &desc)
    {
        return grl::makeRc<D3D12Device>(desc, this);
    }

    grl::Rc<Window> D3D12Context::createWindow(const WindowDesc &desc)
    {
        return grl::makeRc<D3D12Window>(desc);
    }

    grl::Rc<Swapchain> D3D12Context::createSwapchain(const SwapchainDesc &desc)
    {
        return grl::makeRc<D3D12Swapchain>(desc, this, m_device->queue(QueueType::Graphics)->queue());
    }

    clogr::Logger& D3D12Context::logger()
    {
        return *clogr::defaultLogger();
    }
}

#include "d3D12Context.h"

#include "clogr.h"
#include "d3D12Device.h"
#include "d3D12Window.h"

namespace urhi
{
    grl::Rc<Device> D3D12Context::createDevice(const DeviceDesc &desc)
    {
        return grl::makeRc<D3D12Device>(desc);
    }

    grl::Rc<Window> D3D12Context::createWindow(const WindowDesc &desc)
    {
        return grl::makeRc<D3D12Window>(desc);
    }

    clogr::Logger& D3D12Context::logger()
    {
        return *clogr::defaultLogger();
    }
}

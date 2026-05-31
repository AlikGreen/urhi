#include "glContext.h"

#include "clogr.h"
#include "glDevice.h"
#include "glSwapchain.h"
#include "glWindow.h"
#include "validation.h"

namespace urhi
{

    GlContext::GlContext(const ContextDesc& desc)
        : m_cachePath(desc.cachePath)
    {

    }

    grl::Rc<Device> GlContext::createDevice(const DeviceDesc& desc)
    {
        return grl::makeRc<GlDevice>(desc, this);
    }

    grl::Rc<Window> GlContext::createWindow(const WindowDesc &desc)
    {
        return grl::makeRc<GlWindow>(desc, this);
    }

    grl::Rc<Swapchain> GlContext::createSwapchain(const SwapchainDesc &desc)
    {
        return grl::makeRc<GlSwapchain>(desc);
    }

    clogr::Logger & GlContext::logger()
    {
        return *m_logger;
    }
}

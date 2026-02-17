#include "vkContext.h"
#include "vkDevice.h"
#include "vkSwapchain.h"
#include "vkWindow.h"

namespace urhi
{
    VkContext::VkContext()
    {
        constexpr bool useValidationLayers = true;

        vkb::InstanceBuilder builder;
        auto instRet = builder.set_app_name("Placeholder")
            .request_validation_layers(useValidationLayers)
            .use_default_debug_messenger()
            .require_api_version(1, 3, 0)
            .build();

        m_vkbInstance = instRet.value();
        m_instance = m_vkbInstance.instance;
        m_debugMessenger = m_vkbInstance.debug_messenger;
    }

    grl::Rc<Device> VkContext::createDevice(const DeviceDesc& desc)
    {
        return grl::makeRc<VkDevice>(desc, this);
    }

    grl::Rc<Window> VkContext::createWindow(const WindowDesc &desc)
    {
        return grl::makeRc<VkWindow>(desc, this);
    }

    grl::Rc<Swapchain> VkContext::createSwapchain(const SwapchainDesc &desc)
    {
        m_swapchain = grl::makeRc<VkSwapchain>(desc);;
        return m_swapchain;
    }

    vkb::Instance VkContext::getVkbInstance() const
    {
        return m_vkbInstance;
    }

    vk::Instance VkContext::getVkInstance() const
    {
        return m_instance;
    }

    grl::Rc<VkSwapchain> VkContext::getSwapchain() const
    {
        return m_swapchain;
    }
}

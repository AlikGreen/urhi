#include "vkContext.h"

#include "clogr.h"
#include "vkDevice.h"
#include "vkSwapchain.h"
#include "vkWindow.h"

namespace urhi
{
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* data,
        void* pUserData)
    {
        clogr::error("[{}] {}", data->pMessageIdName ? data->pMessageIdName : "VK", data->pMessage);
        return VK_FALSE;
    }


    VkContext::VkContext()
    {
        constexpr bool useValidationLayers = true;

        vkb::InstanceBuilder builder;
        auto instRet = builder.set_app_name("placeholder_app_name")
            .request_validation_layers(useValidationLayers)
            .set_debug_callback(debugCallback)
            .set_debug_messenger_severity(
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            .set_debug_messenger_type(
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
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

    clogr::Logger & VkContext::logger()
    {
        return *m_logger;
    }
}

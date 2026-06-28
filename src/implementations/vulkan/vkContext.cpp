#include "vkContext.h"

#include "clogr.h"
#include "vkDevice.h"
#include "vkSwapchain.h"
#include "vkWindow.h"

namespace urhi
{
    enum class ValidationMode
    {
        None,           // No validation — shipping/release
        Standard,       // Basic validation layers only (fast, catches API misuse)
        Sync,           // Synchronization validation (finds missing barriers)
        GpuAssisted,    // GPU-assisted (finds OOB descriptor/buffer access)
        Full,           // Standard + Best Practices + Debug Printf disabled but everything else
    };

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* data,
        void* pUserData)
    {
        const char* id = data->pMessageIdName ? data->pMessageIdName : "VK";
        const char* msg = data->pMessage       ? data->pMessage       : "(no message)";

        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            clogr::error("[VK] [{}] {}", id, msg);
        #if defined(_MSC_VER) && defined(_DEBUG)
            __debugbreak();
        #endif
        }
        else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            clogr::warn("[VK] [{}] {}", id, msg);
        }
        else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        {
            clogr::info("[VK] [{}] {}", id, msg);
        }
        else
        {
            clogr::trace("[VK] [{}] {}", id, msg);
        }

        return VK_FALSE;
    }

    VkContext::VkContext(const ContextDesc& desc)
    {
        constexpr auto validationMode = ValidationMode::Full;
        bool enableValidation = (validationMode != ValidationMode::None) && desc.debug;

        vkb::InstanceBuilder builder;
        builder
            .set_app_name("placeholder_app_name")
            .require_api_version(1, 3, 0)
            .request_validation_layers(enableValidation);

        if (enableValidation)
        {
            builder
                .set_debug_callback(debugCallback)
                .set_debug_messenger_severity(
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT    |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
                .set_debug_messenger_type(
                    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT    |
                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT);

            switch (validationMode)
            {
            case ValidationMode::Standard:
                break;

            case ValidationMode::Sync:
                builder
                    .add_validation_feature_enable(
                        VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT)
                    .add_validation_feature_enable(
                        VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT);
                break;

            case ValidationMode::GpuAssisted:
                builder
                    .add_validation_feature_enable(
                        VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT)
                    .add_validation_feature_enable(
                        VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT)
                    .add_validation_feature_enable(
                        VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT);
                break;

            case ValidationMode::Full:
                builder
                    .add_validation_feature_enable(
                        VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT)
                    .add_validation_feature_enable(
                        VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT)
                    .add_validation_feature_enable(
                        VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT);
                    // Use debug printf to add print statements inside shaders:
                    //   debugPrintfEXT("value = %f", myValue);
                break;

            case ValidationMode::None:
                break;
            }
        }

        auto instRet = builder.build();
        if (!instRet)
        {
            clogr::error("Failed to create Vulkan instance: {}", instRet.error().message());
            std::abort();
        }

        m_vkbInstance = instRet.value();
        m_instance = m_vkbInstance.instance;
        m_debugMessenger = m_vkbInstance.debug_messenger;

        if (enableValidation)
        {
            clogr::info("Vulkan validation mode: {}", [&]() -> const char* {
                switch (validationMode)
                {
                    case ValidationMode::None:        return "None";
                    case ValidationMode::Standard:    return "Standard";
                    case ValidationMode::Sync:        return "Sync";
                    case ValidationMode::GpuAssisted: return "GPU-Assisted";
                    case ValidationMode::Full:        return "Full";
                }
                return "Unknown";
            }());
        }
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
        m_swapchain = grl::makeRc<VkSwapchain>(desc);
        return m_swapchain;
    }

    const vkb::Instance& VkContext::getVkbInstance() const
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

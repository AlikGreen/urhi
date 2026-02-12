#pragma once
#include "context.h"
#include "VkBootstrap.h"
#include <vulkan/vulkan.hpp>

namespace urhi
{
class VkContext final : public Context
{
public:
    VkContext();
    grl::Rc<Device> createDevice(const DeviceDesc& desc) override;
    grl::Rc<Window> createWindow(const WindowDesc& desc) override;
    grl::Rc<Swapchain> createSwapchain(const SwapchainDesc& desc) override;

    vkb::Instance getVkbInstance() const;
    vk::Instance getVkInstance() const;
private:
    vk::Instance m_instance;
    vk::DebugUtilsMessengerEXT m_debugMessenger;
    vkb::Instance m_vkbInstance;
};

}

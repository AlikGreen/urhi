#pragma once
#include "context.h"
#include "VkBootstrap.h"
#include <vulkan/vulkan.hpp>

namespace urhi
{
class VkSwapchain;

class VkContext final : public Context
{
public:
    VkContext();
    grl::Rc<Device> createDevice(const DeviceDesc& desc) override;
    grl::Rc<Window> createWindow(const WindowDesc& desc) override;
    grl::Rc<Swapchain> createSwapchain(const SwapchainDesc& desc) override;

    [[nodiscard]] vkb::Instance getVkbInstance() const;
    [[nodiscard]] vk::Instance getVkInstance() const;
    [[nodiscard]] grl::Rc<VkSwapchain> getSwapchain() const;

private:
    vk::Instance m_instance;
    vk::DebugUtilsMessengerEXT m_debugMessenger;
    vkb::Instance m_vkbInstance;
    grl::Rc<VkSwapchain> m_swapchain;
};

}

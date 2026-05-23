#pragma once

#include "context.h"
#include <vulkan/vulkan.hpp>
#include "VkBootstrap.h"

#include "logger.h"

namespace urhi
{
class VkSwapchain;

class VkContext final : public Context
{
public:
    VkContext(const ContextDesc& desc);
    grl::Rc<Device> createDevice(const DeviceDesc& desc) override;
    grl::Rc<Window> createWindow(const WindowDesc& desc) override;
    grl::Rc<Swapchain> createSwapchain(const SwapchainDesc& desc) override;

    [[nodiscard]] const vkb::Instance& getVkbInstance() const;
    [[nodiscard]] vk::Instance getVkInstance() const;
    [[nodiscard]] grl::Rc<VkSwapchain> getSwapchain() const;

    clogr::Logger& logger() override;

private:
    vk::Instance m_instance;
    vk::DebugUtilsMessengerEXT m_debugMessenger;
    vkb::Instance m_vkbInstance;
    grl::Rc<VkSwapchain> m_swapchain;

    grl::Rc<clogr::Logger> m_logger;
};

}

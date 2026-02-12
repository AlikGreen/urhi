#pragma once
#include <vulkan/vulkan.hpp>


#include "swapchain.h"
#include "vkDevice.h"
#include "vkWindow.h"
#include "descriptions/swapchainDesc.h"

namespace urhi
{
class VkSwapchain final : public Swapchain
{
public:
    explicit VkSwapchain(const SwapchainDesc& desc);
    void resize(uint32_t width, uint32_t height) override;

    [[nodiscard]] uint32_t acquireNextImage() override;
    [[nodiscard]] const std::vector<grl::Rc<TextureView>> & getTextureViews() const override;

    void present(uint32_t imageIndex) override;
private:
    grl::Rc<VkDevice> m_device;
    grl::Rc<VkWindow> m_window;

    VkColorSpaceKHR m_colorSpace;
    VkPresentModeKHR m_presentMode;

    VkSwapchainKHR m_handle{};
    VkFormat m_imageFormat{};

    std::vector<vk::Image> m_images{};
    std::vector<vk::ImageView> m_imageViews{};
    VkExtent2D m_extent{};
};
}

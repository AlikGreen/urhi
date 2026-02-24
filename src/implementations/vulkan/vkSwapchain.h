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
    ~VkSwapchain() override;
    void resize(uint32_t width, uint32_t height) override;

    [[nodiscard]] grl::Rc<TextureView> acquireNextImage() override;

    void present() override;
    vk::Semaphore consumeSemaphore();
private:
    bool m_semaphoreConsumed = true;
    uint32_t m_imageIndex = 0;
    grl::Rc<VkDevice> m_device;
    grl::Rc<VkWindow> m_window;

    vk::ColorSpaceKHR m_colorSpace;
    vk::PresentModeKHR m_presentMode;

    vk::SwapchainKHR m_handle{nullptr};
    vk::Format m_imageFormat{};
    uint32_t m_width{}, m_height{};

    std::vector<grl::Rc<Texture>> m_textures{};
    std::vector<grl::Rc<TextureView>> m_textureViews{};
    vk::Extent2D m_extent{};

    uint32_t m_frameIndex = 0;
    uint32_t m_maxFramesInFlight{};

    struct Frame
    {
        vk::Semaphore imageAvailableSemaphore;
        vk::CommandPool transitionPool;
        vk::CommandBuffer transitionCmd;
        vk::Fence inFlightFence;
        uint64_t maxTimelineValue = 0;
    };

    std::vector<vk::Semaphore> m_renderFinishedSemaphores;

    std::vector<Frame> m_frames;
};
}

#include "vkSwapchain.h"

#include "clogr.h"
#include "VkBootstrap.h"
#include "vkConvert.h"

namespace urhi
{
    VkSwapchain::VkSwapchain(const SwapchainDesc& desc)
    {
        m_device = std::dynamic_pointer_cast<VkDevice>(desc.device);
        m_window = std::dynamic_pointer_cast<VkWindow>(desc.window);
        m_presentMode = VkConvert::presentMode(desc.presentMode);
        m_colorSpace = VkConvert::colorSpace(desc.colorSpace);
        resize(desc.width, desc.height);
    }

    void VkSwapchain::resize(const uint32_t width, const uint32_t height)
    {
        if(m_handle != nullptr)
        {
            m_device->getHandle().destroySwapchainKHR(m_handle);

            for (const auto m_imageView : m_imageViews)
            {
                m_device->getHandle().destroyImageView(m_imageView);
            }
        }

        vkb::SwapchainBuilder swapchainBuilder{ m_device->getPhysicalDevice(), m_device->getHandle(), m_window->getSurface() };

        m_imageFormat = VK_FORMAT_B8G8R8A8_UNORM;

        vkb::Swapchain vkbSwapchain = swapchainBuilder
            //.use_default_format_selection()
            .set_desired_format(VkSurfaceFormatKHR{ .format = m_imageFormat, .colorSpace = m_colorSpace })
            //use vsync present mode
            .set_desired_present_mode(m_presentMode)
            .set_desired_extent(width, height)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .build()
            .value();

        m_extent = vkbSwapchain.extent;
        //store swapchain and its related images
        m_handle = vkbSwapchain.swapchain;

        auto rawImages = vkbSwapchain.get_images().value();
        m_images.clear();
        m_images.reserve(rawImages.size());
        for (VkImage raw : rawImages)
        {
            m_images.emplace_back(static_cast<vk::Image>(raw));
        }

        auto rawImageViews = vkbSwapchain.get_image_views().value();
        m_imageViews.clear();
        m_imageViews.reserve(rawImageViews.size());
        for (VkImageView rawView : rawImageViews)
        {
            m_imageViews.emplace_back(static_cast<vk::ImageView>(rawView));
        }
    }

    uint32_t VkSwapchain::acquireNextImage()
    {
        const auto acquireInfo = vk::AcquireNextImageInfoKHR(
            m_handle,
            INT64_MAX
        );

        uint32_t imageIndex = 0;
        auto res = m_device->getHandle().acquireNextImage2KHR(&acquireInfo, &imageIndex);
        return imageIndex;
    }

    const std::vector<grl::Rc<TextureView>> & VkSwapchain::getTextureViews() const
    {
        clogr::ensure(false, "not implemented");
    }

    void VkSwapchain::present(const uint32_t imageIndex)
    {
        m_device->m_currentFrameIndex = imageIndex;

    }
}

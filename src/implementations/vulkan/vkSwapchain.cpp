#include "vkSwapchain.h"

#include "clogr.h"
#include "validation.h"
#include "VkBootstrap.h"
#include "vkConvert.h"
#include "vkTextureView.h"

namespace urhi
{
    VkSwapchain::VkSwapchain(const SwapchainDesc& desc)
    {
        m_device = std::dynamic_pointer_cast<VkDevice>(desc.device);
        m_window = std::dynamic_pointer_cast<VkWindow>(desc.window);
        m_presentMode = VkConvert::presentMode(desc.presentMode);
        m_colorSpace = VkConvert::colorSpace(desc.colorSpace);
        m_imageFormat = VkConvert::pixelFormat(desc.format, m_device.get());

        const vk::SurfaceCapabilitiesKHR caps = m_device->getPhysicalDevice().getSurfaceCapabilitiesKHR(m_window->getSurface());
        m_maxFramesInFlight = caps.minImageCount;

        m_frames.resize(m_maxFramesInFlight);
        for (auto& frame : m_frames)
        {
            frame.imageAvailableSemaphore = m_device->handle().createSemaphore({});
            frame.renderFinishedSemaphore = m_device->handle().createSemaphore({});
            frame.transitionPool = m_device->handle().createCommandPool({vk::CommandPoolCreateFlagBits::eTransient, m_device->queue(QueueType::Graphics)->family()});
            vk::CommandBufferAllocateInfo allocInfo
            {
                frame.transitionPool,
                vk::CommandBufferLevel::ePrimary,
                1
            };
            frame.transitionCmd = m_device->handle().allocateCommandBuffers(allocInfo).front();;
        }

        uint32_t width = desc.width;
        uint32_t height = desc.height;

        if(width == 0) width = m_window->width();
        if(height == 0) height = m_window->height();

        resize(width, height);
    }

    VkSwapchain::~VkSwapchain()
    {
        m_device->handle().waitIdle();
        for (const auto& frame : m_frames)
        {
            m_device->handle().destroySemaphore(frame.imageAvailableSemaphore);
            m_device->handle().destroyCommandPool(frame.transitionPool);
        }
    }

    void VkSwapchain::resize(const uint32_t width, const uint32_t height)
    {
        m_width = width;
        m_height = height;
        VkSwapchainKHR oldHandle = m_handle;

        vkb::SwapchainBuilder swapchainBuilder{
            m_device->getPhysicalDevice(),
            m_device->handle(),
            m_window->getSurface()
        };

        auto result = swapchainBuilder
            .set_old_swapchain(oldHandle)
            .set_desired_format(VkSurfaceFormatKHR{
                .format = static_cast<VkFormat>(m_imageFormat),
                .colorSpace = static_cast<VkColorSpaceKHR>(m_colorSpace)
            })
            .set_desired_present_mode(static_cast<VkPresentModeKHR>(m_presentMode))
            .set_desired_extent(width, height)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .build();

        if (!result)
        {
            clogr::error("Failed to create new swapchain when resizing");
            return;
        }

        vkb::Swapchain vkbSwapchain = result.value();

        m_extent = vkbSwapchain.extent;
        m_handle = vkbSwapchain.swapchain;

        if (oldHandle != VK_NULL_HANDLE)
        {
            m_device->handle().destroySwapchainKHR(oldHandle);
        }

        PixelFormat swapchainFormat = VkConvert::pixelFormat(m_imageFormat, m_device.get());


        const auto rawImages = vkbSwapchain.get_images().value();
        m_textures.clear();
        m_textures.reserve(rawImages.size());
        for (VkImage raw : rawImages)
        {
            grl::Rc<VkTexture> tex = grl::makeRc<VkTexture>(m_device.get(), raw, swapchainFormat, m_width, m_height);
            m_textures.push_back(tex);
        }

        const auto rawImageViews = vkbSwapchain.get_image_views().value();
        m_textureViews.clear();
        m_textureViews.reserve(rawImageViews.size());
        for (size_t i = 0; i < rawImageViews.size(); i++)
        {
            grl::Rc<VkTextureView> texView = grl::makeRc<VkTextureView>(m_device.get(), std::dynamic_pointer_cast<VkTexture>(m_textures[i]), swapchainFormat, rawImageViews[i]);
            m_textureViews.push_back(texView);
        }

        const auto queue = m_device->queue(QueueType::Graphics);
        std::scoped_lock lock(queue->mutex());
        queue->handle().waitIdle();
    }

    grl::Rc<TextureView> VkSwapchain::acquireNextImage()
    {
        const auto& frame = m_frames[m_frameIndex];

        auto result = m_device->handle().acquireNextImageKHR(
            m_handle,
            UINT64_MAX,
            frame.imageAvailableSemaphore
        );

        URHI_VALIDATE(result.has_value(), "Failed to acquire swapchain image");

        m_semaphoreConsumed = false;

        m_imageIndex = result.value;

        auto* swapTex = dynamic_cast<VkTexture*>(m_textures[m_imageIndex].get());
        swapTex->resetTrackedState(
            vk::ImageLayout::eUndefined,
            vk::PipelineStageFlagBits2::eColorAttachmentOutput, // Matches your QueueSubmit wait semaphore
            vk::AccessFlagBits2::eNone
        );

        return m_textureViews[m_imageIndex];
    }

    void VkSwapchain::present()
    {
        auto queue = m_device->queue(QueueType::Graphics);

        std::vector<vk::SemaphoreSubmitInfo> waitInfos;

        vk::SemaphoreSubmitInfo timelineWait{};
        timelineWait.semaphore = queue->timelineSemaphore();
        timelineWait.value = queue->timelineValue();
        timelineWait.stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
        waitInfos.push_back(timelineWait);

        vk::Semaphore unconsumedReady = consumeReadySemaphore();
        if (unconsumedReady)
        {
            vk::SemaphoreSubmitInfo readyWait{};
            readyWait.semaphore = unconsumedReady;
            readyWait.value = 0;
            readyWait.stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
            waitInfos.push_back(readyWait);
        }

        vk::SemaphoreSubmitInfo binarySignal{};
        binarySignal.semaphore = m_frames[m_frameIndex].renderFinishedSemaphore;
        binarySignal.value = 0;
        binarySignal.stageMask = vk::PipelineStageFlagBits2::eAllCommands;

        vk::SubmitInfo2 submitInfo{};
        submitInfo.waitSemaphoreInfoCount = static_cast<uint32_t>(waitInfos.size());
        submitInfo.pWaitSemaphoreInfos = waitInfos.data();
        submitInfo.signalSemaphoreInfoCount = 1;
        submitInfo.pSignalSemaphoreInfos = &binarySignal;

        {
            std::scoped_lock lock(queue->mutex());
            queue->handle().submit2({submitInfo});
        }

        vk::PresentInfoKHR presentInfo{};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_frames[m_frameIndex].renderFinishedSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_handle;
        presentInfo.pImageIndices = &m_imageIndex;

        const auto res = m_device->queue(QueueType::Graphics)->handle().presentKHR(presentInfo);
        URHI_VALIDATE(res == vk::Result::eSuccess, "Failed to present image");

        m_frameIndex = (m_frameIndex + 1) % m_maxFramesInFlight;
    }

    vk::Semaphore VkSwapchain::consumeReadySemaphore()
    {
        if(m_semaphoreConsumed)
            return nullptr;

        m_semaphoreConsumed = true;
        return m_frames[m_frameIndex].imageAvailableSemaphore;
    }

    vk::Semaphore VkSwapchain::renderFinishedSemaphore()
    {
        return m_frames[m_frameIndex].renderFinishedSemaphore;
    }
}

#include "vkSwapchain.h"

#include "clogr.h"
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
        m_imageFormat = VkConvert::pixelFormat(desc.format);

        const vk::SurfaceCapabilitiesKHR caps = m_device->getPhysicalDevice().getSurfaceCapabilitiesKHR(m_window->getSurface());
        m_maxFramesInFlight = caps.minImageCount;

        m_frames.resize(m_maxFramesInFlight);
        for (auto& frame : m_frames)
        {
            frame.imageAvailableSemaphore = m_device->getHandle().createSemaphore({});
            frame.transitionPool = m_device->getHandle().createCommandPool({vk::CommandPoolCreateFlagBits::eTransient, m_device->getQueueState(QueueType::Graphics)->family});
            vk::CommandBufferAllocateInfo allocInfo
            {
                frame.transitionPool,
                vk::CommandBufferLevel::ePrimary,
                1
            };
            frame.transitionCmd = m_device->getHandle().allocateCommandBuffers(allocInfo).front();
            frame.inFlightFence = m_device->getHandle().createFence({vk::FenceCreateFlagBits::eSignaled});
        }

        uint32_t width = desc.width;
        uint32_t height = desc.height;

        if(width == 0) width = m_window->getWidth();
        if(height == 0) height = m_window->getHeight();

        resize(width, height);

        for (size_t i = 0; i < m_textureViews.size(); i++)
        {
            m_renderFinishedSemaphores.push_back(m_device->getHandle().createSemaphore({}));
        }
    }

    VkSwapchain::~VkSwapchain()
    {
        m_device->waitIdle();
        for (const auto& frame : m_frames)
        {
            m_device->getHandle().destroySemaphore(frame.imageAvailableSemaphore);
            m_device->getHandle().destroyCommandPool(frame.transitionPool);
        }
    }

    void VkSwapchain::resize(const uint32_t width, const uint32_t height)
    {
        m_width = width;
        m_height = height;

        if(m_handle != nullptr)
        {
            m_device->getHandle().destroySwapchainKHR(m_handle);
        }

        vkb::SwapchainBuilder swapchainBuilder{ m_device->getPhysicalDevice(), m_device->getHandle(), m_window->getSurface() };

        vkb::Swapchain vkbSwapchain = swapchainBuilder
            //.use_default_format_selection()
            .set_desired_format(VkSurfaceFormatKHR{ .format = static_cast<VkFormat>(m_imageFormat), .colorSpace = static_cast<VkColorSpaceKHR>(m_imageFormat) })
            //use vsync present mode
            .set_desired_present_mode(static_cast<VkPresentModeKHR>(m_presentMode))
            .set_desired_extent(width, height)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .build()
            .value();

        m_extent = vkbSwapchain.extent;
        //store swapchain and its related images
        m_handle = vkbSwapchain.swapchain;

        PixelFormat swapchainFormat = VkConvert::pixelFormat(m_imageFormat);

        const auto rawImages = vkbSwapchain.get_images().value();
        m_textures.clear();
        m_textures.reserve(rawImages.size());
        for (const VkImage raw : rawImages)
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
    }

    grl::Rc<TextureView> VkSwapchain::acquireNextImage()
    {
        const auto& frame = m_frames[m_frameIndex];

        auto res = m_device->getHandle().waitForFences(
            frame.inFlightFence, VK_TRUE, UINT64_MAX
        );

        m_device->getHandle().resetFences(frame.inFlightFence);

        const auto result = m_device->getHandle().acquireNextImageKHR(
            m_handle,
            UINT64_MAX,
            frame.imageAvailableSemaphore
        );

        m_semaphoreConsumed = false;

        m_imageIndex = result.value;
        return m_textureViews[m_imageIndex];
    }

    void VkSwapchain::present()
    {
        auto& frame = m_frames[m_frameIndex];
        if(frame.maxTimelineValue > 0)
        {
            vk::SemaphoreWaitInfo waitInfo{};
            waitInfo.semaphoreCount = 1;
            waitInfo.pSemaphores = &m_device->getQueueState(QueueType::Graphics)->timeline;
            waitInfo.pValues = &frame.maxTimelineValue;
            auto res = m_device->getHandle().waitSemaphores(waitInfo, UINT64_MAX);
        }

        const auto queueState = m_device->getQueueState(QueueType::Graphics);

        m_device->getHandle().resetCommandPool(m_frames[m_frameIndex].transitionPool, {});
        const auto& cmd = m_frames[m_frameIndex].transitionCmd;
        cmd.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
        dynamic_cast<VkTexture*>(m_textures[m_imageIndex].get())->transitionLayout(cmd, vk::ImageLayout::ePresentSrcKHR);
        cmd.end();

        vk::SubmitInfo bridgeSubmit{};
        bridgeSubmit.commandBufferCount = 1;
        bridgeSubmit.pCommandBuffers = &cmd;

        // only do bridge submit if work was actually submitted this frame
        if (queueState->nextTimelineValue > 0)
        {
            const vk::Semaphore waitSemaphore = queueState->timeline;
            const vk::Semaphore signalSemaphore = m_renderFinishedSemaphores[m_imageIndex];
            constexpr vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eAllCommands;

            vk::TimelineSemaphoreSubmitInfo timelineWaitInfo{};
            timelineWaitInfo.waitSemaphoreValueCount = 1;
            timelineWaitInfo.pWaitSemaphoreValues = &queueState->nextTimelineValue;

            bridgeSubmit.pNext = &timelineWaitInfo;
            bridgeSubmit.waitSemaphoreCount = 1;
            bridgeSubmit.pWaitSemaphores = &waitSemaphore;
            bridgeSubmit.pWaitDstStageMask = &waitStage;
            bridgeSubmit.signalSemaphoreCount = 1;
            bridgeSubmit.pSignalSemaphores = &signalSemaphore;
        }

        queueState->queue.submit({bridgeSubmit}, m_frames[m_frameIndex].inFlightFence);

        vk::PresentInfoKHR presentInfo{};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_renderFinishedSemaphores[m_imageIndex];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_handle;
        presentInfo.pImageIndices = &m_imageIndex;

        auto res = queueState->queue.presentKHR(presentInfo);

        frame.maxTimelineValue = queueState->nextTimelineValue;
        m_frameIndex = (m_frameIndex + 1) % m_maxFramesInFlight;
    }

    vk::Semaphore VkSwapchain::consumeSemaphore()
    {
        if(m_semaphoreConsumed)
            return nullptr;

        m_semaphoreConsumed = true;
        return m_frames[m_frameIndex].imageAvailableSemaphore;
    }
}

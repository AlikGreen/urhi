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

        if(width == 0) width = m_window->width();
        if(height == 0) height = m_window->height();

        resize(width, height);
    }

    VkSwapchain::~VkSwapchain()
    {
        m_device->getHandle().waitIdle();
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
            .set_desired_format(VkSurfaceFormatKHR{ .format = static_cast<VkFormat>(m_imageFormat), .colorSpace = static_cast<VkColorSpaceKHR>(m_colorSpace) })
            .set_desired_present_mode(static_cast<VkPresentModeKHR>(m_presentMode))
            .set_desired_extent(width, height)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .build()
            .value();

        m_extent = vkbSwapchain.extent;
        //store swapchain and its related images
        m_handle = vkbSwapchain.swapchain;

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

        for (const auto s : m_renderFinishedSemaphores)
            m_device->getHandle().destroySemaphore(s);
        m_renderFinishedSemaphores.clear();

        m_renderFinishedSemaphores.reserve(m_textureViews.size());
        for (size_t i = 0; i < m_textureViews.size(); ++i)
            m_renderFinishedSemaphores.push_back(m_device->getHandle().createSemaphore({}));
    }

    grl::Rc<TextureView> VkSwapchain::acquireNextImage()
    {
        const auto& frame = m_frames[m_frameIndex];

        const auto res = m_device->getHandle().waitForFences(
            frame.inFlightFence, VK_TRUE, UINT64_MAX
        );
        URHI_VALIDATE(res == vk::Result::eSuccess, "Failed to wait for fence - vk::Device::waitForFences returned {}", vk::to_string(res));

        m_device->getHandle().resetFences(frame.inFlightFence);

        const auto result = m_device->getHandle().acquireNextImageKHR(
            m_handle,
            UINT64_MAX,
            frame.imageAvailableSemaphore
        );

        URHI_VALIDATE(result.has_value(), "Failed to acquire image - vk::Device::acquireNextImageKHR returned {}", vk::to_string(res));

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
            const auto res = m_device->getHandle().waitSemaphores(waitInfo, UINT64_MAX);
            URHI_VALIDATE(res == vk::Result::eSuccess, "Failed to wait on semaphore - vk::Device::waitSemaphores returned {}", vk::to_string(res));
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
        const vk::Semaphore signalSemaphore = m_renderFinishedSemaphores[m_imageIndex];
        bridgeSubmit.signalSemaphoreCount = 1;
        bridgeSubmit.pSignalSemaphores = &signalSemaphore;

        // ── move these OUT of the if-block ──
        vk::Semaphore waitSemaphore{};
        vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eAllCommands;
        vk::TimelineSemaphoreSubmitInfo timelineWaitInfo{};

        if (queueState->nextTimelineValue > 0)
        {
            waitSemaphore = queueState->timeline;

            timelineWaitInfo.waitSemaphoreValueCount = 1;
            timelineWaitInfo.pWaitSemaphoreValues    = &queueState->nextTimelineValue;

            bridgeSubmit.pNext             = &timelineWaitInfo;
            bridgeSubmit.waitSemaphoreCount = 1;
            bridgeSubmit.pWaitSemaphores   = &waitSemaphore;
            bridgeSubmit.pWaitDstStageMask = &waitStage;
        }

        queueState->queue.submit({bridgeSubmit}, m_frames[m_frameIndex].inFlightFence);

        vk::PresentInfoKHR presentInfo{};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_renderFinishedSemaphores[m_imageIndex];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_handle;
        presentInfo.pImageIndices = &m_imageIndex;

        const auto res = queueState->queue.presentKHR(presentInfo);
        URHI_VALIDATE(res == vk::Result::eSuccess, "Failed to present image - vk::Device::presentKHR returned {}", vk::to_string(res));

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

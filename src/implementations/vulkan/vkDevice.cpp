#include "vkDevice.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include <vulkan/vulkan.hpp>

#include "validation.h"
#include "vkCommandListEmitter.h"
#include "vkCommandListTracker.h"
#include "vkComputePipeline.h"
#include "vkGraphicsPipeline.h"
#include "vkMappedBuffer.h"
#include "vkTextureView.h"
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include "clogr.h"
#include "VkBootstrap.h"
#include "vkContext.h"
#include "vkPipeline.h"
#include "vkShader.h"
#include "vkWindow.h"
#include "enums/queueType.h"
#include "vkStagedBuffer.h"
#include "vkCommandList.h"
#include "vkSwapchain.h"
#include "vkTexture.h"
#include "vkSampler.h"

namespace urhi
{
    VkDevice::VkDevice(const DeviceDesc& desc, VkContext* context)
        : m_context(context)
    {
        VkPhysicalDeviceVulkan13Features features13{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
        features13.dynamicRendering = true;
        features13.synchronization2 = true;

        VkPhysicalDeviceFeatures requiredFeatures{};
        requiredFeatures.samplerAnisotropy = VK_TRUE;

        VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
        features12.bufferDeviceAddress = true;
        features12.descriptorIndexing = true;
        features12.timelineSemaphore = true;

        const auto vkWindow = dynamic_cast<VkWindow*>(desc.window.get());

        vkb::PhysicalDeviceSelector selector { context->getVkbInstance() };
        vkb::PhysicalDevice physicalDevice = selector
            .set_minimum_version(1, 3)
            .set_required_features_13(features13)
            .set_required_features_12(features12)
            .set_required_features(requiredFeatures)
            .set_surface(vkWindow->getSurface())
            .select()
            .value();


        vkb::DeviceBuilder deviceBuilder{ physicalDevice };
        vkb::Device vkbDevice = deviceBuilder
            .build()
            .value();

        m_handle = vkbDevice.device;
        m_physicalDevice = physicalDevice.physical_device;

        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(context->getVkInstance());
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_handle);

        VmaAllocatorCreateInfo allocatorCI{
            .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
            .physicalDevice = m_physicalDevice,
            .device = m_handle,
            .pVulkanFunctions = nullptr,
            .instance = context->getVkInstance()
        };
        vmaCreateAllocator(&allocatorCI, &m_allocator);


        m_commandQueues[0] = grl::makeRc<VkCommandQueue>(
            this,
            QueueType::Graphics,
            vkbDevice.get_queue_index(vkb::QueueType::graphics).value(),
            vkbDevice.get_queue(vkb::QueueType::graphics).value());

        m_commandQueues[1] = grl::makeRc<VkCommandQueue>(
            this,
            QueueType::Compute,
            vkbDevice.get_queue_index(vkb::QueueType::compute).value(),
            vkbDevice.get_queue(vkb::QueueType::compute).value());

        m_commandQueues[2] = grl::makeRc<VkCommandQueue>(
            this,
            QueueType::Transfer,
            vkbDevice.get_queue_index(vkb::QueueType::transfer).value(),
            vkbDevice.get_queue(vkb::QueueType::transfer).value());


        VkPhysicalDeviceProperties physProps;
        vkGetPhysicalDeviceProperties(m_physicalDevice, &physProps);

        m_maxAnisotropy = physProps.limits.maxSamplerAnisotropy;

        vk::FormatProperties props = m_physicalDevice.getFormatProperties(vk::Format::eD24UnormS8Uint);
        if (props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
            m_depth24PlusStencil8Format = vk::Format::eD24UnormS8Uint;

        props = m_physicalDevice.getFormatProperties(vk::Format::eD32SfloatS8Uint);
        if (props.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
            m_depth24PlusStencil8Format = vk::Format::eD32SfloatS8Uint;
    }

    VkDevice::~VkDevice()
    {
        m_handle.waitIdle();
        tryCollectGarbage();

        m_commandQueues.fill(nullptr);

        vmaDestroyAllocator(m_allocator);
        m_handle.destroy();
    }

    grl::Rc<Pipeline> VkDevice::createPipeline(const GraphicsPipelineDesc &desc)
    {
        return grl::makeRc<VkGraphicsPipeline>(this, desc);
    }

    grl::Rc<Pipeline> VkDevice::createPipeline(const ComputePipelineDesc &desc)
    {
        return grl::makeRc<VkComputePipeline>(this, desc);
    }

    grl::Rc<CommandList> VkDevice::acquireCommandList(QueueType queueType)
    {
        auto queue = m_commandQueues[static_cast<size_t>(queueType)].get();

        for(auto& cmd : m_commandLists)
        {
            if(!cmd->inUse())
            {
                cmd->reset(&queue->submissionContext(), queue);
                return cmd;
            }
        }

        URHI_WARNING(m_commandLists.size() > 128, "Lots of command lists allocated ({})", m_commandLists.size());

        const auto cmd = grl::makeRc<VkCommandList>(this, queue, &queue->submissionContext());
        m_commandLists.push_back(cmd);

        return m_commandLists.back();
    }

    grl::Rc<Texture> VkDevice::createTexture(const TextureDesc &desc)
    {
        return grl::makeRc<VkTexture>(this, desc);
    }

    grl::Rc<Sampler> VkDevice::createSampler(const SamplerDesc &desc)
    {
        return grl::makeRc<VkSampler>(this, desc);
    }

    grl::Rc<TextureView> VkDevice::createTextureView(const TextureViewDesc &desc)
    {
        return grl::makeRc<VkTextureView>(this, desc);
    }

    grl::Rc<Shader> VkDevice::createShader(const ShaderEntryPoint &entryPoint)
    {
        return grl::makeRc<VkShader>(this, entryPoint);
    }

    grl::Rc<Buffer> VkDevice::createBuffer(const BufferDesc &desc)
    {
        URHI_VALIDATE(desc.size != 0, "Invalid buffer size ({}) - buffer size must be greater than 0 and less than vram available", desc.size);
        URHI_VALIDATE(desc.usage != BufferUsage::None, "Invalid buffer usage - buffer usage must not be BufferUsage::None");
        URHI_VALIDATE(!(hasFlag(desc.usage, BufferUsage::Static) && hasFlag(desc.usage, BufferUsage::Dynaimic)), "Invalid buffer usage - buffer usage cannot have BufferUsage::Dynamic and BufferUsage::Static");

        if ((hasFlag(desc.usage, BufferUsage::Uniform) && !hasFlag(desc.usage, BufferUsage::Static)) || hasFlag(desc.usage, BufferUsage::Dynaimic))
           return grl::makeRc<VkMappedBuffer>(this, desc);

        return grl::makeRc<VkStagedBuffer>(this, desc);
    }

    void VkDevice::waitIdle()
    {
        m_handle.waitIdle();
    }

    void VkDevice::submit(const grl::Rc<CommandList> &cmdList)
    {
        const auto vkCmd = dynamic_cast<VkCommandList*>(cmdList.get());
        vkCmd->submit();

        const auto commandQueue = vkCmd->queue();
        const vk::CommandBuffer cmdBuffer = commandQueue->acquireCommandBuffer();

        auto commandStream = vkCmd->commands();
        VkCommandListTracker tracker;
        for (auto& cmd : commandStream->commands())
            std::visit([&](auto& c) { tracker.record(c); }, cmd);

        const uint64_t submitValue = commandQueue->timelineValue() + 1;

        VkCommandListEmitter emitter{ this, tracker, cmdBuffer, commandQueue, submitValue, commandStream };
        for (auto& cmd : commandStream->commands())
            std::visit([&](auto& c) { emitter.emit(c); }, cmd);

        emitter.endRecording();

        vk::Semaphore waitSwapchainSemaphore = nullptr;
        if(commandQueue->type() == QueueType::Graphics)
            waitSwapchainSemaphore = m_context->getSwapchain()->consumeReadySemaphore();

        commandQueue->submit(cmdBuffer, vkCmd->submissionContext(), waitSwapchainSemaphore);

        tryCollectGarbage();
    }

    void VkDevice::queueDestroy(VkLifetime lifetime, const std::function<void(VkDevice* device)>& callback)
    {
        m_destroyQueue.emplace_back(lifetime, callback);
    }

    void VkDevice::tryCollectGarbage()
    {
        if(m_destroyQueue.empty()) return;

        for (size_t i = 0; i < m_destroyQueue.size(); i++ )
        {
            auto& [lifetime, callback] = m_destroyQueue[i];

            if(!lifetime.lastSubmitQueue)
            {
                callback(this);
                m_destroyQueue[i] = std::move(m_destroyQueue.back());
                m_destroyQueue.pop_back();
                i--;
            }else
            {
                const uint64_t completeValue = m_handle.getSemaphoreCounterValue(lifetime.lastSubmitQueue->timelineSemaphore());

                if(lifetime.lastSubmitValue <= completeValue)
                {
                    callback(this);
                    m_destroyQueue[i] = std::move(m_destroyQueue.back());
                    m_destroyQueue.pop_back();
                    i--;
                }
            }
        }

        URHI_WARNING(m_destroyQueue.size() <= 512, "Too many resource destroys queued - {} destroys queued, you may have a memory leak", m_destroyQueue.size());
    }

    clogr::Logger & VkDevice::logger() const
    {
        return m_context->logger();
    }

    grl::Rc<VkCommandQueue> VkDevice::queue(QueueType type) const
    {
        return m_commandQueues[static_cast<size_t>(type)];
    }
}

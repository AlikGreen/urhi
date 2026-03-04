#include "vkDevice.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include <vulkan/vulkan.hpp>

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
            .add_required_extension(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME)
            .set_minimum_version(1, 3)
            .set_required_features_13(features13)
            .set_required_features_12(features12)
            .set_required_features(requiredFeatures)
            .set_surface(vkWindow->getSurface())
            .select()
            .value();


        vkb::DeviceBuilder deviceBuilder { physicalDevice };
        vkb::Device vkbDevice = deviceBuilder.build().value();

        m_handle = vkbDevice.device;
        m_physicalDevice = physicalDevice.physical_device;

        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(context->getVkInstance());
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_handle);

        vk::SemaphoreTypeCreateInfo typeCreateInfo
        {
            vk::SemaphoreType::eTimeline,
            0
        };

        m_queueStates[0] = grl::makeRc<VkQueueState>();
        m_queueStates[0]->queue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
        m_queueStates[0]->family = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
        m_queueStates[0]->mutex = grl::makeBox<std::mutex>();
        m_queueStates[0]->timeline = m_handle.createSemaphore({{}, typeCreateInfo});

        m_queueStates[1] = grl::makeRc<VkQueueState>();
        m_queueStates[1]->queue = vkbDevice.get_queue(vkb::QueueType::compute).value();
        m_queueStates[1]->family = vkbDevice.get_queue_index(vkb::QueueType::compute).value();
        m_queueStates[1]->mutex = grl::makeBox<std::mutex>();
        m_queueStates[1]->timeline = m_handle.createSemaphore({{}, typeCreateInfo});

        m_queueStates[2] = grl::makeRc<VkQueueState>();
        m_queueStates[2]->queue = vkbDevice.get_queue(vkb::QueueType::transfer).value();
        m_queueStates[2]->family = vkbDevice.get_queue_index(vkb::QueueType::transfer).value();
        m_queueStates[2]->mutex = grl::makeBox<std::mutex>();
        m_queueStates[2]->timeline = m_handle.createSemaphore({{}, typeCreateInfo});

        VmaVulkanFunctions vkFunctions{
            .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
            .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
            .vkCreateImage = vkCreateImage
        };
        VmaAllocatorCreateInfo allocatorCI{
            .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
            .physicalDevice = m_physicalDevice,
            .device = m_handle,
            .pVulkanFunctions = &vkFunctions,
            .instance = context->getVkInstance()
        };
        vmaCreateAllocator(&allocatorCI, &m_allocator);

        VkPhysicalDeviceProperties physProps;
        vkGetPhysicalDeviceProperties(m_physicalDevice, &physProps);

        m_maxAnisotropy = physProps.limits.maxSamplerAnisotropy;
    }

    VkDevice::~VkDevice()
    {
        m_handle.waitIdle();
        tryCollectGarbage();
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
        return grl::makeRc<VkCommandList>(queueType);
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
       if (desc.usage == BufferUsage::Uniform && desc.size < 1024 * 8) // if < 8 MB use persistent mapped buffer
           return grl::makeRc<VkMappedBuffer>(this, desc);

        return grl::makeRc<VkStagedBuffer>(this, desc);
    }

    void VkDevice::submit(const grl::Rc<CommandList> &cmdList)
    {
        const auto vkCmd = dynamic_cast<VkCommandList*>(cmdList.get());

        auto& pool = m_commandListPools[m_cmdListIndex][static_cast<size_t>(vkCmd->m_queueType)];

        if(!pool)
        {
            pool = grl::makeRc<VkCommandListPool>(
                this,
                m_queueStates[static_cast<size_t>(vkCmd->m_queueType)]
            );
        }

        const vk::CommandBuffer commandBuffer = pool->acquire();

        VkCommandListTracker tracker;
        for (auto& cmd : vkCmd->m_commands)
            std::visit([&](auto& c) { tracker.record(c); }, cmd);

        const uint64_t submitValue = pool->m_queueState->nextTimelineValue + 1;

        VkCommandListEmitter emitter{ this, vkCmd->m_queueType, submitValue, pool->m_queueState->timeline, commandBuffer, tracker, pool->m_linearStagingAllocator };
        for (auto& cmd : vkCmd->m_commands)
            std::visit([&](auto& c) { emitter.emit(c); }, cmd);


        commandBuffer.end();

        vk::Semaphore semaphore = nullptr;

        if(vkCmd->m_queueType == QueueType::Graphics)
            semaphore = m_context->getSwapchain()->consumeSemaphore();

        pool->submit(commandBuffer, semaphore);

        m_cmdListIndex = ++m_cmdListIndex % CMD_POOLS_PER_QUEUE;

        tryCollectGarbage();
    }

    void VkDevice::queueDestroy(VkLifetime lifetime, std::function<void(vk::Device device)> callback)
    {
        m_destroyQueue.emplace_back(lifetime, callback);
    }

    vk::PhysicalDevice VkDevice::getPhysicalDevice() const
    {
        return m_physicalDevice;
    }

    vk::Device VkDevice::getHandle() const
    {
        return m_handle;
    }

    VmaAllocator VkDevice::getAllocator() const
    {
        return m_allocator;
    }

    grl::Rc<VkQueueState> VkDevice::getQueueState(QueueType queueType)
    {
        return m_queueStates[static_cast<size_t>(queueType)];
    }
    
    void VkDevice::tryCollectGarbage()
    {
        if(m_destroyQueue.size() < 1) return;

        uint64_t completeValues[3];

        for (size_t i = 0; i < m_destroyQueue.size(); i++ )
        {
            auto& [lifetime, callback] = m_destroyQueue[i];

            uint64_t completeValue = completeValues[static_cast<uint8_t>(lifetime.lastSubmitQueue)];
            if(completeValue == 0)
            {
                completeValue = m_handle.getSemaphoreCounterValue(m_queueStates[i]->timeline);
                completeValues[static_cast<uint8_t>(lifetime.lastSubmitQueue)] = completeValue;
            }

            if(lifetime.lastSubmitValue <= completeValue)
            {
                callback(m_handle);
                m_destroyQueue[i] = std::move(m_destroyQueue.back());
                m_destroyQueue.pop_back();
                i--;
            }
        }
    }

    float VkDevice::getMaxAnisotropy() const
    {
        return m_maxAnisotropy;
    }

    clogr::Logger& VkDevice::logger() const
    {
        return m_context->logger();
    }
}

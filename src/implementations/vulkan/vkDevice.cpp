#include "vkDevice.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "clogr.h"
#include "VkBootstrap.h"
#include "vkContext.h"
#include "vkPipeline.h"
#include "vkShader.h"
#include "vkWindow.h"
#include "enums/queueType.h"
#include "vkStagedBuffer.h"
#include "vkTexture.h"

namespace urhi
{
    VkDevice::VkDevice(const DeviceDesc& desc, VkContext* context)
    {
        VkPhysicalDeviceVulkan13Features features13{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
        features13.dynamicRendering = true;
        features13.synchronization2 = true;

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
            .set_surface(vkWindow->getSurface())
            .select()
            .value();

        vkb::DeviceBuilder deviceBuilder { physicalDevice };
        vkb::Device vkbDevice = deviceBuilder.build().value();

        m_handle = vkbDevice.device;
        m_physicalDevice = physicalDevice.physical_device;

        m_queueStates[0] = grl::makeRc<VkQueueState>();
        m_queueStates[0]->queue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
        m_queueStates[0]->family = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
        m_queueStates[0]->mutex = grl::makeBox<std::mutex>();

        m_queueStates[1] = grl::makeRc<VkQueueState>();
        m_queueStates[1]->queue = vkbDevice.get_queue(vkb::QueueType::compute).value();
        m_queueStates[1]->family = vkbDevice.get_queue_index(vkb::QueueType::compute).value();
        m_queueStates[1]->mutex = grl::makeBox<std::mutex>();

        m_queueStates[2] = grl::makeRc<VkQueueState>();
        m_queueStates[2]->queue = vkbDevice.get_queue(vkb::QueueType::transfer).value();
        m_queueStates[2]->family = vkbDevice.get_queue_index(vkb::QueueType::transfer).value();
        m_queueStates[2]->mutex = grl::makeBox<std::mutex>();

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

        m_stagingBufferPool = grl::makeRc<VkStagingBufferPool>(this);
    }

    grl::Rc<Pipeline> VkDevice::createPipeline(const GraphicsPipelineDesc &desc)
    {
        return grl::makeRc<VkPipeline>(this, desc);
    }

    grl::Rc<Pipeline> VkDevice::createPipeline(const ComputePipelineDesc &desc)
    {
        clogr::abort("not implemented");
    }

    grl::Rc<CommandList> VkDevice::acquireCommandList(QueueType queueType)
    {
        auto& pool = m_commandListPools[m_cmdListIndex][static_cast<size_t>(queueType)];

        if(!pool)
        {
            pool = grl::makeBox<VkCommandListPool>(
                this,
                m_queueStates[static_cast<size_t>(queueType)]
            );
        }

        return grl::makeRc<VkCommandList>(this, pool.get(), queueType, pool->acquire());
    }

    grl::Rc<Texture> VkDevice::createTexture(const TextureDesc &desc)
    {
        return grl::makeRc<VkTexture>(this, desc);
    }

    grl::Rc<Sampler> VkDevice::createSampler(const SamplerDesc &desc)
    {
        clogr::abort("not implemented");
    }

    grl::Rc<TextureView> VkDevice::createTextureView(const TextureViewDesc &desc)
    {
        clogr::abort("not implemented");
    }

    grl::Rc<Shader> VkDevice::createShader(SpirvShader shader)
    {
        return grl::makeRc<VkShader>(this, shader);
    }

    grl::Rc<Buffer> VkDevice::createBuffer(const BufferDesc &desc)
    {
        // if (desc.usage == BufferUsage::Uniform && desc.size < 64 * 1024)
        //     return grl::makeRc<VkPersistentMappedBuffer>(this, desc);

        return grl::makeRc<VkStagedBuffer>(this, desc);
    }

    void VkDevice::submit(const grl::Rc<CommandList> &cmd)
    {
        const auto vkCmd = dynamic_cast<VkCommandList*>(cmd.get());
        const vk::CommandBuffer commandBuffer = vkCmd->getCmdBuffer();
        commandBuffer.end();

        vkCmd->getPool().submit(vkCmd);

        m_cmdListIndex = ++m_cmdListIndex % CMD_POOLS_PER_QUEUE;
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

    grl::Rc<VkStagingBufferPool> VkDevice::getStagingBufferPool() const
    {
        return m_stagingBufferPool;
    }

    grl::Rc<VkQueueState> VkDevice::getQueueState(QueueType queueType)
    {
        return m_queueStates[static_cast<size_t>(queueType)];
    }
}

#include "vkDevice.h"

#include "clogr.h"
#include "VkBootstrap.h"
#include "vkContext.h"
#include "vkWindow.h"
#include "enums/queueType.h"

namespace urhi
{
    thread_local std::vector<std::vector<grl::Rc<VkCommandListPool>>> VkDevice::m_commandListPools = {};

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

        m_queueStates.resize(3);

        m_queueStates[0].queue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
        m_queueStates[0].family = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
        m_queueStates[0].mutex = grl::makeBox<std::mutex>();

        m_queueStates[1].queue = vkbDevice.get_queue(vkb::QueueType::compute).value();
        m_queueStates[1].family = vkbDevice.get_queue_index(vkb::QueueType::compute).value();
        m_queueStates[1].mutex = grl::makeBox<std::mutex>();

        m_queueStates[2].queue = vkbDevice.get_queue(vkb::QueueType::transfer).value();
        m_queueStates[2].family = vkbDevice.get_queue_index(vkb::QueueType::transfer).value();
        m_queueStates[2].mutex = grl::makeBox<std::mutex>();
    }

    grl::Rc<Pipeline> VkDevice::createPipeline(const GraphicsPipelineDesc &desc)
    {
        clogr::ensure(false, "not implemented");
    }

    grl::Rc<Pipeline> VkDevice::createPipeline(const ComputePipelineDesc &desc)
    {
        clogr::ensure(false, "not implemented");
    }

    grl::Rc<CommandList> VkDevice::acquireCommandList(QueueType queueType)
    {
        if(m_commandListPools.size() < 3)
            m_commandListPools.resize(3);

        auto& queuePools = m_commandListPools.at(static_cast<size_t>(queueType));

        if(queuePools.size() < m_currentFrameIndex+1)
            queuePools.resize(m_currentFrameIndex+1);

        if(queuePools[m_currentFrameIndex] == nullptr)
            queuePools[m_currentFrameIndex] = grl::makeBox<VkCommandListPool>(m_queueStates[static_cast<size_t>(queueType)].family, this);


        auto pool = queuePools[m_currentFrameIndex];
        return grl::makeRc<VkCommandList>(this, pool, queueType, pool->acquire());
    }

    grl::Rc<Texture> VkDevice::createTexture(const TextureDesc &description)
    {
        clogr::ensure(false, "not implemented");
    }

    grl::Rc<Sampler> VkDevice::createSampler(const SamplerDesc &description)
    {
        clogr::ensure(false, "not implemented");
    }

    grl::Rc<TextureView> VkDevice::createTextureView(const TextureViewDesc &desc)
    {
        clogr::ensure(false, "not implemented");
    }

    grl::Rc<Shader> VkDevice::createShader(CompiledShader shader)
    {
        clogr::ensure(false, "not implemented");
    }

    grl::Rc<Buffer> VkDevice::createIndexBuffer()
    {
        clogr::ensure(false, "not implemented");
    }

    grl::Rc<Buffer> VkDevice::createVertexBuffer()
    {
        clogr::ensure(false, "not implemented");
    }

    grl::Rc<Buffer> VkDevice::createUniformBuffer()
    {
        clogr::ensure(false, "not implemented");
    }

    grl::Rc<Buffer> VkDevice::createStorageBuffer()
    {
        clogr::ensure(false, "not implemented");
    }

    void VkDevice::submit(const grl::Rc<CommandList> &cmd)
    {
        const auto vkCmd = dynamic_cast<VkCommandList*>(cmd.get());
        const vk::CommandBuffer commandBuffer = vkCmd->getHandle();
        commandBuffer.end();

        vkCmd->getPool().submit(vkCmd);
    }

    vk::PhysicalDevice VkDevice::getPhysicalDevice() const
    {
        return m_physicalDevice;
    }

    vk::Device VkDevice::getHandle() const
    {
        return m_handle;
    }

    vk::Queue VkDevice::getQueue(QueueType queueType) const
    {
        return m_queueStates[static_cast<size_t>(queueType)].queue;
    }

    std::mutex& VkDevice::getQueueMutex(QueueType queueType) const
    {
        return *m_queueStates[static_cast<size_t>(queueType)].mutex;
    }
}

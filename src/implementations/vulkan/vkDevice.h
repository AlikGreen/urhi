#pragma once
#include <vulkan/vulkan.hpp>

#include "device.h"
#include "vkCommandListPool.h"
#include "descriptions/deviceDesc.h"
#include "enums/queueType.h"

namespace urhi
{
class VkContext;

class VkDevice final : public Device
{
public:
    explicit VkDevice(const DeviceDesc& desc, VkContext* context);
    grl::Rc<Pipeline> createPipeline(const GraphicsPipelineDesc &desc) override;
    grl::Rc<Pipeline> createPipeline(const ComputePipelineDesc &desc) override;

    grl::Rc<CommandList> acquireCommandList(QueueType queueType) override;

    grl::Rc<Texture> createTexture(const TextureDesc &description) override;
    grl::Rc<Sampler> createSampler(const SamplerDesc &description) override;
    grl::Rc<TextureView> createTextureView(const TextureViewDesc &desc) override;

    grl::Rc<Shader> createShader(CompiledShader shader) override;

    grl::Rc<Buffer> createIndexBuffer() override;
    grl::Rc<Buffer> createVertexBuffer() override;
    grl::Rc<Buffer> createUniformBuffer() override;
    grl::Rc<Buffer> createStorageBuffer() override;

    void submit(const grl::Rc<CommandList> &cmd) override;

    [[nodiscard]] vk::PhysicalDevice getPhysicalDevice() const;
    [[nodiscard]] vk::Device getHandle() const;

    [[nodiscard]] vk::Queue getQueue(QueueType queueType) const;
    [[nodiscard]] std::mutex& getQueueMutex(QueueType queueType) const;
private:
    friend class VkSwapchain;

    static thread_local std::vector<std::vector<grl::Rc<VkCommandListPool>>> m_commandListPools;
    uint32_t m_currentFrameIndex = 0;

    vk::PhysicalDevice m_physicalDevice;
    vk::Device m_handle;

    struct QueueState
    {
        QueueState() = default;
        vk::Queue queue = VK_NULL_HANDLE;
        uint32_t family = 0;
        grl::Box<std::mutex> mutex{};
    };

    std::vector<QueueState> m_queueStates{};
};
}

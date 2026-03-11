#pragma once
#include <vulkan/vulkan.hpp>
#include <mutex>
#include <atomic>
#include <vma/vk_mem_alloc.h>

#include "device.h"
#include "logger.h"
#include "descriptions/deviceDesc.h"
#include "descriptions/shaderEntryPoint.h"
#include "enums/queueType.h"
#include "vkCommandListPool.h"

namespace urhi
{
    class VkCommandListPool;
    class VkContext;

struct VkQueueState
{
    VkQueueState() = default;
    vk::Queue queue = VK_NULL_HANDLE;
    uint32_t family = 0;
    grl::Box<std::mutex> mutex{};
    vk::Semaphore timeline;
    uint64_t nextTimelineValue{};
};

class VkDevice final : public Device
{
public:
    explicit VkDevice(const DeviceDesc& desc, VkContext* context);
    ~VkDevice() override;

    grl::Rc<Pipeline> createPipeline(const GraphicsPipelineDesc &desc) override;
    grl::Rc<Pipeline> createPipeline(const ComputePipelineDesc &desc) override;

    grl::Rc<CommandList> acquireCommandList(QueueType queueType) override;

    grl::Rc<Texture> createTexture(const TextureDesc &desc) override;
    grl::Rc<Sampler> createSampler(const SamplerDesc &desc) override;
    grl::Rc<TextureView> createTextureView(const TextureViewDesc &desc) override;

    grl::Rc<Shader> createShader(const ShaderEntryPoint& entryPoint) override;

    grl::Rc<Buffer> createBuffer(const BufferDesc& desc) override;
    void submit(const grl::Rc<CommandList> &cmdList) override;

    void queueDestroy(VkLifetime lifetime, const std::function<void(vk::Device device)> &callback);

    [[nodiscard]] vk::PhysicalDevice getPhysicalDevice() const;
    [[nodiscard]] vk::Device getHandle() const;
    [[nodiscard]] VmaAllocator getAllocator() const;
    [[nodiscard]] float getMaxAnisotropy() const;

    clogr::Logger& logger() const;
    grl::Rc<VkQueueState> getQueueState(QueueType queueType);

    vk::Format depth24PlusStencil8Format() const;
private:
    friend class VkSwapchain;

    static constexpr size_t CMD_POOLS_PER_QUEUE = 4;
    static constexpr size_t QUEUE_TYPES = 3;

    void tryCollectGarbage();

    VkContext* m_context;
    std::atomic<uint32_t> m_cmdListIndex;

    vk::PhysicalDevice m_physicalDevice;
    vk::Device m_handle;
    VmaAllocator m_allocator{};

    std::array<std::array<grl::Rc<VkCommandListPool>, QUEUE_TYPES>, CMD_POOLS_PER_QUEUE> m_commandListPools; // 4 command list pools per queue (so they get a chance to be reset)

    std::array<grl::Rc<VkQueueState>, 3> m_queueStates{};

    std::vector<std::pair<VkLifetime, std::function<void(vk::Device device)>>> m_destroyQueue{};

    float m_maxAnisotropy = 0.0f;
    vk::Format m_depth24PlusStencil8Format;
};
}

#pragma once
#include <vulkan/vulkan.hpp>
#include <mutex>
#include <vma/vk_mem_alloc.h>

#include "device.h"
#include "logger.h"
#include "vkCommandQueue.h"
#include "vkLifetime.h"
#include "descriptions/deviceDesc.h"
#include "enums/queueType.h"

namespace urhi
{
    class VkCommandListPool;
    class VkContext;

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

    grl::Rc<Buffer> createBuffer(const BufferDesc& desc) override;
    grl::Rc<Shader> createShader(const ShaderEntryPoint &entryPoint) override;

    void waitIdle() override;

    void submit(const grl::Rc<CommandList> &cmdList) override;

    void queueDestroy(VkLifetime lifetime, const std::function<void(VkDevice* device)> &callback);

    [[nodiscard]] vk::Device handle() const { return m_handle; }
    [[nodiscard]] vk::PhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }
    [[nodiscard]] VmaAllocator allocator() const { return m_allocator; }

    [[nodiscard]] float maxAnisotropy() const { return m_maxAnisotropy; }
    [[nodiscard]] vk::Format depth24PlusStencil8Format() const { return m_depth24PlusStencil8Format; }

    [[nodiscard]] clogr::Logger& logger() const;

    [[nodiscard]] grl::Rc<VkCommandQueue> queue(QueueType type) const;

private:
    friend class VkSwapchain;
    static constexpr uint32_t kQueueTypes = 3;

    void tryCollectGarbage();

    VkContext* m_context;

    vk::PhysicalDevice m_physicalDevice;
    vk::Device m_handle;
    VmaAllocator m_allocator{};

    std::array<grl::Rc<VkCommandQueue>, kQueueTypes> m_commandQueues{};

    std::vector<std::pair<VkLifetime, std::function<void(VkDevice* device)>>> m_destroyQueue{};

    std::vector<grl::Rc<VkCommandList>> m_commandLists{};

    float m_maxAnisotropy = 0.0f;
    vk::Format m_depth24PlusStencil8Format;
};
}

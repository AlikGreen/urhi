#pragma once
#include "commandList.h"
#include <vulkan/vulkan.hpp>

#include "VkReadbackRequest.h"
#include "enums/queueType.h"

namespace urhi
{
class VkDevice;
class VkCommandListPool;
class VkCommandList final : public CommandList
{
public:
    VkCommandList(VkDevice* device, VkCommandListPool* pool, QueueType queueType, vk::CommandBuffer commandBuffer);
    void begin() override;

    grl::Rc<RenderPass> beginRenderPass(const RenderPassDesc &desc) override;
    grl::Rc<ComputePass> beginComputePass() override;

    void updateTexture(const TextureUploadDesc &desc) override;
    void generateMipmaps(const grl::Rc<Texture> &texture) override;

    grl::Rc<ReadbackRequest> readback(const TextureReadbackDesc& desc) override;

    void onSubmit(uint64_t submittedValue);

    [[nodiscard]] vk::CommandBuffer getCmdBuffer() const;
    [[nodiscard]] QueueType getQueueType() const;

    [[nodiscard]] VkCommandListPool& getPool() const;
protected:
    void updateBufferImpl(const grl::Rc<Buffer> &buffer, void *data, uint32_t size) override;
private:
    friend class VkCommandListPool;

    VkDevice* m_device;

    vk::CommandBuffer m_cmd;
    QueueType m_queueType;
    VkCommandListPool* m_pool;

    std::vector<grl::Rc<VkReadbackRequest>> m_readbackRequests{};
};
}


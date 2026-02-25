#pragma once
#include "commandList.h"
#include <vulkan/vulkan.hpp>

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
    void updateTexture(const grl::Rc<Texture> &texture, const TextureUploadDesc &desc) override;
    void generateMipmaps(const grl::Rc<Texture> &texture) override;

    [[nodiscard]] vk::CommandBuffer getCmdBuffer() const;
    [[nodiscard]] QueueType getQueueType() const;

    [[nodiscard]] VkCommandListPool& getPool() const;
protected:
    void updateBufferImpl(const grl::Rc<Buffer> &buffer, void *data, uint32_t size) override;
    void readTextureImpl(const grl::Rc<TextureView> &texture, const TextureReadDesc &desc, size_t destSize, void *dest) override;
private:
    friend class VkCommandListPool;

    vk::CommandBuffer m_commandBuffer;
    QueueType m_queueType;
    VkCommandListPool* m_pool;
    VkDevice* m_device;
};
}


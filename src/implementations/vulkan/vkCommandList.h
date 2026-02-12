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
    explicit VkCommandList(VkDevice* device, const grl::Rc<VkCommandListPool> &pool, QueueType queueType, vk::CommandBuffer commandBuffer);
    void begin() override;

    void setUniformBuffer(const std::string &name, const grl::Rc<Buffer> &buffer) override;
    void setStorageBuffer(const std::string &name, const grl::Rc<Buffer> &buffer) override;
    void setTexture(const std::string &name, const grl::Rc<TextureView> &texture) override;
    void setSampler(const std::string &name, const grl::Rc<Sampler> &sampler) override;
    void setImage(const std::string &name, const grl::Rc<TextureView> &texture, ImageAccess access) override;
    void setPipeline(const grl::Rc<Pipeline> &pipeline) override;

    void beginRenderPass(const RenderPassDesc &desc) override;
    void endRenderPass() override;

    void setVertexBuffer(uint32_t index, const grl::Rc<Buffer> &vertexBuffer) override;
    void setIndexBuffer(const grl::Rc<Buffer> &indexBuffer, IndexFormat indexFormat) override;

    void setScissor(ScissorRect rect) override;

    void updateTexture(const grl::Rc<Texture> &texture, const TextureUploadDesc &desc) override;
    void generateMipmaps(const grl::Rc<Texture> &texture) override;
    void reserveBuffer(const grl::Rc<Buffer> &buffer, size_t size) override;

    void dispatch(const glm::ivec3 &numGroups) override;

    void resourceBarrier(const grl::Rc<Texture> &texture, ImageAccess nextAccess) override;
    void resourceBarrier(const grl::Rc<Buffer> &buffer) override;

    [[nodiscard]] vk::CommandBuffer getHandle() const;
    [[nodiscard]] QueueType getQueueType() const;

    [[nodiscard]] VkCommandListPool& getPool() const;
protected:
    void drawImpl(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
    void drawIndexedImpl(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int vertexOffset, uint32_t firstInstance) override;

    void updateBufferImpl(const grl::Rc<Buffer> &buffer, void *data, uint32_t size) override;
    void readTextureImpl(const grl::Rc<TextureView> &texture, const TextureReadDesc &desc, size_t destSize, void *dest) override;
private:
    vk::CommandBuffer m_commandBuffer;
    QueueType m_queueType;
    grl::Rc<VkCommandListPool> m_pool;
    VkDevice* m_device;
};
}


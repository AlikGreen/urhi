#include "vkCommandList.h"

#include <utility>

#include "clogr.h"
#include "vkDevice.h"

namespace urhi
{
    VkCommandList::VkCommandList(VkDevice* device, const grl::Rc<VkCommandListPool> &pool, const QueueType queueType, const vk::CommandBuffer commandBuffer)
        : m_commandBuffer(commandBuffer),  m_queueType(queueType), m_pool(std::move(pool)), m_device(device)
    {

    }

    void VkCommandList::begin()
    {
        constexpr vk::CommandBufferBeginInfo info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        m_commandBuffer.begin(info);
        m_pool->m_recordingCount++;
    }

    void VkCommandList::setUniformBuffer(const std::string &name, const grl::Rc<Buffer> &buffer)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::setStorageBuffer(const std::string &name, const grl::Rc<Buffer> &buffer)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::setTexture(const std::string &name, const grl::Rc<TextureView> &texture)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::setSampler(const std::string &name, const grl::Rc<Sampler> &sampler)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::setImage(const std::string &name, const grl::Rc<TextureView> &texture, ImageAccess access)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::setPipeline(const grl::Rc<Pipeline> &pipeline)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::beginRenderPass(const RenderPassDesc &desc)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::endRenderPass()
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::setVertexBuffer(uint32_t index, const grl::Rc<Buffer> &vertexBuffer)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::setIndexBuffer(const grl::Rc<Buffer> &indexBuffer, IndexFormat indexFormat)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::setScissor(ScissorRect rect)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::updateTexture(const grl::Rc<Texture> &texture, const TextureUploadDesc &desc)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::generateMipmaps(const grl::Rc<Texture> &texture)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::reserveBuffer(const grl::Rc<Buffer> &buffer, size_t size)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::dispatch(const glm::ivec3 &numGroups)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::resourceBarrier(const grl::Rc<Texture> &texture, ImageAccess nextAccess)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::resourceBarrier(const grl::Rc<Buffer> &buffer)
    {
        clogr::ensure(false, "not implemented");
    }

    vk::CommandBuffer VkCommandList::getHandle() const
    {
        return m_commandBuffer;
    }

    QueueType VkCommandList::getQueueType() const
    {
        return m_queueType;
    }

    VkCommandListPool& VkCommandList::getPool() const
    {
        return *m_pool;
    }

    void VkCommandList::drawImpl(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::drawIndexedImpl(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int vertexOffset, uint32_t firstInstance)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::updateBufferImpl(const grl::Rc<Buffer> &buffer, void *data, uint32_t size)
    {
        clogr::ensure(false, "not implemented");
    }

    void VkCommandList::readTextureImpl(const grl::Rc<TextureView> &texture, const TextureReadDesc &desc, size_t destSize, void *dest)
    {
        clogr::ensure(false, "not implemented");
    }
}

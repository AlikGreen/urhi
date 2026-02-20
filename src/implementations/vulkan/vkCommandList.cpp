#include "vkCommandList.h"

#include <utility>

#include "clogr.h"
#include "renderPass.h"
#include "vkDevice.h"
#include "vkStagedBuffer.h"
#include "vkRenderPass.h"

namespace urhi
{
    VkCommandList::VkCommandList(VkDevice* device, VkCommandListPool* pool, const QueueType queueType, const vk::CommandBuffer commandBuffer)
        : m_commandBuffer(commandBuffer),  m_queueType(queueType), m_pool(pool), m_device(device)
    {

    }

    void VkCommandList::begin()
    {
        constexpr vk::CommandBufferBeginInfo info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        m_commandBuffer.begin(info);
        m_pool->m_recordingCount++;
    }

    grl::Rc<RenderPass> VkCommandList::beginRenderPass(const RenderPassDesc &desc)
    {
        return grl::makeRc<VkRenderPass>(m_device, m_commandBuffer, desc);
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

    vk::CommandBuffer VkCommandList::getCmdBuffer() const
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

    void VkCommandList::updateBufferImpl(const grl::Rc<Buffer> &buffer, void *data, const uint32_t size)
    {
        if(const auto vkStaged = dynamic_cast<VkStagedBuffer*>(buffer.get()))
        {
            m_pool->m_linearStagingAllocator->upload(data, size, vkStaged->getHandle(), 0, m_commandBuffer);
        }
    }

    void VkCommandList::readTextureImpl(const grl::Rc<TextureView> &texture, const TextureReadDesc &desc, size_t destSize, void *dest)
    {
        clogr::ensure(false, "not implemented");
    }
}

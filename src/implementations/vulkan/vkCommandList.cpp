#include "vkCommandList.h"

#include <utility>

#include "clogr.h"
#include "renderPass.h"
#include "vkCommandListPool.h"
#include "vkComputePass.h"
#include "vkConvert.h"
#include "vkDevice.h"
#include "vkMappedBuffer.h"
#include "vkStagedBuffer.h"
#include "vkRenderPass.h"

namespace urhi
{
    VkCommandList::VkCommandList(const QueueType queueType)
        : m_queueType(queueType)
    {
    }

    void VkCommandList::begin()
    {
        m_readbackRequests.clear();

        m_commands.clear();

        m_commands.push_back(CmdBeginCommandBuffer{});
    }

    grl::Rc<RenderPass> VkCommandList::beginRenderPass(const RenderPassDesc &desc)
    {
        return grl::makeRc<VkRenderPass>(m_commands, desc);
    }

    grl::Rc<ComputePass> VkCommandList::beginComputePass()
    {
        return grl::makeRc<VkComputePass>(m_commands);
    }

    void VkCommandList::updateTexture(const TextureUploadDesc &desc)
    {
        const uint32_t size = desc.width * desc.height * desc.depth * VkConvert::pixelFormatBytes(desc.texture->format());

        std::vector<uint8_t> vec{};
        vec.resize(size);
        std::memcpy(vec.data(), desc.data, size);

        m_commands.push_back(CmdUpdateTexture{desc, vec});
    }

    void VkCommandList::generateMipmaps(const grl::Rc<Texture> &texture)
    {
        m_commands.push_back(CmdGenerateMips{texture});
    }

    void VkCommandList::blitTexture(const BlitTextureDesc &desc)
    {
        m_commands.push_back(CmdBlitTexture{desc});
    }

    grl::Rc<ReadbackRequest> VkCommandList::readback(const TextureReadbackDesc &desc)
    {
        grl::Rc<VkReadbackRequest> request = grl::makeRc<VkReadbackRequest>();
        m_commands.push_back(CmdReadbackTexture{request, desc});
        return request;
    }

    void VkCommandList::updateBufferImpl(const grl::Rc<Buffer> &buffer, void *data, const uint32_t size)
    {
        std::vector<uint8_t> vec{};
        vec.resize(size);
        std::memcpy(vec.data(), data, size);

        m_commands.push_back(CmdUpdateBuffer{buffer, vec});
    }
}

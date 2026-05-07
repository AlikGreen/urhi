#include "vkCommandList.h"

#include "clogr.h"
#include "renderPass.h"
#include "vkComputePass.h"
#include "vkConvert.h"
#include "vkDevice.h"
#include "vkMappedBuffer.h"
#include "vkRenderPass.h"

namespace urhi
{
    VkCommandList::VkCommandList(VkDevice *device, VkCommandQueue* queue, VkSubmissionContext* submissionContext)
        : m_device(device), m_submissionContext(submissionContext), m_queue(queue)
    {
    }

    void VkCommandList::begin()
    {
        m_readbackRequests.clear();

        m_commands.clear();

        m_commands.emplace_back(CmdBeginCommandBuffer{});
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
        const uint32_t size = desc.width * desc.height * desc.depth * VkConvert::pixelFormatBytes(desc.texture->format(), m_device);

        std::vector<uint8_t> vec{};
        vec.resize(size);
        std::memcpy(vec.data(), desc.data, size);

        m_commands.emplace_back(CmdUpdateTexture{grl::makeBox<TextureUploadDesc>(desc), vec});
    }

    void VkCommandList::generateMipmaps(const grl::Rc<Texture> &texture)
    {
        m_commands.emplace_back(CmdGenerateMips{texture});
    }

    void VkCommandList::blitTexture(const BlitTextureDesc &desc)
    {
        m_commands.emplace_back(CmdBlitTexture{grl::makeBox<BlitTextureDesc>(desc)});
    }

    grl::Rc<ReadbackRequest> VkCommandList::readback(const TextureReadbackDesc &desc)
    {
        grl::Rc<VkReadbackRequest> request = grl::makeRc<VkReadbackRequest>();
        m_commands.emplace_back(CmdReadbackTexture{request, grl::makeBox<TextureReadbackDesc>(desc)});
        return request;
    }

    grl::Rc<ReadbackRequest> VkCommandList::readback(const BufferReadbackDesc &desc)
    {
        grl::Rc<VkReadbackRequest> request = grl::makeRc<VkReadbackRequest>();
        m_commands.emplace_back(CmdReadbackBuffer{request, desc});
        return request;
    }

    void VkCommandList::updateBufferImpl(const grl::Rc<Buffer> &buffer, void *data, const uint32_t size)
    {
        std::vector<uint8_t> vec{};
        vec.resize(size);
        std::memcpy(vec.data(), data, size);

        m_commands.emplace_back(CmdUpdateBuffer{buffer, vec});
    }
}

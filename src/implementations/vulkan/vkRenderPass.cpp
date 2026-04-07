#include "vkRenderPass.h"

#include <unordered_set>

#include "clogr.h"
#include "vkConvert.h"
#include "vkMappedBuffer.h"
#include "vkPipeline.h"
#include "vkSampler.h"
#include "vkTextureView.h"
#include "vkStagedBuffer.h"

namespace urhi
{
    VkRenderPass::VkRenderPass(std::vector<Command>& commands, const RenderPassDesc &desc)
        : m_commands(commands)
    {
        m_commands.emplace_back(CmdBeginRenderPass{desc});
    }

    void VkRenderPass::setPipeline(const grl::Rc<Pipeline>& pipeline)
    {
        m_commands.emplace_back(CmdSetPipeline{pipeline, vk::PipelineBindPoint::eGraphics});
    }

    void VkRenderPass::setBuffer(const std::string &name, const grl::Rc<Buffer> &buffer)
    {
        m_commands.emplace_back(CmdSetBuffer{name, buffer});
    }

    void VkRenderPass::setTexture(const std::string &name, const grl::Rc<TextureView> &texture)
    {
        m_commands.emplace_back(CmdSetTexture{name, texture});
    }

    void VkRenderPass::setSampler(const std::string &name, const grl::Rc<Sampler> &sampler)
    {
        m_commands.emplace_back(CmdSetSampler{name, sampler});
    }

    void VkRenderPass::pushConstants(void *data, const size_t size)
    {
        std::vector<uint8_t> vec{};
        vec.resize(size);
        std::memcpy(vec.data(), data, size);

        m_commands.emplace_back(CmdPushConstants{vec});
    }

    void VkRenderPass::setVertexBuffer(const uint32_t index, const grl::Rc<Buffer> &vertexBuffer)
    {
        m_commands.emplace_back(CmdSetVertexBuffer{index, vertexBuffer});
    }

    void VkRenderPass::setIndexBuffer(const grl::Rc<Buffer> &indexBuffer, const IndexFormat indexFormat)
    {
        m_commands.emplace_back(CmdSetIndexBuffer{indexBuffer, indexFormat});
    }

    void VkRenderPass::setScissor(const Rect2D rect)
    {
        m_commands.emplace_back(CmdSetScissor{rect});
    }

    void VkRenderPass::setViewport(const Viewport viewport)
    {
        m_commands.emplace_back(CmdSetViewport{viewport});
    }

    void VkRenderPass::end()
    {
        m_commands.emplace_back(CmdEndRenderPass{});
    }

    void VkRenderPass::drawImpl(const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t firstVertex, const uint32_t firstInstance)
    {
        m_commands.emplace_back(CmdDraw{vertexCount, instanceCount, firstVertex, firstInstance});
    }

    void VkRenderPass::drawIndexedImpl(const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex, const int vertexOffset, const uint32_t firstInstance)
    {
        m_commands.emplace_back(CmdDrawIndexed{indexCount, instanceCount, firstIndex, vertexOffset, firstInstance});
    }
}

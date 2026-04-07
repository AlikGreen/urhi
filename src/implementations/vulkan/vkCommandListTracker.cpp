#include "vkCommandListTracker.h"

#include "vkTextureView.h"

namespace urhi
{
    void VkCommandListTracker::record(const CmdSetTexture &c)
    {
        m_textureUses[dynamic_cast<VkTexture*>(dynamic_cast<VkTextureView*>(c.texture.get())->texture().get())].push_back(
        {
            vk::ImageLayout::eShaderReadOnlyOptimal,
            vk::PipelineStageFlagBits2::eFragmentShader,
            vk::AccessFlagBits2::eShaderSampledRead,
            m_idx++
        });
    }

    void VkCommandListTracker::record(const CmdBeginRenderPass &c)
    {
        for (const auto& attachment : c.desc.colorAttachments)
        {
            const auto vkView = dynamic_cast<VkTextureView*>(attachment.target.get());
            const auto vkTex = dynamic_cast<VkTexture*>(vkView->texture().get());

            m_textureUses[vkTex].push_back({
                vk::ImageLayout::eColorAttachmentOptimal,
                vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                vk::AccessFlagBits2::eColorAttachmentWrite | vk::AccessFlagBits2::eColorAttachmentRead,
                m_idx
            });
        }

        if (c.desc.depthAttachment.has_value())
        {
            const auto vkView = dynamic_cast<VkTextureView*>(c.desc.depthAttachment->target.get());
            const auto vkTex = dynamic_cast<VkTexture*>(vkView->texture().get());

            m_textureUses[vkTex].push_back({
                vk::ImageLayout::eDepthAttachmentOptimal,
                vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
                vk::AccessFlagBits2::eDepthStencilAttachmentWrite | vk::AccessFlagBits2::eDepthStencilAttachmentRead,
                m_idx
            });
        }

        m_idx++;
    }

    void VkCommandListTracker::record(const CmdBlitTexture &c)
    {
        const auto vkSrc = dynamic_cast<VkTexture*>(c.desc.src.get());

        m_textureUses[vkSrc].push_back({
            vk::ImageLayout::eTransferSrcOptimal,
            vk::PipelineStageFlagBits2::eBlit,
            vk::AccessFlagBits2::eTransferRead,
            m_idx
        });

        const auto vkDst = dynamic_cast<VkTexture*>(c.desc.dst.get());

        m_textureUses[vkDst].push_back({
            vk::ImageLayout::eTransferDstOptimal,
            vk::PipelineStageFlagBits2::eBlit,
            vk::AccessFlagBits2::eTransferWrite,
            m_idx
        });
    }
}

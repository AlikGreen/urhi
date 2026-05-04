#include "vkCommandListTracker.h"

#include "vkTextureView.h"

namespace urhi
{
    namespace
    {
        vk::AccessFlags2 colorAttachmentAccess(const ColorAttachment& attachment)
        {
            vk::AccessFlags2 access{};

            if (attachment.loadOp == LoadOp::Load)
                access |= vk::AccessFlagBits2::eColorAttachmentRead;

            if (attachment.loadOp == LoadOp::Clear ||
                attachment.storeOp == StoreOp::Store)
            {
                access |= vk::AccessFlagBits2::eColorAttachmentWrite;
            }

            return access;
        }

        vk::AccessFlags2 depthAttachmentAccess(const DepthStencilAttachment& attachment)
        {
            vk::AccessFlags2 access{};

            if (attachment.loadOp == LoadOp::Load)
                access |= vk::AccessFlagBits2::eDepthStencilAttachmentRead;

            if (attachment.loadOp == LoadOp::Clear ||
                attachment.storeOp == StoreOp::Store)
            {
                access |= vk::AccessFlagBits2::eDepthStencilAttachmentWrite;
            }

            return access;
        }
    }

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
        auto& desc = *c.desc;
        for (const auto& attachment : desc.colorAttachments)
        {
            const auto vkView = dynamic_cast<VkTextureView*>(attachment.target.get());
            const auto vkTex = dynamic_cast<VkTexture*>(vkView->texture().get());

            m_textureUses[vkTex].push_back({
                vk::ImageLayout::eColorAttachmentOptimal,
                vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                colorAttachmentAccess(attachment),
                m_idx
            });
        }

        if (desc.depthAttachment.has_value())
        {
            const auto vkView = dynamic_cast<VkTextureView*>(desc.depthAttachment->target.get());
            const auto vkTex = dynamic_cast<VkTexture*>(vkView->texture().get());

            m_textureUses[vkTex].push_back({
                vk::ImageLayout::eDepthAttachmentOptimal,
                vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
                depthAttachmentAccess(*desc.depthAttachment),
                m_idx
            });
        }

        m_idx++;
    }

    void VkCommandListTracker::record(const CmdBlitTexture &c)
    {
        const auto& desc = *c.desc;
        const auto vkSrc = dynamic_cast<VkTexture*>(desc.src.get());

        m_textureUses[vkSrc].push_back({
            vk::ImageLayout::eTransferSrcOptimal,
            vk::PipelineStageFlagBits2::eBlit,
            vk::AccessFlagBits2::eTransferRead,
            m_idx
        });

        const auto vkDst = dynamic_cast<VkTexture*>(desc.dst.get());

        m_textureUses[vkDst].push_back({
            vk::ImageLayout::eTransferDstOptimal,
            vk::PipelineStageFlagBits2::eBlit,
            vk::AccessFlagBits2::eTransferWrite,
            m_idx
        });
    }
}

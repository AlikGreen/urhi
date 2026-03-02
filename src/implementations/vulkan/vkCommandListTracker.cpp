#include "vkCommandListTracker.h"

#include "vkTextureView.h"

namespace urhi
{
    void VkCommandListTracker::record(const CmdBeginRenderPass &c)
    {
        for (auto& attachment : c.desc.colorAttachments)
            m_textureUses[dynamic_cast<VkTexture*>(dynamic_cast<VkTextureView*>(attachment.target.get())->texture().get())].push_back({ vk::ImageLayout::eColorAttachmentOptimal, m_idx });
        m_idx++;
    }

    void VkCommandListTracker::record(const CmdSetTexture &c)
    {
        m_textureUses[dynamic_cast<VkTexture*>(dynamic_cast<VkTextureView*>(c.texture.get())->texture().get())].push_back({ vk::ImageLayout::eShaderReadOnlyOptimal, m_idx++ });
    }
}

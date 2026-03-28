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

    void VkCommandListTracker::record(const CmdSetImage &c)
    {
        m_textureUses[dynamic_cast<VkTexture*>(dynamic_cast<VkTextureView*>(c.texture.get())->texture().get())].push_back(
       {
           vk::ImageLayout::eGeneral,
           vk::PipelineStageFlagBits2::eComputeShader | vk::PipelineStageFlagBits2::eFragmentShader,
           vk::AccessFlagBits2::eShaderStorageWrite | vk::AccessFlagBits2::eShaderStorageRead,
           m_idx++
       });
    }
}

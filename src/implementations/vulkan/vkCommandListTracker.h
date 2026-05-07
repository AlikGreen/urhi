#pragma once
#include <unordered_map>

#include "textureView.h"
#include <vulkan/vulkan.hpp>
#include "vkCommandList.h"

namespace urhi
{
struct TextureUse
{
    vk::ImageLayout requiredLayout;
    vk::PipelineStageFlags2 requiredStageMask;
    vk::AccessFlags2 requiredAccessMask;
    uint32_t commandIndex;
};

class VkCommandListTracker
{
public:
    void record(const CmdSetTexture& c);
    void record(const CmdBeginRenderPass& c);
    void record(const CmdBlitTexture& c);

    template<typename T> void record(const T&) { m_idx++; }

    std::unordered_map<Texture*, std::vector<TextureUse>> m_textureUses;
private:
    uint32_t m_idx = 0;
};
}

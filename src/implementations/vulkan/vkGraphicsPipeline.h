#pragma once

#include "vkPipeline.h"

namespace urhi
{
class VkGraphicsPipeline final : public VkPipeline
{
public:
    VkGraphicsPipeline(VkDevice *device, const GraphicsPipelineDesc& desc);
    [[nodiscard]] uint32_t colorAttachmentsCount() const;
private:
    friend class VkCommandListEmitter;
    GraphicsPipelineDesc m_desc;
};
}

#pragma once
#include <vulkan/vulkan.hpp>

#include "pipeline.h"
#include "descriptions/graphicsPipelineDesc.h"

namespace urhi
{
class VkDevice;
class VkPipeline final : public Pipeline
{
public:
    VkPipeline(VkDevice* device, const GraphicsPipelineDesc& desc);
    [[nodiscard]] vk::Pipeline getHandle() const;
    [[nodiscard]] vk::PipelineLayout getLayout() const;
private:
    vk::PipelineLayout m_pipelineLayout;
    vk::Pipeline m_pipeline;
    VkDevice* m_device;
    GraphicsPipelineDesc m_graphicsDesc;
};
}

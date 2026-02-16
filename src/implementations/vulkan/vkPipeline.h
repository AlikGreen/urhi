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
    VkPipeline(VkDevice* device, GraphicsPipelineDesc  desc);
private:
    vk::DescriptorSetLayout createDescriptorSetLayout() const;
    vk::PipelineVertexInputStateCreateInfo createVertexInputState() const;
    vk::PipelineLayout m_pipelineLayout;
    VkDevice* m_device;
    GraphicsPipelineDesc m_graphicsDesc;
};
}

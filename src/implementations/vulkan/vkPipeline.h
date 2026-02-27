#pragma once
#include <vulkan/vulkan.hpp>

#include "pipeline.h"
#include "vkShader.h"
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
    [[nodiscard]] ShaderReflection getReflection(ShaderStage stage) const;
private:
    friend class VkRenderPass;

    vk::PipelineLayout m_layout;
    vk::Pipeline m_pipeline;
    VkDevice* m_device;

    vk::PushConstantRange* m_pushConstantRange{};

    grl::Rc<VkShader> m_fragmentShader{};
    grl::Rc<VkShader> m_vertexShader{};
};
}

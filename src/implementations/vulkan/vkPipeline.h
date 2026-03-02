#pragma once
#include <vulkan/vulkan.hpp>

#include "pipeline.h"
#include "vkShader.h"
#include "descriptions/graphicsPipelineDesc.h"

namespace urhi
{
class VkDevice;
class VkPipeline : public Pipeline
{
public:
    VkPipeline(VkDevice* device, const std::vector<grl::Rc<Shader>> &shaders);
    ~VkPipeline() override = default;

    [[nodiscard]] vk::Pipeline getHandle() const;
    [[nodiscard]] vk::PipelineLayout getLayout() const;
    [[nodiscard]] ShaderReflection getReflection(ShaderStage stage) const;
protected:
    friend class VkCommandListEmitter;

    vk::PipelineLayout m_layout;
    vk::Pipeline m_pipeline;
    VkDevice* m_device;

    std::unordered_map<ShaderStage, grl::Rc<VkShader>> m_shaderMap{};

    vk::PushConstantRange* m_pushConstantRange{};
};
}

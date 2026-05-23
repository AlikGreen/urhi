#pragma once
#include <vulkan/vulkan.hpp>

#include "pipeline.h"
#include "vkShader.h"

namespace urhi
{
    class VkBindGroup;
    class VkDevice;
class VkPipeline : public Pipeline
{
public:
    struct BindingInfo;

    VkPipeline(VkDevice* device, const std::vector<grl::Rc<Shader>> &shaders);
    ~VkPipeline() override = default;

    [[nodiscard]] vk::Pipeline handle() const { return m_pipeline; }
    [[nodiscard]] vk::PipelineLayout layout() const { return m_layout; }

    [[nodiscard]] vk::DescriptorSetLayout descriptorSetLayout(const uint32_t index) const { return m_descriptorSetLayouts.at(index); }
    [[nodiscard]] const std::vector<vk::DescriptorSetLayout>& descriptorSetLayouts() const { return m_descriptorSetLayouts; }

    ShaderReflection reflection(const ShaderStage stage) { return m_shaderMap[stage]->entryPoint().reflection; }
    [[nodiscard]] vk::PushConstantRange* pushConstantsRange() const { return m_pushConstantRange; }

    std::optional<BindingInfo> bindingInfo(uint32_t nameHash);
    const std::unordered_map<uint32_t, BindingInfo>& bindingInfo();
protected:
    VkDevice* m_device;
    std::unordered_map<ShaderStage, grl::Rc<VkShader>> m_shaderMap{};
    std::unordered_map<uint32_t, BindingInfo> m_bindingInfo;

    vk::PipelineLayout m_layout;
    vk::Pipeline m_pipeline;

    std::vector<vk::DescriptorSetLayout> m_descriptorSetLayouts;
    vk::PushConstantRange* m_pushConstantRange{};
public:
    struct BindingInfo
    {
        uint32_t set;
        uint32_t binding;
        vk::DescriptorType type;
        uint32_t nameHash;
    };
};
}

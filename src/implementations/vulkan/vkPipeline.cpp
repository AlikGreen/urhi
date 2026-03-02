#include "vkPipeline.h"

#include <ranges>
#include <utility>
#include <vulkan/vulkan.hpp>

#include "clogr.h"
#include "vkConvert.h"
#include "vkDevice.h"
#include "vkShader.h"


namespace urhi
{
    VkPipeline::VkPipeline(VkDevice* device, const std::vector<grl::Rc<Shader>> &shaders)
        : m_device(device)
    {

        std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding> layoutBindingsMap;

        auto processReflection = [&](const ShaderEntryPoint& entryPoint)
        {
            if(entryPoint.reflection.pushConstant.has_value())
            {
                const auto pc = entryPoint.reflection.pushConstant.value();
                if(m_pushConstantRange != nullptr)
                {
                    m_pushConstantRange->stageFlags |= VkConvert::shaderStage(entryPoint.stage);
                }
                else
                {
                    const auto range = new vk::PushConstantRange();
                    range->stageFlags = VkConvert::shaderStage(entryPoint.stage);
                    range->size = pc.size;
                    range->offset = pc.offset;
                    m_pushConstantRange = range;
                }
            }

            for (const auto& resource : entryPoint.reflection.resources)
            {
                auto it = layoutBindingsMap.find(resource.binding);
                if (it != layoutBindingsMap.end())
                {
                    it->second.stageFlags |= VkConvert::shaderStage(entryPoint.stage);
                }
                else
                {
                    vk::DescriptorSetLayoutBinding binding{};
                    binding.binding = resource.binding;
                    binding.descriptorType = VkConvert::resourceType(resource.type);
                    binding.descriptorCount = resource.arrayCount;
                    binding.stageFlags = VkConvert::shaderStage(entryPoint.stage);
                    layoutBindingsMap[resource.binding] = binding;
                }
            }
        };

        for(const auto& shader : shaders)
        {
            clogr::ensure(!m_shaderMap.contains(shader->entryPoint().stage), "Pipeline cannot be created with multiple shaders of the same type.");
            auto vkShader = std::dynamic_pointer_cast<VkShader>(shader);
            m_shaderMap.emplace(shader->entryPoint().stage, vkShader);
            processReflection(vkShader->entryPoint());
        }

        std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
        layoutBindings.reserve(layoutBindingsMap.size());
        for (const auto &val: layoutBindingsMap | std::views::values)
        {
            layoutBindings.push_back(val);
        }

        vk::DescriptorSetLayoutCreateInfo descriptorLayoutInfo{};
        descriptorLayoutInfo.flags = vk::DescriptorSetLayoutCreateFlagBits::ePushDescriptorKHR;
        descriptorLayoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
        descriptorLayoutInfo.pBindings = layoutBindings.data();

        auto descriptorSetLayout = m_device->getHandle().createDescriptorSetLayout(descriptorLayoutInfo);

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = m_pushConstantRange ? 1 : 0;
        pipelineLayoutInfo.pPushConstantRanges = m_pushConstantRange;

        m_layout = m_device->getHandle().createPipelineLayout(pipelineLayoutInfo);
    }

    vk::Pipeline VkPipeline::getHandle() const
    {
        return m_pipeline;
    }

    vk::PipelineLayout VkPipeline::getLayout() const
    {
        return m_layout;
    }

    ShaderReflection VkPipeline::getReflection(const ShaderStage stage) const
    {
        return m_shaderMap.at(stage)->entryPoint().reflection;
    }
}

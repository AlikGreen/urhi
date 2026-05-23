#include "vkPipeline.h"

#include <ranges>
#include <unordered_set>
#include <utility>
#include <vulkan/vulkan.hpp>

#include "clogr.h"
#include "validation.h"
#include "vkConvert.h"
#include "vkDevice.h"
#include "vkShader.h"


namespace urhi
{
    VkPipeline::VkPipeline(VkDevice* device, const std::vector<grl::Rc<Shader>> &shaders)
        : m_device(device)
    {
        std::unordered_map<uint32_t, std::unordered_map<uint32_t, vk::DescriptorSetLayoutBinding>> setsMap;
        std::unordered_map<vk::DescriptorType, uint32_t> typeCounts;

        for(const auto& shader : shaders)
        {
            URHI_VALIDATE(!m_shaderMap.contains(shader->entryPoint().stage), "Duplicate shader types - Pipeline cannot be created with multiple shaders of the same type");
            auto vkShader = std::dynamic_pointer_cast<VkShader>(shader);
            auto entryPoint = vkShader->entryPoint();
            auto stageBit = VkConvert::shaderStage(entryPoint.stage);

            m_shaderMap.emplace(entryPoint.stage, vkShader);

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
                const auto descriptorType = VkConvert::descriptorType(resource.type);
                auto& setBindings = setsMap[resource.set]; // Access the map for this specific set

                auto nameHash = grl::Hash::fnv1a32(resource.name);

                m_bindingInfo[nameHash] = {
                    resource.set,
                    resource.binding,
                    descriptorType,
                    nameHash,
                };

                if (setBindings.contains(resource.binding))
                {
                    setBindings[resource.binding].stageFlags |= stageBit;

                } else
                {
                    vk::DescriptorSetLayoutBinding b{};
                    b.binding = resource.binding;
                    b.descriptorType = VkConvert::descriptorType(resource.type);
                    b.descriptorCount = std::max(1u, resource.count);
                    b.stageFlags = stageBit;
                    setBindings[resource.binding] = b;

                    if(typeCounts.contains(descriptorType))
                    {
                        typeCounts[descriptorType]++;
                    }else
                    {
                        typeCounts[descriptorType] = 1;
                    }
                }
            }
        }

        // Determine the max set index to handle gaps if necessary
        uint32_t maxSet = 0;
        for (const auto &setIdx: setsMap | std::views::keys) maxSet = std::max(maxSet, setIdx);

        for (uint32_t i = 0; i <= maxSet; i++)
        {
            std::vector<vk::DescriptorSetLayoutBinding> bindings;
            if (setsMap.contains(i))
            {
                for (const auto &b: setsMap[i] | std::views::values)
                {
                    bindings.push_back(b);
                }
            }


            vk::DescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            layoutInfo.pBindings = bindings.data();

            m_descriptorSetLayouts.push_back(m_device->handle().createDescriptorSetLayout(layoutInfo));
        }

        // Create Pipeline Layout with all sets
        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(m_descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = m_descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = m_pushConstantRange ? 1 : 0;
        pipelineLayoutInfo.pPushConstantRanges = m_pushConstantRange;

        m_layout = m_device->handle().createPipelineLayout(pipelineLayoutInfo);
    }

    std::optional<VkPipeline::BindingInfo> VkPipeline::bindingInfo(const uint32_t nameHash)
    {
        const auto it = m_bindingInfo.find(nameHash);
        if(it != m_bindingInfo.end())
            return it->second;

        return std::nullopt;
    }

    const std::unordered_map<uint32_t, VkPipeline::BindingInfo> & VkPipeline::bindingInfo()
    {
        return m_bindingInfo;
    }
}

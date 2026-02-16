#include "vkPipeline.h"
#include <utility>
#include <vulkan/vulkan.hpp>

#include "vkConvert.h"
#include "vkDevice.h"


namespace urhi
{
    VkPipeline::VkPipeline(VkDevice* device, GraphicsPipelineDesc desc)
        : m_device(device), m_graphicsDesc(std::move(desc))
    {
        auto layout = createDescriptorSetLayout();
        const vk::PipelineLayoutCreateInfo pipelineLayoutCI{
            vk::PipelineLayoutCreateFlags{0},
            {layout} ,
            {}
        };

        auto res = device->getHandle().createPipelineLayout(&pipelineLayoutCI, nullptr, &m_pipelineLayout);
    }

    vk::DescriptorSetLayout VkPipeline::createDescriptorSetLayout() const
    {
        std::vector<vk::DescriptorSetLayoutBinding> bindings{};

        for(const auto& resource : m_graphicsDesc.shader->reflection().resources)
        {
            if(resource.type == ShaderReflection::ResourceType::PushConstant) continue;

            vk::DescriptorSetLayoutBinding binding{};
            binding.binding = resource.binding;
            binding.descriptorType = VkConvert::resourceType(resource.type);
            binding.descriptorCount = resource.arrayCount;
            binding.stageFlags = VkConvert::shaderStage(resource.stage);
            bindings.push_back(binding);
        }

        vk::DescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.bindingCount = bindings.size();
        layoutInfo.pBindings = bindings.data();

        const vk::DescriptorSetLayout setLayout = m_device->getHandle().createDescriptorSetLayout(layoutInfo);

        return setLayout;
    }

    vk::PipelineVertexInputStateCreateInfo VkPipeline::createVertexInputState() const
    {
        std::vector<vk::VertexInputBindingDescription> bindingDescriptions{};

        for(auto input : m_graphicsDesc.shader->reflection().vertexInputs)
        {
            vk::VertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = input.binding;
            bindingDescription.stride = input.stride;
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


            // Describe vertex attributes (position, color, uv, etc.)
            VkVertexInputAttributeDescription attributeDescriptions[3]{};

            // Position attribute
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;  // layout(location = 0) in shader
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, position);

            // Color attribute
            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            // UV attribute
            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, uv);

            bindingDescriptions.push_back(bindingDescription);
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = 3;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;
    }
}

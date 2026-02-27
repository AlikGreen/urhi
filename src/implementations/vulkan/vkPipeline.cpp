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
    VkPipeline::VkPipeline(VkDevice* device, const GraphicsPipelineDesc& desc)
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

        m_fragmentShader = std::dynamic_pointer_cast<VkShader>(desc.fragmentShader);
        m_vertexShader = std::dynamic_pointer_cast<VkShader>(desc.vertexShader);

        if (desc.vertexShader)
            processReflection(desc.vertexShader->entryPoint());
        if (desc.fragmentShader)
            processReflection(desc.fragmentShader->entryPoint());

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
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = m_pushConstantRange;

        m_layout = m_device->getHandle().createPipelineLayout(pipelineLayoutInfo);

        std::vector<vk::VertexInputBindingDescription> vertexBindingDescs{};
        std::vector<vk::VertexInputAttributeDescription> vertexAttributeDescs{};

        for(const auto& input : desc.vertexShader->entryPoint().reflection.vertexBindings)
        {
            vk::VertexInputBindingDescription bindingDesc{};
            bindingDesc.binding = input.binding;
            bindingDesc.stride = input.stride;
            bindingDesc.inputRate = vk::VertexInputRate::eVertex;
            vertexBindingDescs.push_back(bindingDesc);

            for(const auto& attrib : input.attributes)
            {
                vk::VertexInputAttributeDescription attribDesc{};
                attribDesc.location = attrib.location;
                attribDesc.binding = input.binding;
                attribDesc.format = VkConvert::format(attrib.type);
                attribDesc.offset = attrib.offset;
                vertexAttributeDescs.push_back(attribDesc);
            }
        }

        vk::PipelineVertexInputStateCreateInfo vertexInputState{};
        vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindingDescs.size());
        vertexInputState.pVertexBindingDescriptions = vertexBindingDescs.data();
        vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescs.size());
        vertexInputState.pVertexAttributeDescriptions = vertexAttributeDescs.data();

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState{};
        inputAssemblyState.topology = VkConvert::primitiveType(desc.primitiveType);
        inputAssemblyState.primitiveRestartEnable = false;

        vk::PipelineViewportStateCreateInfo viewportState{};
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        vk::PipelineRasterizationStateCreateInfo rasterizerState{};
        rasterizerState.depthClampEnable = false;
        rasterizerState.rasterizerDiscardEnable = false;
        rasterizerState.polygonMode = VkConvert::fillMode(desc.rasterizerState.fillMode);
        rasterizerState.cullMode = VkConvert::cullMode(desc.rasterizerState.cullMode);
        rasterizerState.frontFace = vk::FrontFace::eCounterClockwise;
        rasterizerState.depthBiasEnable = false;
        rasterizerState.lineWidth = 1.0f;

        vk::PipelineMultisampleStateCreateInfo multisampleState{};
        multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;

        vk::PipelineDepthStencilStateCreateInfo depthStencilState{};
        depthStencilState.depthTestEnable = desc.depthState.enableDepthTest;
        depthStencilState.depthWriteEnable = desc.depthState.enableDepthWrite;
        depthStencilState.depthCompareOp = VkConvert::compareOp(desc.depthState.compareOp);

        std::vector<vk::PipelineColorBlendAttachmentState> blendAttachments;
        for(const auto& attachment : desc.colorAttachments)
        {
            vk::PipelineColorBlendAttachmentState blendAttachment{};
            blendAttachment.blendEnable = attachment.blend.enableBlend;
            blendAttachment.srcColorBlendFactor = VkConvert::blendFactor(attachment.blend.srcColorFactor);
            blendAttachment.dstColorBlendFactor = VkConvert::blendFactor(attachment.blend.dstColorFactor);
            blendAttachment.colorBlendOp = VkConvert::blendOp(attachment.blend.colorOp);
            blendAttachment.srcAlphaBlendFactor = VkConvert::blendFactor(attachment.blend.srcAlphaFactor);
            blendAttachment.dstAlphaBlendFactor = VkConvert::blendFactor(attachment.blend.dstAlphaFactor);
            blendAttachment.alphaBlendOp = VkConvert::blendOp(attachment.blend.alphaOp);
            blendAttachment.colorWriteMask = VkConvert::colorWriteMask(attachment.blend.writeMask);
            blendAttachments.push_back(blendAttachment);
        }

        vk::PipelineColorBlendStateCreateInfo colorBlendState{};
        colorBlendState.attachmentCount = static_cast<uint32_t>(blendAttachments.size());
        colorBlendState.pAttachments = blendAttachments.data();

        std::vector<vk::DynamicState> dynamicStates =
        {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor
        };

        vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
        dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicStateInfo.pDynamicStates = dynamicStates.data();

        std::vector<vk::Format> colorFormats;
        for (const auto& attachment : desc.colorAttachments)
        {
            colorFormats.push_back(VkConvert::pixelFormat(attachment.format));
        }

        vk::PipelineRenderingCreateInfo renderingInfo{};
        renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colorFormats.size());
        renderingInfo.pColorAttachmentFormats = colorFormats.data();
        if (desc.depthAttachmentFormat)
        {
            renderingInfo.depthAttachmentFormat = VkConvert::pixelFormat(desc.depthAttachmentFormat.value());
            renderingInfo.stencilAttachmentFormat = renderingInfo.depthAttachmentFormat;
        }else
        {
            renderingInfo.depthAttachmentFormat = vk::Format::eUndefined;
        }


        const auto vertexModule = dynamic_cast<VkShader*>(desc.vertexShader.get())->getModule();
        const auto fragmentModule = dynamic_cast<VkShader*>(desc.fragmentShader.get())->getModule();

        std::array shaderStages =
        {
            vk::PipelineShaderStageCreateInfo
            {
                {},
                vk::ShaderStageFlagBits::eVertex,
                vertexModule,
                "main"
            },
            vk::PipelineShaderStageCreateInfo
            {
                {},
                vk::ShaderStageFlagBits::eFragment,
                fragmentModule,
                "main"
            }
        };

        vk::GraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.pNext = &renderingInfo;
        pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputState;
        pipelineInfo.pInputAssemblyState = &inputAssemblyState;
        pipelineInfo.pTessellationState = nullptr;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizerState;
        pipelineInfo.pMultisampleState = &multisampleState;
        pipelineInfo.pDepthStencilState = &depthStencilState;
        pipelineInfo.pColorBlendState = &colorBlendState;
        pipelineInfo.pDynamicState = &dynamicStateInfo;
        pipelineInfo.layout = m_layout;
        pipelineInfo.renderPass = nullptr;
        pipelineInfo.subpass = 0;

        auto result = m_device->getHandle().createGraphicsPipeline(nullptr, pipelineInfo);
        clogr::ensure(result.result == vk::Result::eSuccess, "Could not create pipeline");
        m_pipeline = result.value;
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
        switch (stage)
        {
            case ShaderStage::Fragment:
                return m_fragmentShader->entryPoint().reflection;
            case ShaderStage::Vertex:
                return m_vertexShader->entryPoint().reflection;
            default:
                return ShaderReflection{};
        }
    }
}

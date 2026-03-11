#include "vkGraphicsPipeline.h"

#include "clogr.h"
#include "vkConvert.h"

namespace urhi
{
    VkGraphicsPipeline::VkGraphicsPipeline(VkDevice *device, const GraphicsPipelineDesc &desc)
        : VkPipeline(device, desc.shaders), m_desc(desc)
    {
        VkShader* vertexShader = nullptr;
        VkShader* fragmentShader = nullptr;
        for(const auto& shader : desc.shaders)
        {
            if(shader->entryPoint().stage == ShaderStage::Vertex)
            {
                clogr::ensure(vertexShader == nullptr, "Shader contains multiple vertex shaders.");
                vertexShader = dynamic_cast<VkShader*>(shader.get());
            }
            if(shader->entryPoint().stage == ShaderStage::Fragment)
            {
                clogr::ensure(fragmentShader == nullptr, "Shader contains multiple fragment shaders.");
                fragmentShader = dynamic_cast<VkShader*>(shader.get());
            }
        }

        clogr::ensure(vertexShader && fragmentShader, "Pipeline must contain a vertex and fragment shader");

        std::vector<vk::VertexInputBindingDescription> vertexBindingDescs{};
        std::vector<vk::VertexInputAttributeDescription> vertexAttributeDescs{};

        for(const auto& input : vertexShader->entryPoint().reflection.vertexBindings)
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

        std::vector dynamicStates =
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
            colorFormats.push_back(VkConvert::pixelFormat(attachment.format, m_device));
        }

        vk::PipelineRenderingCreateInfo renderingInfo{};
        renderingInfo.colorAttachmentCount = static_cast<uint32_t>(colorFormats.size());
        renderingInfo.pColorAttachmentFormats = colorFormats.data();
        if (desc.depthAttachmentFormat)
        {
            renderingInfo.depthAttachmentFormat = VkConvert::pixelFormat(*desc.depthAttachmentFormat, m_device);

            if(*desc.depthAttachmentFormat == PixelFormat::Depth24PlusStencil8)
                renderingInfo.stencilAttachmentFormat = renderingInfo.depthAttachmentFormat;
        }else
        {
            renderingInfo.depthAttachmentFormat = vk::Format::eUndefined;
        }


        std::vector<vk::PipelineShaderStageCreateInfo> shaderStages{};

        for(const auto& shader : desc.shaders)
        {
            auto vkShader = dynamic_cast<VkShader*>(shader.get());
            auto pipelineShaderInfo = vk::PipelineShaderStageCreateInfo
            {
                {},
                VkConvert::shaderStageBits(shader->entryPoint().stage),
                vkShader->getModule(),
                "main"
            };

            shaderStages.push_back(pipelineShaderInfo);
        }

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
}

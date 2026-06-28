#include "vkComputePipeline.h"

#include "clogr.h"

namespace urhi
{
    VkComputePipeline::VkComputePipeline(VkDevice *device, const ComputePipelineDesc &desc)
        : VkPipeline(device, { desc.shader })
    {
        clogr::ensure(desc.shader != nullptr, "Description does not contain a shader");

        const auto vkShader = dynamic_cast<VkShader*>(desc.shader.get());
        clogr::ensure(vkShader->stage() == ShaderStage::Compute, "Provided shader is not of type compute");

        const auto stage = vk::PipelineShaderStageCreateInfo
        {
            {},
            vk::ShaderStageFlagBits::eCompute,
            vkShader->getModule(),
            "main"
        };

        vk::ComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.layout = m_layout;
        pipelineInfo.stage = stage;

        const auto result = m_device->handle().createComputePipeline(nullptr, pipelineInfo);
        clogr::ensure(result.result == vk::Result::eSuccess, "Could not create pipeline");
        m_pipeline = result.value;
    }
}

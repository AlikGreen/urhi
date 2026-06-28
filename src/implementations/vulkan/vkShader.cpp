#include "vkShader.h"


namespace urhi
{
    VkShader::VkShader(VkDevice *device, const ShaderEntryPoint& ep)
        : m_device(device), m_reflection(ep.reflection), m_stage(ep.stage)
    {
        const vk::ShaderModuleCreateInfo shaderModuleInfo
        {
            vk::ShaderModuleCreateFlags{0},
            ep.spirvCode
        };

        m_shaderModule = m_device->handle().createShaderModule(shaderModuleInfo);
    }

    vk::ShaderModule VkShader::getModule() const
    {
        return m_shaderModule;
    }
}

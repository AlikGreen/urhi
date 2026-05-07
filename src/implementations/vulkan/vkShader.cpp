#include "vkShader.h"


namespace urhi
{
    VkShader::VkShader(VkDevice *device, const ShaderEntryPoint& entryPoint)
        : m_device(device), m_entryPoint(entryPoint)
    {
        const vk::ShaderModuleCreateInfo shaderModuleInfo
        {
            vk::ShaderModuleCreateFlags{0},
            entryPoint.spirv
        };

        m_shaderModule = m_device->handle().createShaderModule(shaderModuleInfo);
    }

    ShaderEntryPoint VkShader::entryPoint()
    {
        return m_entryPoint;
    }

    vk::ShaderModule VkShader::getModule() const
    {
        return m_shaderModule;
    }
}

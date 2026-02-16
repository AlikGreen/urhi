#include "vkShader.h"

namespace urhi
{
    VkShader::VkShader(const VkDevice *device, SpirvShader spirv)
    {
        const vk::ShaderModuleCreateInfo shaderModuleCI{
            vk::ShaderModuleCreateFlags{0},
            spirv.spirv
        };

        m_reflection = spirv.reflection;

        auto res = device->getHandle().createShaderModule(&shaderModuleCI, nullptr, &m_shaderModule);
    }

    ShaderReflection VkShader::reflection()
    {
        return m_reflection;
    }
}

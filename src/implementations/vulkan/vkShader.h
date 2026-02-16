#pragma once
#include "shader.h"
#include "vkDevice.h"

namespace urhi
{
class VkShader final : public Shader
{
public:
    VkShader(const VkDevice* device, SpirvShader spirv);
    ShaderReflection reflection() override;
private:
    vk::ShaderModule m_shaderModule;
    ShaderReflection m_reflection;
};
}

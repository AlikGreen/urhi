#pragma once
#include "shader.h"
#include "vkDevice.h"

namespace urhi
{
class VkDevice;
class VkShader final : public Shader
{
public:
    VkShader(VkDevice* device, const ShaderEntryPoint& ep);

    [[nodiscard]] vk::ShaderModule getModule() const;

    refl::Data reflection() override { return m_reflection; }
    ShaderStage stage() override { return m_stage; }
private:
    VkDevice* m_device;
    vk::ShaderModule m_shaderModule;
    refl::Data m_reflection;
    ShaderStage m_stage;
};
}

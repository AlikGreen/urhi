#pragma once
#include "shader.h"
#include "vkDevice.h"

namespace urhi
{
class VkDevice;
class VkShader final : public Shader
{
public:
    VkShader(VkDevice* device, const ShaderEntryPoint& entryPoint);
    ShaderEntryPoint entryPoint() override;
    [[nodiscard]] vk::ShaderModule getModule() const;
private:
    VkDevice* m_device;
    vk::ShaderModule m_shaderModule;
    ShaderEntryPoint m_entryPoint;
};
}

#pragma once
#include "computePass.h"
#include <vulkan/vulkan.hpp>

#include "vkBasePass.h"

namespace urhi
{
class VkDevice;
class VkComputePass final : public ComputePass, VkPassBase
{
public:
    VkComputePass(VkDevice* device, vk::CommandBuffer cmd);

    void setPipeline(const grl::Rc<Pipeline> &pipeline) override;

    void setUniformBuffer(const std::string &name, const grl::Rc<Buffer> &buffer) override;
    void setStorageBuffer(const std::string &name, const grl::Rc<Buffer> &buffer) override;

    void setTexture(const std::string &name, const grl::Rc<TextureView> &texture) override;
    void setSampler(const std::string &name, const grl::Rc<Sampler> &sampler) override;
    void setImage(const std::string &name, const grl::Rc<TextureView> &texture, ResourceAccess access) override;

    void pushConstants(void *data, size_t size) override;

    void dispatch(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ) override;

    void end() override;
private:
    VkDevice* m_device;
    vk::CommandBuffer m_cmd;
};
}

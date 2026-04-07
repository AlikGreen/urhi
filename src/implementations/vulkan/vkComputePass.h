#pragma once
#include "computePass.h"
#include <vulkan/vulkan.hpp>

#include "vkCommandList.h"

namespace urhi
{
class VkDevice;
class VkComputePass final : public ComputePass
{
public:
    explicit VkComputePass(std::vector<Command>& commands);

    void setPipeline(const grl::Rc<Pipeline> &pipeline) override;

    void setBuffer(const std::string &name, const grl::Rc<Buffer> &buffer) override;
    void setTexture(const std::string &name, const grl::Rc<TextureView> &texture) override;
    void setSampler(const std::string &name, const grl::Rc<Sampler> &sampler) override;

    void pushConstants(void *data, size_t size) override;

    void dispatch(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ) override;

    void end() override;
private:
    std::vector<Command>& m_commands;
};
}

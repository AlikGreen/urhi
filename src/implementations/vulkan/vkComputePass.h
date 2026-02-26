#pragma once
#include "computePass.h"
#include <vulkan/vulkan.hpp>

namespace urhi
{
class VkDevice;
class VkComputePass final : public ComputePass
{
public:
    VkComputePass(VkDevice* device, vk::CommandBuffer cmd);
    void dispatch(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ) override;
private:
    VkDevice* m_device;
    vk::CommandBuffer m_cmd;
};
}

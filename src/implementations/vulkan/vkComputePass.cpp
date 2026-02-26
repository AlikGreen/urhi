#include "vkComputePass.h"

namespace urhi
{
    VkComputePass::VkComputePass(VkDevice *device, const vk::CommandBuffer cmd)
        : m_device(device), m_cmd(cmd)
    {
    }

    void VkComputePass::dispatch(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ)
    {
    }
}

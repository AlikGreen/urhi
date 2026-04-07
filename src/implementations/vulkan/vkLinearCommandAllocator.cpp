#include "vkLinearCommandAllocator.h"

#include "validation.h"
#include "vkDevice.h"

namespace urhi
{
    void VkLinearCommandAllocator::init(VkDevice *device, const uint32_t queueFamilyIndex)
    {
        m_device = device;

        vk::CommandPoolCreateInfo poolInfo{};
        poolInfo.queueFamilyIndex = queueFamilyIndex;
        poolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;

        m_pool = m_device->handle().createCommandPool(poolInfo);
    }

    void VkLinearCommandAllocator::destroy() const
    {
        if (m_pool)
        {
            m_device->handle().destroyCommandPool(m_pool);
        }
    }

    void VkLinearCommandAllocator::reset()
    {
        m_device->handle().resetCommandPool(m_pool);
        m_currentAllocateIndex = 0;
    }

    vk::CommandBuffer VkLinearCommandAllocator::allocate()
    {
        if (m_currentAllocateIndex < m_commandBuffers.size())
        {
            return m_commandBuffers[m_currentAllocateIndex++];
        }

        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.commandPool = m_pool;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = 1;

        const vk::CommandBuffer cmd = m_device->handle().allocateCommandBuffers(allocInfo)[0];

        m_commandBuffers.push_back(cmd);
        m_currentAllocateIndex++;

        return cmd;
    }
}

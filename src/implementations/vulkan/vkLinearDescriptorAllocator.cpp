#include "vkLinearDescriptorAllocator.h"

#include "validation.h"
#include "vkDevice.h"

namespace urhi
{
    void VkLinearDescriptorAllocator::init(VkDevice *device)
    {
        m_device = device;
    }

    void VkLinearDescriptorAllocator::destroy()
    {
        for (const auto p : m_freePools) m_device->handle().destroyDescriptorPool(p);
        for (const auto p : m_usedPools) m_device->handle().destroyDescriptorPool(p);
        if (m_currentPool) m_device->handle().destroyDescriptorPool(m_currentPool);

        m_freePools.clear();
        m_usedPools.clear();
        m_currentPool = VK_NULL_HANDLE;
    }

    void VkLinearDescriptorAllocator::reset()
    {
        if (m_currentPool)
        {
            m_usedPools.push_back(m_currentPool);
            m_currentPool = VK_NULL_HANDLE;
        }

        for (auto pool : m_usedPools)
        {
            m_device->handle().resetDescriptorPool(pool);
            m_freePools.push_back(pool);
        }

        m_usedPools.clear();
    }

    vk::DescriptorSet VkLinearDescriptorAllocator::allocate(const vk::DescriptorSetLayout layout)
    {
        if (!m_currentPool)
        {
            m_currentPool = grabPool();
        }

        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = m_currentPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;

        auto cAllocInfo = static_cast<VkDescriptorSetAllocateInfo>(allocInfo);
        VkDescriptorSet cSet;

        VkResult result = vkAllocateDescriptorSets(m_device->handle(), &cAllocInfo, &cSet);

        if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL)
        {
            m_usedPools.push_back(m_currentPool);
            m_currentPool = grabPool();

            cAllocInfo.descriptorPool = static_cast<VkDescriptorPool>(m_currentPool);
            result = vkAllocateDescriptorSets(m_device->handle(), &cAllocInfo, &cSet);
        }

        URHI_VALIDATE(result == VK_SUCCESS, "Failed to allocate descriptor set even after creating a new pool.");

        return cSet;
    }

    vk::DescriptorPool VkLinearDescriptorAllocator::grabPool()
    {
        if (!m_freePools.empty())
        {
            const vk::DescriptorPool pool = m_freePools.back();
            m_freePools.pop_back();
            return pool;
        }

        const std::vector<vk::DescriptorPoolSize> sizes =
        {
            { vk::DescriptorType::eSampler, 512 },
            { vk::DescriptorType::eCombinedImageSampler, 512 },
            { vk::DescriptorType::eSampledImage, 512 },
            { vk::DescriptorType::eStorageImage, 512 },
            { vk::DescriptorType::eUniformBuffer, 512 },
            { vk::DescriptorType::eStorageBuffer, 512 }
        };

        vk::DescriptorPoolCreateInfo poolInfo{};

        poolInfo.flags = vk::DescriptorPoolCreateFlags(0);
        poolInfo.maxSets = 512;
        poolInfo.poolSizeCount = static_cast<uint32_t>(sizes.size());
        poolInfo.pPoolSizes = sizes.data();

        return m_device->handle().createDescriptorPool(poolInfo);
    }
}

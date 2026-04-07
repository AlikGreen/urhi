#pragma once
#include <vulkan/vulkan.hpp>

namespace urhi
{
class VkDevice;
class VkLinearDescriptorAllocator
{
public:
    VkLinearDescriptorAllocator() = default;

    VkLinearDescriptorAllocator(const VkLinearDescriptorAllocator&) = delete;
    VkLinearDescriptorAllocator& operator=(const VkLinearDescriptorAllocator&) = delete;

    void init(VkDevice* device);
    void destroy();

    void reset();

    vk::DescriptorSet allocate(vk::DescriptorSetLayout layout);
private:
    vk::DescriptorPool grabPool();

    VkDevice* m_device{};
    vk::DescriptorPool m_currentPool = VK_NULL_HANDLE;
    std::vector<vk::DescriptorPool> m_usedPools;
    std::vector<vk::DescriptorPool> m_freePools;
};
}

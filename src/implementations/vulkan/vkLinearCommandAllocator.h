#pragma once
#include <vulkan/vulkan.hpp>

namespace urhi
{
    class VkDevice;
    class VkLinearCommandAllocator
    {
    public:
        VkLinearCommandAllocator() = default;

        VkLinearCommandAllocator(const VkLinearCommandAllocator&) = delete;
        VkLinearCommandAllocator& operator=(const VkLinearCommandAllocator&) = delete;

        void init(VkDevice* device, uint32_t queueFamilyIndex);
        void destroy() const;

        void reset();

        vk::CommandBuffer allocate();
    private:
        VkDevice* m_device = nullptr;
        vk::CommandPool m_pool;
        std::vector<vk::CommandBuffer> m_commandBuffers;
        uint32_t m_currentAllocateIndex = 0;
    };
}

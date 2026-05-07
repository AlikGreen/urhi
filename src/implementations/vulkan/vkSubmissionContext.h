#pragma once
#include <vulkan/vulkan.hpp>

#include "vkLinearStagingAllocator.h"
#include "vkLinearCommandAllocator.h"
#include "vkLinearDescriptorAllocator.h"

namespace urhi
{
class VkDevice;
class VkLinearStagingAllocator;

class VkSubmissionContext
{
public:
    VkSubmissionContext(VkDevice *device, uint32_t queueFamily);
    ~VkSubmissionContext();

    VkLinearStagingAllocator& stagingAllocator() { return m_stagingAllocator; }
    VkLinearDescriptorAllocator& descriptorAllocator() { return m_descriptorAllocator; }

    vk::CommandBuffer allocateCommandBuffer() { return m_commandAllocator.allocate(); }
    void reset();

    void completionValue(const uint32_t value) { m_completionValue = value; }
    [[nodiscard]] uint64_t completionValue() const { return m_completionValue; }
;private:
    static constexpr size_t kWarnThreshold = 64;
    static constexpr size_t kErrorThreshold = 256;

    VkDevice* m_device{};
    uint32_t m_nextBufferIndex = 0;

    VkLinearCommandAllocator m_commandAllocator;
    VkLinearStagingAllocator m_stagingAllocator;
    VkLinearDescriptorAllocator m_descriptorAllocator;

    uint64_t m_completionValue = 0;
};
}

#pragma once
#include "readbackRequest.h"

#include <vulkan/vulkan.hpp>

#include "vkLinearStagingAllocator.h"

namespace urhi
{
class VkDevice;
class VkReadbackRequest final : public ReadbackRequest
{
public:
    VkReadbackRequest() = default;
    ~VkReadbackRequest() override;
    [[nodiscard]] bool isReady() const override;
    void wait() const override;

    [[nodiscard]] const void* data() const override;
    [[nodiscard]] size_t size() const override;
private:
    friend class VkCommandListEmitter;
    VkDevice* m_device{};

    void* m_mapped{};
    size_t m_size{};

    VmaAllocation m_bufferAllocation{};
    vk::Buffer m_buffer = VK_NULL_HANDLE;

    vk::Semaphore m_timeline = VK_NULL_HANDLE;
    uint64_t m_waitValue = ~0u;
};
}

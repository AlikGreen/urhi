#pragma once
#include <vulkan/vulkan.hpp>

#include "vkBuffer.h"
#include "descriptions/bufferDesc.h"

namespace urhi
{
class VkCommandList;
class VkDevice;

class VkStagedBuffer final : public VkBuffer
{
public:
    explicit VkStagedBuffer(VkDevice* device, BufferDesc desc);

    vk::Buffer getHandle() const override;
    uint64_t getSize() const override;
private:
    VkDevice* m_device;
    vk::Buffer m_buffer{};
    uint64_t m_bufferSize;
};
}

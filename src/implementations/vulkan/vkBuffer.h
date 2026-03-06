#pragma once
#include <vulkan/vulkan.hpp>
#include "buffer.h"
#include "vkLifetime.h"

namespace urhi
{
class VkBuffer : public Buffer
{
public:
    ~VkBuffer() override = default;
    [[nodiscard]] virtual vk::Buffer handle() const = 0;
    virtual uint64_t size() const = 0;
    virtual VkLifetime& lifetime() = 0;
};
}

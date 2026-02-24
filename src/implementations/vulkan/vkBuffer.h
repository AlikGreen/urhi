#pragma once
#include <vulkan/vulkan.hpp>
#include "buffer.h"

namespace urhi
{
class VkBuffer : public Buffer
{
public:
    ~VkBuffer() override = default;
    [[nodiscard]] virtual vk::Buffer getHandle() const = 0;
    [[nodiscard]] virtual uint64_t getSize() const = 0;
};
}

#pragma once
#include "sampler.h"
#include "descriptions/samplerDesc.h"

#include <vulkan/vulkan.hpp>

#include "vkLifetime.h"

namespace urhi
{
class VkDevice;
class VkSampler final : public Sampler
{
public:
    VkSampler(VkDevice* device, const SamplerDesc& desc);
    ~VkSampler() override;

    [[nodiscard]] vk::Sampler getHandle() const;
    VkLifetime& lifetime();
private:
    VkDevice* m_device;
    vk::Sampler m_handle;

    VkLifetime m_life{};
};
}

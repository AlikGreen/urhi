#pragma once
#include "sampler.h"
#include "descriptions/samplerDesc.h"

#include <vulkan/vulkan.hpp>

namespace urhi
{
class VkDevice;
class VkSampler final : public Sampler
{
public:
    VkSampler(VkDevice* device, const SamplerDesc& desc);
    [[nodiscard]] vk::Sampler getHandle() const;
private:
    vk::Sampler m_handle;
};
}

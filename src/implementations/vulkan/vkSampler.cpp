#include "vkSampler.h"

#include "vkConvert.h"
#include "vkDevice.h"

namespace urhi
{
    VkSampler::VkSampler(VkDevice *device, const SamplerDesc &desc)
    {
        const vk::SamplerCreateInfo samplerCI
        {
            {},
            VkConvert::filter(desc.magFilter),
            VkConvert::filter(desc.minFilter),
            VkConvert::mipmapFilter(desc.mipmapFilter),
            VkConvert::addressMode(desc.addressModeU),
            VkConvert::addressMode(desc.addressModeV),
            VkConvert::addressMode(desc.addressModeW),
            desc.lodBias,
            true,
            device->getMaxAnisotropy(),
            desc.enableCompare,
            VkConvert::compareOp(desc.compareOp),
            0.0f,
            VK_LOD_CLAMP_NONE,
            VkConvert::borderColor(desc.borderColor),
            desc.unnormalizedCoordinates
        };

        m_handle = device->getHandle().createSampler(samplerCI);
    }

    vk::Sampler VkSampler::getHandle() const
    {
        return m_handle;
    }
}

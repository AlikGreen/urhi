#include "vkSampler.h"

#include "vkConvert.h"
#include "vkDevice.h"

namespace urhi
{
    VkSampler::VkSampler(VkDevice *device, const SamplerDesc &desc)
        : m_device(device)
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

    VkSampler::~VkSampler()
    {
        m_device->queueDestroy(m_life,
        [h = m_handle](const VkDevice* device)
        {
            device->getHandle().destroySampler(h);
        });
    }

    vk::Sampler VkSampler::getHandle() const
    {
        return m_handle;
    }

    VkLifetime& VkSampler::lifetime()
    {
        return m_life;
    }
}

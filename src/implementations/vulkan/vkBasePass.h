#pragma once
#include <unordered_map>
#include <grl/mem.h>
#include <vulkan/vulkan.hpp>

#include <sampler.h>
#include <buffer.h>
#include <textureView.h>

#include "pipeline.h"
#include "descriptions/shaderReflection.h"

namespace urhi
{
class VkPipeline;
class VkPassBase
{
protected:
    virtual ~VkPassBase() = default;
    explicit VkPassBase(const vk::CommandBuffer cmd) : m_cmd(cmd) {}

    void setUniformBufferImpl(const std::string& name, const grl::Rc<Buffer>& buffer);
    void setStorageBufferImpl(const std::string &name, const grl::Rc<Buffer> &buffer);

    void setTextureImpl(const std::string& name, const grl::Rc<TextureView>& texture);
    void setSamplerImpl(const std::string& name, const grl::Rc<Sampler>& sampler);
    void setImageImpl(const std::string& name, const grl::Rc<TextureView> &texture, ResourceAccess access);

    void pushConstantsImpl(const void* data, size_t size) const;
    void setPipelineImpl(const grl::Rc<Pipeline>& pipeline, vk::PipelineBindPoint bindPoint);

    void pushDescriptorsImpl(vk::PipelineBindPoint bindPoint);

    struct BoundResource
    {
        ShaderReflection::ResourceType type;
        vk::DescriptorBufferInfo bufferInfo;
        vk::DescriptorImageInfo imageInfo;
    };

    grl::Rc<VkPipeline> m_currentPipeline = nullptr;
    vk::CommandBuffer m_cmd;
    std::unordered_map<std::string, BoundResource> m_boundResources;
};
}

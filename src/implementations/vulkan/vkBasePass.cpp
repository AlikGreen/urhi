#include "vkBasePass.h"

#include <ranges>
#include <unordered_set>

#include "clogr.h"
#include "vkConvert.h"
#include "vkSampler.h"
#include "vkBuffer.h"
#include "vkPipeline.h"
#include "vkTextureView.h"

namespace urhi
{
    void VkPassBase::setUniformBufferImpl(const std::string& name, const grl::Rc<Buffer> &buffer)
    {
        clogr::ensure(m_currentPipeline != nullptr, "No pipeline set");

        const auto vkBuffer = dynamic_cast<VkBuffer*>(buffer.get());

        m_boundResources[name] = BoundResource{
            .type = ShaderReflection::ResourceType::ConstantBuffer,
            .bufferInfo = { vkBuffer->getHandle(), 0, vkBuffer->getSize() }
        };
    }

    void VkPassBase::setStorageBufferImpl(const std::string& name, const grl::Rc<Buffer> &buffer)
    {
        clogr::ensure(m_currentPipeline != nullptr, "No pipeline set");

        const auto vkBuffer = dynamic_cast<VkBuffer*>(buffer.get());

        m_boundResources[name] = BoundResource{
            .type = ShaderReflection::ResourceType::StorageBuffer,
            .bufferInfo = { vkBuffer->getHandle(), 0, vkBuffer->getSize() }
        };
    }

    void VkPassBase::setTextureImpl(const std::string& name, const grl::Rc<TextureView> &texture)
    {
        clogr::ensure(m_currentPipeline != nullptr, "Pipeline must be set before setting texture.");

        const auto vkView = dynamic_cast<VkTextureView*>(texture.get());
        const auto vkTex = dynamic_cast<VkTexture*>(vkView->texture().get());
        vkTex->transitionLayout(m_cmd, vk::ImageLayout::eShaderReadOnlyOptimal);

        m_boundResources[name] = BoundResource{
            .type = ShaderReflection::ResourceType::Texture,
            .imageInfo = { nullptr, vkView->getHandle(), vk::ImageLayout::eShaderReadOnlyOptimal }
        };
    }

    void VkPassBase::setSamplerImpl(const std::string& name, const grl::Rc<Sampler> &sampler)
    {
        clogr::ensure(m_currentPipeline != nullptr, "Pipeline must be set before setting sampler.");

        const auto vkSampler = dynamic_cast<VkSampler*>(sampler.get());

        m_boundResources[name] = BoundResource{
            .type = ShaderReflection::ResourceType::Sampler,
            .imageInfo = { vkSampler->getHandle(), nullptr, vk::ImageLayout::eUndefined }
        };
    }

    void VkPassBase::setImageImpl(const std::string& name, const grl::Rc<TextureView> &texture, ResourceAccess access)
    {
        clogr::ensure(m_currentPipeline != nullptr, "Pipeline must be set before setting image.");

        const auto vkView = dynamic_cast<VkTextureView*>(texture.get());
        const auto vkTex  = dynamic_cast<VkTexture*>(vkView->texture().get());

        vkTex->transitionLayout(m_cmd, vk::ImageLayout::eGeneral);

        m_boundResources[name] = BoundResource{
            .type      = ShaderReflection::ResourceType::StorageImage,
            .imageInfo = { nullptr, vkView->getHandle(), vk::ImageLayout::eGeneral }
        };
    }

    void VkPassBase::pushConstantsImpl(const void* data, const size_t size) const
    {
        clogr::ensure(data != nullptr, "data is nullptr");
        clogr::ensure(m_currentPipeline->m_pushConstantRange->size == size, "Size of uploaded data doesnt match shader");
        m_cmd.pushConstants(m_currentPipeline->getLayout(), m_currentPipeline->m_pushConstantRange->stageFlags, m_currentPipeline->m_pushConstantRange->offset, size, data);
    }

    void VkPassBase::setPipelineImpl(const grl::Rc<Pipeline> &pipeline, vk::PipelineBindPoint bindPoint)
    {
        m_currentPipeline = std::dynamic_pointer_cast<VkPipeline>(pipeline);
        clogr::ensure(m_currentPipeline != nullptr, "Pipeline is not a graphics pipeline");
        m_cmd.bindPipeline(bindPoint, m_currentPipeline->getHandle());
    }

    void VkPassBase::pushDescriptorsImpl(vk::PipelineBindPoint bindPoint)
    {
        std::unordered_set<uint32_t> seenBindings;
        std::vector<vk::WriteDescriptorSet> writes;
        writes.reserve(m_boundResources.size());


        for(const auto& shader: m_currentPipeline->m_shaderMap | std::views::values)
        {
            for (const auto& resource : shader->entryPoint().reflection.resources)
            {
                if (!seenBindings.insert(resource.binding).second) continue;

                auto it = m_boundResources.find(resource.name);
                if (it == m_boundResources.end())
                {
                    clogr::ensure(false, "Resource not bound: {}", resource.name);
                    continue;
                }

                vk::WriteDescriptorSet write{};
                write.dstBinding = resource.binding;
                write.descriptorCount = 1;
                write.descriptorType = VkConvert::resourceType(resource.type);

                if (resource.type == ShaderReflection::ResourceType::ConstantBuffer)
                {
                    write.pBufferInfo = &it->second.bufferInfo;
                } else
                {
                    write.pImageInfo = &it->second.imageInfo;
                }

                writes.push_back(write);
            }
        }

        m_cmd.pushDescriptorSetKHR(
            bindPoint,
            m_currentPipeline->getLayout(),
            0, // always 0 for now might change
            writes
        );

        m_boundResources.clear();
    }
}

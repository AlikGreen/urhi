#include "vkRenderPass.h"

#include <unordered_set>

#include "clogr.h"
#include "vkConvert.h"
#include "vkMappedBuffer.h"
#include "vkPipeline.h"
#include "vkSampler.h"
#include "vkTextureView.h"
#include "vkStagedBuffer.h"

namespace urhi
{
    VkRenderPass::VkRenderPass(VkDevice *device, const vk::CommandBuffer commandBuffer, const RenderPassDesc &desc)
        : m_device(device), m_cmd(commandBuffer)
    {

        std::vector<vk::RenderingAttachmentInfo> colorAttachments;
        colorAttachments.reserve(desc.colorAttachments.size());

        Rect2D renderArea = desc.renderArea;

        for(const auto& attachment : desc.colorAttachments)
        {
            auto vkTex = dynamic_cast<VkTextureView*>(attachment.target.get());

            if(renderArea.width == 0 && renderArea.height == 0)
            {
                renderArea.width = vkTex->getTexture()->getWidth();
                renderArea.height = vkTex->getTexture()->getHeight();
            }

            clogr::ensure(vkTex->getTexture()->getWidth() >= renderArea.x + renderArea.width || vkTex->getTexture()->getHeight() >= renderArea.y + renderArea.height,
                "Render area outside the bounds of render target\nRender target size: ({}, {})\nRender area: ({}, {}, {}, {})",
                vkTex->getTexture()->getWidth(), vkTex->getTexture()->getHeight(),
                renderArea.x, renderArea.y, renderArea.width, renderArea.height);

            vk::RenderingAttachmentInfo colorAttachment
            {
                vkTex->getHandle(),
                vk::ImageLayout::eColorAttachmentOptimal,
                vk::ResolveModeFlagBits::eNone,
                VK_NULL_HANDLE,
                vk::ImageLayout::eUndefined,
                VkConvert::loadOp(attachment.loadOp),
                VkConvert::storeOp(attachment.storeOp),
                VkConvert::clearValue(attachment.clearValue)
            };

            dynamic_cast<VkTexture*>(vkTex->getTexture().get())->transitionLayout(m_cmd, vk::ImageLayout::eColorAttachmentOptimal);
            colorAttachments.push_back(colorAttachment);
        }

        vk::RenderingAttachmentInfo depthAttachment{nullptr};

        if(desc.depthAttachment.has_value())
        {
            const auto vkTex = dynamic_cast<VkTextureView*>(desc.depthAttachment->target.get());


            depthAttachment =
            {
                vkTex->getHandle(),
                vk::ImageLayout::eDepthAttachmentOptimal,
                vk::ResolveModeFlagBits::eNone,
                VK_NULL_HANDLE,
                vk::ImageLayout::eUndefined,
                VkConvert::loadOp(desc.depthAttachment->loadOp),
                VkConvert::storeOp(desc.depthAttachment->storeOp),
                {{desc.depthAttachment->clearDepth, desc.depthAttachment->clearStencil}}
            };
        }

        vk::Rect2D vkRenderArea{{renderArea.x, renderArea.y}, {renderArea.width, renderArea.height}};

        const vk::RenderingInfo renderingInfo
        {
            vk::RenderingFlags{0},
            vkRenderArea,
            1,
            0,
            colorAttachments,
            &depthAttachment,
        };

        m_cmd.beginRendering(&renderingInfo);

        vk::Viewport viewport{static_cast<float>(renderArea.x), static_cast<float>(renderArea.y),
            static_cast<float>(renderArea.width), static_cast<float>(renderArea.height), 0.0f, 1.0f};

        m_cmd.setViewport(0, {viewport});
        m_cmd.setScissor(0, {vkRenderArea});
    }

    void VkRenderPass::setPipeline(const grl::Rc<Pipeline>& pipeline)
    {
        m_currentPipeline = dynamic_cast<VkPipeline*>(pipeline.get());
        m_cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_currentPipeline->getHandle());
    }

    void VkRenderPass::setUniformBuffer(const std::string& name, const grl::Rc<Buffer> &buffer)
    {
        clogr::ensure(m_currentPipeline != nullptr, "No pipeline set");

        const auto vkBuffer = dynamic_cast<VkBuffer*>(buffer.get());

        m_boundResources[name] = BoundResource{
            .type = ShaderReflection::ResourceType::ConstantBuffer,
            .bufferInfo = { vkBuffer->getHandle(), 0, vkBuffer->getSize() }
        };
    }

    void VkRenderPass::setStorageBuffer(const std::string& name, const grl::Rc<Buffer> &buffer)
    {
        clogr::abort("not implemented");
    }

    void VkRenderPass::setTexture(const std::string& name, const grl::Rc<TextureView> &texture)
    {
        clogr::ensure(m_currentPipeline != nullptr, "Pipeline must be set before setting texture.");

        const auto vkView = dynamic_cast<VkTextureView*>(texture.get());

        m_boundResources[name] = BoundResource{
            .type = ShaderReflection::ResourceType::Texture,
            .imageInfo = { nullptr, vkView->getHandle(), vk::ImageLayout::eShaderReadOnlyOptimal }
        };
    }

    void VkRenderPass::setSampler(const std::string& name, const grl::Rc<Sampler> &sampler)
    {
        clogr::ensure(m_currentPipeline != nullptr, "Pipeline must be set before setting sampler.");

        const auto vkSampler = dynamic_cast<VkSampler*>(sampler.get());

        m_boundResources[name] = BoundResource{
            .type = ShaderReflection::ResourceType::Sampler,
            .imageInfo = { vkSampler->getHandle(), nullptr, vk::ImageLayout::eUndefined }
        };
    }

    void VkRenderPass::setImage(const std::string& name, const grl::Rc<TextureView> &texture, ResourceAccess access)
    {
        clogr::abort("not implemented");
    }

    void VkRenderPass::setVertexBuffer(uint32_t index, const grl::Rc<Buffer> &vertexBuffer)
    {
        const auto vkBuffer = dynamic_cast<VkStagedBuffer*>(vertexBuffer.get());
        m_cmd.bindVertexBuffers(0, {vkBuffer->getHandle()}, {0});
    }

    void VkRenderPass::setIndexBuffer(const grl::Rc<Buffer> &indexBuffer, const IndexFormat indexFormat)
    {
        const auto vkBuffer = dynamic_cast<VkStagedBuffer*>(indexBuffer.get());
        m_cmd.bindIndexBuffer(vkBuffer->getHandle(), 0, VkConvert::indexFormat(indexFormat));
    }

    void VkRenderPass::setScissor(Rect2D rect)
    {
        const vk::Rect2D scissorRect{{rect.x, rect.y}, {rect.width, rect.height}};
        m_cmd.setScissor(0, 1, &scissorRect);
    }

    void VkRenderPass::setViewport(const Viewport viewport)
    {
        const vk::Viewport vkViewport{viewport.x, viewport.y, viewport.width, viewport.height, viewport.minDepth, viewport.maxDepth};
        m_cmd.setViewport(0, 1, &vkViewport);
    }

    void VkRenderPass::end()
    {
        m_cmd.endRendering();
    }

    void VkRenderPass::pushDescriptors()
    {
        std::unordered_set<uint32_t> seenBindings;
        std::vector<vk::WriteDescriptorSet> writes;
        writes.reserve(m_boundResources.size());

        auto processStage = [&](const ShaderStage stage)
        {
            for (const auto& resource : m_currentPipeline->getReflection(stage).resources)
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
        };

        // TODO change pipeline to be more modular eg vector of shaders or stages (so i can loop them)
        processStage(ShaderStage::Fragment);
        processStage(ShaderStage::Vertex);

        m_cmd.pushDescriptorSetKHR(
            vk::PipelineBindPoint::eGraphics,
            m_currentPipeline->getLayout(),
            0, // always 0 for now might change
            writes
        );

        m_boundResources.clear();
    }


    void VkRenderPass::drawImpl(const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t firstVertex, const uint32_t firstInstance)
    {
        pushDescriptors();
        m_cmd.draw(vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void VkRenderPass::drawIndexedImpl(const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex, const int vertexOffset, const uint32_t firstInstance)
    {
        pushDescriptors();
        m_cmd.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }
}

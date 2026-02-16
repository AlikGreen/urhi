#include "vkRenderPass.h"

#include "vkConvert.h"
#include "vkPipeline.h"
#include "vkTextureView.h"

namespace urhi
{
    VkRenderPass::VkRenderPass(VkDevice *device, const vk::CommandBuffer commandBuffer, const RenderPassDesc &desc)
        : m_device(device), m_commandBuffer(commandBuffer)
    {

        std::vector<vk::RenderingAttachmentInfo> colorAttachments{desc.colorAttachments.size()};

        for(const auto& attachment : desc.colorAttachments)
        {
            auto vkTex = dynamic_cast<VkTextureView*>(attachment.target.get());
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

            colorAttachments.push_back(colorAttachment);
        }

        vk::RenderingAttachmentInfo depthAttachment
        {
            desc.depthAttachment.target->getHandle(),
            vk::ImageLayout::eDepthAttachmentOptimal,
            vk::ResolveModeFlagBits::eNone,
            VK_NULL_HANDLE,
            vk::ImageLayout::eUndefined,
            VkConvert::loadOp(desc.depthAttachment.loadOp),
            VkConvert::storeOp(desc.depthAttachment.storeOp),
            {{desc.depthAttachment.clearDepth, desc.depthAttachment.clearStencil}}
        };

        const vk::RenderingInfo renderingInfo
        {
            vk::RenderingFlags{0},
            vk::Rect2D{{desc.renderArea.x, desc.renderArea.y}, {desc.renderArea.width, desc.renderArea.height}},
            0,
            0,
            colorAttachments,
            &depthAttachment,
        };

        m_commandBuffer.beginRendering(&renderingInfo);

        // vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    }

    void VkRenderPass::setPipeline(const grl::Rc<Pipeline>& pipeline)
    {
        auto vkPipeline = dynamic_cast<VkPipeline*>(pipeline.get());
        m_commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, vkPipeline->getHandle());
    }

    void VkRenderPass::setScissor(Rect2D rect)
    {
        const vk::Rect2D scissorRect{{rect.x, rect.y}, {rect.width, rect.height}};
        m_commandBuffer.setScissor(0, 1, &scissorRect);
    }

    void VkRenderPass::setViewport(const Viewport viewport)
    {
        const vk::Viewport vkViewport{viewport.x, viewport.y, viewport.width, viewport.height, viewport.minDepth, viewport.maxDepth};
        m_commandBuffer.setViewport(0, 1, &vkViewport);
    }

    void VkRenderPass::end()
    {
        m_commandBuffer.endRendering();
    }

    void VkRenderPass::drawImpl(const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t firstVertex, const uint32_t firstInstance)
    {
        m_commandBuffer.draw(vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void VkRenderPass::drawIndexedImpl(const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex, const int vertexOffset, const uint32_t firstInstance)
    {
        m_commandBuffer.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }
}

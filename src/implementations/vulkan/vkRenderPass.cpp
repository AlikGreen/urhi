#include "vkRenderPass.h"

#include "clogr.h"
#include "vkConvert.h"
#include "vkPipeline.h"
#include "vkTextureView.h"

namespace urhi
{
    VkRenderPass::VkRenderPass(VkDevice *device, const vk::CommandBuffer commandBuffer, const RenderPassDesc &desc)
        : m_device(device), m_commandBuffer(commandBuffer)
    {

        std::vector<vk::RenderingAttachmentInfo> colorAttachments;
        colorAttachments.reserve(desc.colorAttachments.size());

        for(const auto& attachment : desc.colorAttachments)
        {
            auto vkTex = dynamic_cast<VkTextureView*>(attachment.target.get());

            clogr::ensure(vkTex->getTexture()->getWidth() >= desc.renderArea.x + desc.renderArea.width || vkTex->getTexture()->getHeight() >= desc.renderArea.y + desc.renderArea.height,
                "Render area outside the bounds of render target\nRender target size: ({}, {})\nRender area: ({}, {}, {}, {})",
                vkTex->getTexture()->getWidth(), vkTex->getTexture()->getHeight(),
                desc.renderArea.x, desc.renderArea.y, desc.renderArea.width, desc.renderArea.height);

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

            vk::ImageMemoryBarrier2 barrier{
                vk::PipelineStageFlagBits2::eTopOfPipe,
                vk::AccessFlagBits2::eNone,
                vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                vk::AccessFlagBits2::eColorAttachmentWrite,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::eColorAttachmentOptimal,
                VK_QUEUE_FAMILY_IGNORED,
                VK_QUEUE_FAMILY_IGNORED,
                dynamic_cast<VkTexture*>(vkTex->getTexture().get())->getHandle(),
                { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
            };

            vk::DependencyInfo depInfo{};
            depInfo.setImageMemoryBarriers(barrier);
            commandBuffer.pipelineBarrier2(depInfo);

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

        clogr::ensure(desc.renderArea.width > 0 && desc.renderArea.height > 0, "Render area width and height must be grater than 0");

        const vk::RenderingInfo renderingInfo
        {
            vk::RenderingFlags{0},
            vk::Rect2D{{desc.renderArea.x, desc.renderArea.y}, {desc.renderArea.width, desc.renderArea.height}},
            1,
            0,
            colorAttachments,
            &depthAttachment,
        };

        m_commandBuffer.beginRendering(&renderingInfo);

        // vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    }

    void VkRenderPass::setPipeline(const grl::Rc<Pipeline>& pipeline)
    {
        const auto vkPipeline = dynamic_cast<VkPipeline*>(pipeline.get());
        m_commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, vkPipeline->getHandle());
    }

    void VkRenderPass::setUniformBuffer(std::string_view name, const grl::Rc<Buffer> &buffer)
    {
        clogr::abort("not implemented");
    }

    void VkRenderPass::setStorageBuffer(std::string_view name, const grl::Rc<Buffer> &buffer)
    {
        clogr::abort("not implemented");
    }

    void VkRenderPass::setTexture(std::string_view name, const grl::Rc<TextureView> &texture)
    {
        clogr::abort("not implemented");
    }

    void VkRenderPass::setSampler(std::string_view name, const grl::Rc<Sampler> &sampler)
    {
        clogr::abort("not implemented");
    }

    void VkRenderPass::setImage(std::string_view name, const grl::Rc<TextureView> &texture, ResourceAccess access)
    {
        clogr::abort("not implemented");
    }

    void VkRenderPass::setVertexBuffer(uint32_t index, const grl::Rc<Buffer> &vertexBuffer)
    {
        clogr::abort("not implemented");
    }

    void VkRenderPass::setIndexBuffer(const grl::Rc<Buffer> &indexBuffer, IndexFormat indexFormat)
    {
        clogr::abort("not implemented");
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

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
        : VkPassBase(commandBuffer), m_device(device)
    {

        std::vector<vk::RenderingAttachmentInfo> colorAttachments;
        colorAttachments.reserve(desc.colorAttachments.size());

        Rect2D renderArea = desc.renderArea;

        for(const auto& attachment : desc.colorAttachments)
        {
            auto vkTex = dynamic_cast<VkTextureView*>(attachment.target.get());

            if(renderArea.width == 0 && renderArea.height == 0)
            {
                renderArea.width = vkTex->texture()->width();
                renderArea.height = vkTex->texture()->height();
            }

            clogr::ensure(vkTex->texture()->width() >= renderArea.x + renderArea.width || vkTex->texture()->height() >= renderArea.y + renderArea.height,
                "Render area outside the bounds of render target\nRender target size: ({}, {})\nRender area: ({}, {}, {}, {})",
                vkTex->texture()->width(), vkTex->texture()->height(),
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

            dynamic_cast<VkTexture*>(vkTex->texture().get())->transitionLayout(m_cmd, vk::ImageLayout::eColorAttachmentOptimal);
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
        setPipelineImpl(pipeline, vk::PipelineBindPoint::eGraphics);
    }

    void VkRenderPass::setUniformBuffer(const std::string &name, const grl::Rc<Buffer> &buffer)
    {
        setUniformBufferImpl(name, buffer);
    }

    void VkRenderPass::setStorageBuffer(const std::string &name, const grl::Rc<Buffer> &buffer)
    {
        setStorageBufferImpl(name, buffer);
    }

    void VkRenderPass::setTexture(const std::string &name, const grl::Rc<TextureView> &texture)
    {
        setTextureImpl(name, texture);
    }

    void VkRenderPass::setSampler(const std::string &name, const grl::Rc<Sampler> &sampler)
    {
        setSamplerImpl(name, sampler);
    }

    void VkRenderPass::setImage(const std::string &name, const grl::Rc<TextureView> &texture, const ResourceAccess access)
    {
        setImageImpl(name, texture, access);
    }

    void VkRenderPass::pushConstants(void *data, const size_t size)
    {
        pushConstantsImpl(data, size);
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

    void VkRenderPass::drawImpl(const uint32_t vertexCount, const uint32_t instanceCount, const uint32_t firstVertex, const uint32_t firstInstance)
    {
        pushDescriptorsImpl(vk::PipelineBindPoint::eGraphics);
        m_cmd.draw(vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void VkRenderPass::drawIndexedImpl(const uint32_t indexCount, const uint32_t instanceCount, const uint32_t firstIndex, const int vertexOffset, const uint32_t firstInstance)
    {
        pushDescriptorsImpl(vk::PipelineBindPoint::eGraphics);
        m_cmd.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }
}

#include "vkCommandListEmitter.h"

#include <ranges>
#include <unordered_set>
#include <utility>

#include "clogr.h"
#include "urhiToString.h"
#include "validation.h"
#include "vkCommandListTracker.h"
#include "vkConvert.h"
#include "vkDevice.h"
#include "vkGraphicsPipeline.h"
#include "vkMappedBuffer.h"
#include "vkPipeline.h"
#include "vkStagedBuffer.h"
#include "vkTextureView.h"
#include "vkSampler.h"

namespace urhi
{
    namespace
    {
        vk::AccessFlags2 colorAttachmentAccess(const ColorAttachment& attachment)
        {
            vk::AccessFlags2 access{};

            if (attachment.loadOp == LoadOp::Load)
                access |= vk::AccessFlagBits2::eColorAttachmentRead;

            if (attachment.loadOp == LoadOp::Clear ||
                attachment.storeOp == StoreOp::Store)
            {
                access |= vk::AccessFlagBits2::eColorAttachmentWrite;
            }

            return access;
        }

        vk::AccessFlags2 depthAttachmentAccess(const DepthStencilAttachment& attachment)
        {
            vk::AccessFlags2 access{};

            if (attachment.loadOp == LoadOp::Load)
                access |= vk::AccessFlagBits2::eDepthStencilAttachmentRead;

            if (attachment.loadOp == LoadOp::Clear ||
                attachment.storeOp == StoreOp::Store)
            {
                access |= vk::AccessFlagBits2::eDepthStencilAttachmentWrite;
            }

            return access;
        }
    }

    VkCommandListEmitter::VkCommandListEmitter(VkDevice *device, VkCommandListTracker tracker, const vk::CommandBuffer cmd,
        VkCommandQueue* commandQueue, const uint64_t submitValue, const grl::Rc<CommandStream> &cmdStream)
            : m_device(device), m_cmd(cmd), m_tracker(std::move(tracker)), m_submitValue(submitValue), m_commandQueue(commandQueue), m_cmdStream(cmdStream)
    { }

    void VkCommandListEmitter::endRecording()
    {
        for (const auto &texture: m_tracker.m_textureUses | std::views::keys)
        {
            const auto vkTex = static_cast<VkTexture*>(texture);

            if (vkTex->isSwapchainTexture())
            {
                vkTex->transitionLayout(m_cmd,
                    vk::ImageLayout::ePresentSrcKHR,
                    vk::PipelineStageFlagBits2::eAllCommands,
                    vk::AccessFlagBits2::eNone);
                vkTex->lifetime();
            }
        }

        m_cmd.end();
    }

    void VkCommandListEmitter::emit(const CmdDrawIndexed &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline set - draw requires a graphics pipeline to be bound first");

        URHI_VALIDATE(m_boundPipelineBindPoint == vk::PipelineBindPoint::eGraphics,
            "Draw requires a graphics pipeline, but the currently bound pipeline uses bind point {}",
            vk::to_string(m_boundPipelineBindPoint));

        pushDescriptors();
        m_cmd.drawIndexed(c.indexCount, c.instanceCount, c.firstIndex, c.vertexOffset, c.firstInstance);

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdDraw &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline set - draw requires a graphics pipeline to be bound first");

        URHI_VALIDATE(m_boundPipelineBindPoint == vk::PipelineBindPoint::eGraphics,
            "Draw requires a graphics pipeline, but the currently bound pipeline uses bind point {}",
            vk::to_string(m_boundPipelineBindPoint));

        pushDescriptors();
        m_cmd.draw(c.vertexCount, c.instanceCount, c.firstVertex, c.firstInstance);

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdBeginRenderPass &c)
    {
        m_currentBindings.clear();
        m_renderPassActive = true;
        m_currentRenderPassDesc = c.desc;
        m_boundPipeline = nullptr;

        const auto& desc = c.desc;

        std::unordered_set<Texture*> attachmentTextures;

        for (const auto& attachment : desc.colorAttachments)
        {
            const auto vkView = static_cast<VkTextureView*>(attachment.target.get());
            attachmentTextures.insert(vkView->texture().get());
        }

        if (desc.depthAttachment.has_value())
        {
            const auto vkView = static_cast<VkTextureView*>(desc.depthAttachment->target.get());
            attachmentTextures.insert(vkView->texture().get());
        }

        for (auto& [texture, useList] : m_tracker.m_textureUses)
        {
            if (attachmentTextures.contains(texture))
                continue;

            for (const auto& use : useList)
            {
                if (use.commandIndex > m_idx)
                {
                    const auto tex = static_cast<VkTexture*>(texture);
                    tex->transitionLayout(m_cmd,
                        use.requiredLayout,
                        use.requiredStageMask,
                        use.requiredAccessMask);
                    break;
                }
            }
        }


        std::vector<vk::RenderingAttachmentInfo> colorAttachments;
        colorAttachments.reserve(desc.colorAttachments.size());

        Rect2D renderArea = desc.renderArea;

        for(const auto & attachment : desc.colorAttachments)
        {
            auto vkView = static_cast<VkTextureView*>(attachment.target.get());
            auto vkTex = static_cast<VkTexture*>(vkView->texture().get());
            vkView->markUsed(m_commandQueue, m_submitValue);

            URHI_VALIDATE(
                attachment.loadOp != LoadOp::Load ||
                vkTex->getLayout() != vk::ImageLayout::eUndefined,
                "Cannot use LoadOp::Load on a color attachment with undefined contents"
            );

            vkTex->transitionLayout(m_cmd,
                vk::ImageLayout::eColorAttachmentOptimal,
                vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                colorAttachmentAccess(attachment));


            if(renderArea.width == 0 && renderArea.height == 0)
            {
                renderArea.width = vkView->texture()->width();
                renderArea.height = vkView->texture()->height();
            }

            URHI_VALIDATE(
                vkView->texture()->width()  >= renderArea.x + renderArea.width &&
                vkView->texture()->height() >= renderArea.y + renderArea.height,
                "Render area outside the bounds of render target\n"
                "Render target size: ({}, {})\n"
                "Render area: ({}, {}, {}, {})",
                vkView->texture()->width(),
                vkView->texture()->height(),
                renderArea.x,
                renderArea.y,
                renderArea.width,
                renderArea.height
            );


            vk::RenderingAttachmentInfo colorAttachment
            {
                vkView->getHandle(),
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

        vk::RenderingAttachmentInfo depthAttachment{nullptr};

        if(desc.depthAttachment.has_value())
        {
            const auto vkView = static_cast<VkTextureView*>(desc.depthAttachment->target.get());
            const auto vkTex = static_cast<VkTexture*>(vkView->texture().get());
            vkView->markUsed(m_commandQueue, m_submitValue);

            URHI_VALIDATE(
                desc.depthAttachment->loadOp != LoadOp::Load ||
                vkTex->getLayout() != vk::ImageLayout::eUndefined,
                "Cannot use LoadOp::Load on a depth attachment with undefined contents"
            );

            vkTex->transitionLayout(m_cmd,
                vk::ImageLayout::eDepthAttachmentOptimal,
                vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
                depthAttachmentAccess(*desc.depthAttachment));

            URHI_VALIDATE(
                vkView->width()  >= renderArea.x + renderArea.width &&
                vkView->height() >= renderArea.y + renderArea.height,
                "Render area outside the bounds of depth target\n"
                "Depth target size: ({}, {})\n"
                "Render area: ({}, {}, {}, {})",
                vkView->width(),
                vkView->height(),
                renderArea.x,
                renderArea.y,
                renderArea.width,
                renderArea.height
            );

            depthAttachment =
            {
                vkView->getHandle(),
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
            desc.depthAttachment.has_value() ? &depthAttachment : nullptr,
        };

        m_cmd.beginRendering(&renderingInfo);

        vk::Viewport viewport{static_cast<float>(renderArea.x), static_cast<float>(renderArea.y),
            static_cast<float>(renderArea.width), static_cast<float>(renderArea.height), 0.0f, 1.0f};

        m_cmd.setViewport(0, {viewport});
        m_cmd.setScissor(0, {vkRenderArea});

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdReadbackTexture &c)
    {
        const auto& desc = c.desc;
        const auto vkTex = static_cast<VkTexture*>(desc.texture.get());

        const auto oldLayout = vkTex->getLayout();
        const auto oldStage  = vkTex->getStage();
        const auto oldAccess = vkTex->getAccess();

        URHI_VALIDATE(desc.texture != nullptr,
            "Texture must not be null");

        URHI_VALIDATE(desc.mipLevel < vkTex->mipLevelCount(),
            "Readback mip level {} out of range for texture with {} mip levels",
            desc.mipLevel, vkTex->mipLevelCount());
        URHI_VALIDATE(desc.x + desc.width  <= vkTex->width(0),  "Readback region out of bounds - x offset ({}) + width ({}) is not less than texture width ({})", desc.x, desc.width, vkTex->width(0));
        URHI_VALIDATE(desc.y + desc.height <= vkTex->height(0), "Readback region out of bounds - y offset ({}) + height ({}) is not less than texture height ({})", desc.y, desc.height, vkTex->height(0));
        URHI_VALIDATE(desc.z + desc.depth  <= vkTex->depth(0),  "Readback region out of bounds - z offset ({}) + depth ({}) is not less than texture depth ({})", desc.z, desc.depth, vkTex->depth(0));

        const bool isArray = desc.texture->type() == TextureType::Texture2DArray || desc.texture->type() == TextureType::TextureCubeArray;

        vk::BufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = desc.mipLevel;
        region.imageSubresource.baseArrayLayer = desc.baseArrayLayer;
        region.imageSubresource.layerCount = isArray ? desc.depth : 1;
        region.imageOffset = vk::Offset3D{ static_cast<int32_t>(desc.x), static_cast<int32_t>(desc.y), static_cast<int32_t>(isArray ? 1 : desc.z) };
        region.imageExtent = vk::Extent3D{ desc.width, desc.height, desc.depth };

        const uint32_t size = desc.width * desc.height * desc.depth * VkConvert::pixelFormatBytes(vkTex->format(), m_device);

        const VkBufferCreateInfo bufferInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size =  size,
            .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT
        };

        constexpr VmaAllocationCreateInfo allocInfo = {
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                     VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO
        };

        VmaAllocation allocation;
        VmaAllocationInfo allocRes;
        vk::Buffer buffer;

        auto res = vmaCreateBuffer(
            m_device->allocator(),
            &bufferInfo,
            &allocInfo,
            reinterpret_cast<::VkBuffer*>(&buffer),
            &allocation,
            &allocRes
        );

        URHI_VALIDATE(res == VK_SUCCESS, "Failed to create buffer allocation - vmaCreateBuffer returned {}", vk::to_string(static_cast<vk::Result>(res)));

        vkTex->transitionLayout(m_cmd,
            vk::ImageLayout::eTransferSrcOptimal,
            vk::PipelineStageFlagBits2::eTransfer,
            vk::AccessFlagBits2::eTransferRead);

        m_cmd.copyImageToBuffer(
        vkTex->getHandle(),
            vk::ImageLayout::eTransferSrcOptimal,
           buffer,
           1,
           &region
       );

        const auto request = std::static_pointer_cast<VkReadbackRequest>(c.request);
        request->m_device = m_device;
        request->m_mapped = allocRes.pMappedData;
        request->m_buffer = buffer;
        request->m_size = size;
        request->m_bufferAllocation = allocation;
        request->m_timeline = m_commandQueue->timelineSemaphore();
        request->m_waitValue = m_submitValue;

        vkTex->transitionLayout(m_cmd, oldLayout, oldStage, oldAccess);

        m_idx++;

        vkTex->lifetime().markUsed(m_commandQueue, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdReadbackBuffer &c)
    {
        const auto desc = c.desc;
        const auto vkBuffer = static_cast<VkBuffer*>(desc.buffer.get());

        URHI_VALIDATE(vkBuffer != nullptr, "Buffer must not be null");

        URHI_VALIDATE(desc.offset < vkBuffer->size(),
            "Readback region out of bounds - offset ({}) cannot be greater than size of buffer ({})",
            desc.offset, vkBuffer->size());

        vk::BufferCopy region = {};
        region.srcOffset = c.desc.offset;
        region.dstOffset = 0;
        region.size = c.desc.size;

        const VkBufferCreateInfo bufferInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size =  c.desc.size,
            .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT
        };

        constexpr VmaAllocationCreateInfo allocInfo = {
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                     VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO
        };

        VmaAllocation allocation;
        VmaAllocationInfo allocRes;
        vk::Buffer buffer;

        auto res = vmaCreateBuffer(
            m_device->allocator(),
            &bufferInfo,
            &allocInfo,
            reinterpret_cast<::VkBuffer*>(&buffer),
            &allocation,
            &allocRes
        );

        URHI_VALIDATE(res == VK_SUCCESS, "Failed to create buffer allocation - vmaCreateBuffer returned {}", vk::to_string(static_cast<vk::Result>(res)));

        vkBuffer->barrier(m_cmd,
            vk::PipelineStageFlagBits2::eAllCommands, vk::AccessFlagBits2::eMemoryWrite,
            vk::PipelineStageFlagBits2::eTransfer,    vk::AccessFlagBits2::eTransferRead);

        m_cmd.copyBuffer(
           vkBuffer->handle(),
           buffer,
           1,
           &region
       );


        const auto request = std::static_pointer_cast<VkReadbackRequest>(c.request);
        request->m_device = m_device;
        request->m_mapped = allocRes.pMappedData;
        request->m_buffer = buffer;
        request->m_size = c.desc.size;
        request->m_bufferAllocation = allocation;
        request->m_timeline = m_commandQueue->timelineSemaphore();
        request->m_waitValue = m_submitValue;

        m_idx++;

        vkBuffer->lifetime().markUsed(m_commandQueue, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdUpdateBuffer &c)
    {
        const void* data = m_cmdStream->getData(c.offset);
        if(const auto vkStaged = static_cast<VkStagedBuffer*>(c.buffer.get()))
        {
            m_commandQueue->submissionContext().stagingAllocator().upload(data, c.size, vkStaged->handle(), 0, m_cmd);
            vkStaged->barrierAfterUpload(m_cmd);
            vkStaged->lifetime().markUsed(m_commandQueue, m_submitValue);
        }
        else if(const auto vkMapped = static_cast<VkMappedBuffer*>(c.buffer.get()))
        {
            vkMapped->upload(data, c.size);
            vkMapped->lifetime().markUsed(m_commandQueue, m_submitValue);
        }else
        {
            URHI_VALIDATE(false, "Failed to update buffer - Buffer was not of type VkStagedBuffer or VkMappedBuffer");
        }

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdUpdateTexture &c)
    {
        auto& desc = c.desc;
        const void* data = m_cmdStream->getData(c.offset);

        m_commandQueue->submissionContext().stagingAllocator().uploadToImage(desc, m_cmd, data, c.size);

        m_idx++;

       static_cast<VkTexture*>(desc.texture.get())->lifetime().markUsed(m_commandQueue, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdSetBuffer &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline set - set sampler require a pipeline to be bound first");

        const auto layoutBinding = m_boundPipeline->bindingInfo(c.nameHash);
        if (!layoutBinding.has_value())
        {
            URHI_WARNING(false, "Binding a resource that the shader doesnt use - you should not bind resources that are not used in the shader");
            return;
        }

        const auto vkBuffer = static_cast<VkBuffer*>(c.buffer.get());

        ResourceBinding rb{};
        rb.set = layoutBinding->set;
        rb.binding = layoutBinding->binding;
        rb.type = layoutBinding->type;
        rb.isImage = false;

        rb.bufferInfo.buffer = vkBuffer->handle();
        rb.bufferInfo.offset = 0;
        rb.bufferInfo.range = vkBuffer->size();

        m_currentBindings[c.nameHash] = rb;

        vkBuffer->lifetime().markUsed(m_commandQueue, m_submitValue);
        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdSetTexture &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline set - set sampler require a pipeline to be bound first");

        const auto layoutBinding = m_boundPipeline->bindingInfo(c.nameHash);
        if (!layoutBinding.has_value())
        {
            URHI_WARNING(false, "Binding a resource that the shader doesnt use - you should not bind resources that are not used in the shader");
            return;
        }

        const auto vkView = static_cast<VkTextureView*>(c.texture.get());
        const auto vkTex = static_cast<VkTexture*>(vkView->texture().get());

        if (!m_renderPassActive)
        {
            vkTex->transitionLayout(m_cmd,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                vk::PipelineStageFlagBits2::eFragmentShader,
                vk::AccessFlagBits2::eShaderSampledRead);
        }

        ResourceBinding rb{};
        rb.set = layoutBinding->set;
        rb.binding = layoutBinding->binding;
        rb.type = layoutBinding->type;
        rb.isImage = true;

        rb.imageInfo.imageView = vkView->getHandle();
        rb.imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

        m_currentBindings[c.nameHash] = rb;

        vkView->markUsed(m_commandQueue, m_submitValue);
        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdSetSampler &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline set - set sampler require a pipeline to be bound first");

        const auto layoutBinding = m_boundPipeline->bindingInfo(c.nameHash);
        if (!layoutBinding.has_value())
        {
            URHI_WARNING(false, "Binding a resource that the shader doesnt use - you should not bind resources that are not used in the shader");
            return;
        }

        const auto vkSampler = static_cast<VkSampler*>(c.sampler.get());

        ResourceBinding rb{};
        rb.set = layoutBinding->set;
        rb.binding = layoutBinding->binding;
        rb.type = layoutBinding->type;
        rb.isImage = true;

        rb.imageInfo.sampler = vkSampler->getHandle();

        m_currentBindings[c.nameHash] = rb;

        vkSampler->lifetime().markUsed(m_commandQueue, m_submitValue);
        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdPushConstants &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline set - push constants require a pipeline to be bound first");
        URHI_VALIDATE(c.size != 0, "No data uploaded");

        const auto pcr = m_boundPipeline->pushConstantsRange();

        URHI_VALIDATE(pcr != nullptr, "This shader attached to the bound pipeline has no push constants");
        URHI_VALIDATE(pcr->size == c.size, "Size of uploaded data ({} bytes) doesnt match shader struct ({} bytes)", c.size, pcr->size);

        const void* data = m_cmdStream->getData(c.offset);

        m_cmd.pushConstants(m_boundPipeline->layout(), pcr->stageFlags, pcr->offset, c.size, data);

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdSetVertexBuffer &c)
    {
        const auto vkBuffer = static_cast<VkStagedBuffer*>(c.buffer.get());
        m_cmd.bindVertexBuffers(0, {vkBuffer->handle()}, {0});

        m_idx++;

        vkBuffer->lifetime().markUsed(m_commandQueue, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdSetIndexBuffer &c)
    {
        const auto vkBuffer = static_cast<VkStagedBuffer*>(c.buffer.get());
        m_cmd.bindIndexBuffer(vkBuffer->handle(), 0, VkConvert::indexFormat(c.format));

        m_idx++;

        vkBuffer->lifetime().markUsed(m_commandQueue, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdSetScissor &c)
    {
        URHI_VALIDATE(c.rect.x >= 0, "Scissor offset x ({}) is invalid - x offset must be >= 0", c.rect.x);
        URHI_VALIDATE(c.rect.y >= 0, "Scissor offset y ({}) is invalid - y offset must be >= 0", c.rect.y);

        const vk::Rect2D scissorRect{{c.rect.x, c.rect.y}, {c.rect.width, c.rect.height}};
        m_cmd.setScissor(0, 1, &scissorRect);

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdSetViewport &c)
    {
        const vk::Viewport vkViewport{c.viewport.x, c.viewport.y, c.viewport.width, c.viewport.height, c.viewport.minDepth, c.viewport.maxDepth};
        m_cmd.setViewport(0, 1, &vkViewport);

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdGenerateMips &c)
    {
        const auto texture = static_cast<VkTexture*>(c.texture.get());
        const auto oldLayout = texture->getLayout();
        const auto oldStage  = texture->getStage();
        const auto oldAccess = texture->getAccess();

        const uint32_t mipLevels = texture->mipLevelCount();

        URHI_VALIDATE(c.texture != nullptr,
            "Texture must not be null");

        URHI_VALIDATE(mipLevels > 1,
            "Cannot generate mipmaps for a texture with only {} mip level",
            mipLevels);

        const vk::FormatProperties formatProperties = m_device->getPhysicalDevice().getFormatProperties(VkConvert::pixelFormat(texture->format(), m_device));
        URHI_VALIDATE(static_cast<bool>(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear), "Texture format is not blitable - generate mipmaps requires the texture format to be blitable");

        auto mipWidth = static_cast<int32_t>(texture->width(0));
        auto mipHeight = static_cast<int32_t>(texture->height(0));

        texture->transitionLayout(m_cmd,
            vk::ImageLayout::eTransferDstOptimal,
            vk::PipelineStageFlagBits2::eTransfer,
            vk::AccessFlagBits2::eTransferWrite);

        uint32_t layerCount = texture->type() == TextureType::Texture2DArray || texture->type() == TextureType::TextureCubeArray ? texture->depth(0) : 1;

        for (uint32_t i = 1; i < mipLevels; i++)
        {
            vk::ImageMemoryBarrier2 srcBarrier{
                vk::PipelineStageFlagBits2::eTransfer,
                vk::AccessFlagBits2::eTransferWrite,
                vk::PipelineStageFlagBits2::eTransfer,
                vk::AccessFlagBits2::eTransferRead,
                vk::ImageLayout::eTransferDstOptimal,
                vk::ImageLayout::eTransferSrcOptimal,
                VK_QUEUE_FAMILY_IGNORED,
                VK_QUEUE_FAMILY_IGNORED,
                texture->getHandle(),
                { vk::ImageAspectFlagBits::eColor, i - 1, 1, 0, layerCount }
            };

            m_cmd.pipelineBarrier2(vk::DependencyInfo{}.setImageMemoryBarriers(srcBarrier));

            vk::ImageBlit2 blit{};
            blit.srcOffsets = std::array{ vk::Offset3D{ 0, 0, 0 }, vk::Offset3D{ mipWidth, mipHeight, 1 } };
            blit.srcSubresource = vk::ImageSubresourceLayers
            {
                vk::ImageAspectFlagBits::eColor,
                i - 1,
                0,
                layerCount
            };

            blit.dstOffsets = std::array{ vk::Offset3D{ 0, 0, 0 }, vk::Offset3D{ mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 } };
            blit.dstSubresource = vk::ImageSubresourceLayers
            {
                vk::ImageAspectFlagBits::eColor,
                i,
                0,
                layerCount
            };

            vk::BlitImageInfo2 blitInfo
            {
                texture->getHandle(),
                vk::ImageLayout::eTransferSrcOptimal,
                texture->getHandle(),
                vk::ImageLayout::eTransferDstOptimal,
                1u,
                &blit,
                vk::Filter::eLinear
            };

            m_cmd.blitImage2(blitInfo);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        vk::ImageMemoryBarrier2 lastMipBarrier{
            vk::PipelineStageFlagBits2::eTransfer,
            vk::AccessFlagBits2::eTransferWrite,
            vk::PipelineStageFlagBits2::eTransfer,
            vk::AccessFlagBits2::eTransferRead,
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eTransferSrcOptimal,
            VK_QUEUE_FAMILY_IGNORED,
            VK_QUEUE_FAMILY_IGNORED,
            texture->getHandle(),
            { vk::ImageAspectFlagBits::eColor, mipLevels - 1, 1, 0, layerCount }
        };

        m_cmd.pipelineBarrier2(vk::DependencyInfo{}.setImageMemoryBarriers(lastMipBarrier));

        texture->resetTrackedState(
            vk::ImageLayout::eTransferSrcOptimal,
            vk::PipelineStageFlagBits2::eTransfer,
            vk::AccessFlagBits2::eTransferRead);

        texture->transitionLayout(m_cmd, oldLayout, oldStage, oldAccess);

        m_idx++;

        texture->lifetime().markUsed(m_commandQueue, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdBlitTexture &c) const
    {
        const auto& desc = c.desc;
        const auto vkSrc = static_cast<VkTexture*>(desc.src.get());
        const auto vkDst = static_cast<VkTexture*>(desc.dst.get());

        const auto srcOldLayout = vkSrc->getLayout();
        const auto srcOldStage  = vkSrc->getStage();
        const auto srcOldAccess = vkSrc->getAccess();

        const auto dstOldLayout = vkDst->getLayout();
        const auto dstOldStage  = vkDst->getStage();
        const auto dstOldAccess = vkDst->getAccess();

        URHI_VALIDATE(desc.src != nullptr, "Source texture must not be null");
        URHI_VALIDATE(desc.dst != nullptr, "Destination texture must not be null");
        URHI_VALIDATE(desc.srcMipLevel < vkSrc->mipLevelCount(),
            "Source mip level {} out of range for texture with {} mip levels",
            desc.srcMipLevel, vkSrc->mipLevelCount());

        URHI_VALIDATE(desc.dstMipLevel < vkDst->mipLevelCount(),
            "Destination mip level {} out of range for texture with {} mip levels",
            desc.dstMipLevel, vkDst->mipLevelCount());

        URHI_VALIDATE(
        static_cast<int32_t>(desc.srcOffset.x) >= 0 &&
        static_cast<int32_t>(desc.srcOffset.y) >= 0 &&
        static_cast<int32_t>(desc.srcOffset.z) >= 0,
        "Source offset must be non-negative");

        URHI_VALIDATE(
            static_cast<int32_t>(desc.dstOffset.x) >= 0 &&
            static_cast<int32_t>(desc.dstOffset.y) >= 0 &&
            static_cast<int32_t>(desc.dstOffset.z) >= 0,
            "Destination offset must be non-negative");

        URHI_VALIDATE(desc.src != desc.dst, "Source and destination textures are the same — blit requires distinct textures");

        vkSrc->transitionLayout(m_cmd,
            vk::ImageLayout::eTransferSrcOptimal,
            vk::PipelineStageFlagBits2::eTransfer,
            vk::AccessFlagBits2::eTransferRead);

        vkDst->transitionLayout(m_cmd,
            vk::ImageLayout::eTransferDstOptimal,
            vk::PipelineStageFlagBits2::eTransfer,
            vk::AccessFlagBits2::eTransferWrite);

        vk::ImageBlit2 blit{};

        const int32_t srcMaxX = desc.srcExtent.x != 0 ? desc.srcExtent.x + static_cast<int32_t>(desc.srcOffset.x) : static_cast<int32_t>(vkSrc->width (desc.srcMipLevel));
        const int32_t srcMaxY = desc.srcExtent.y != 0 ? desc.srcExtent.y + static_cast<int32_t>(desc.srcOffset.y) : static_cast<int32_t>(vkSrc->height(desc.srcMipLevel));
        const int32_t srcMaxZ = desc.srcExtent.z != 0 ? desc.srcExtent.z + static_cast<int32_t>(desc.srcOffset.z) : static_cast<int32_t>(vkSrc->depth (desc.srcMipLevel)) ;

        const int32_t dstMaxX = desc.dstExtent.x != 0 ? desc.dstExtent.x + static_cast<int32_t>(desc.dstOffset.x) : static_cast<int32_t>(vkDst->width (desc.dstMipLevel));
        const int32_t dstMaxY = desc.dstExtent.y != 0 ? desc.dstExtent.y + static_cast<int32_t>(desc.dstOffset.y) : static_cast<int32_t>(vkDst->height(desc.dstMipLevel));
        const int32_t dstMaxZ = desc.dstExtent.z != 0 ? desc.dstExtent.z + static_cast<int32_t>(desc.dstOffset.z) : static_cast<int32_t>(vkDst->depth (desc.dstMipLevel));

        URHI_VALIDATE(srcMaxX > static_cast<int32_t>(desc.srcOffset.x) &&
              srcMaxY > static_cast<int32_t>(desc.srcOffset.y) &&
              srcMaxZ > static_cast<int32_t>(desc.srcOffset.z),
            "Source blit region must have non-zero extent");

        URHI_VALIDATE(dstMaxX > static_cast<int32_t>(desc.dstOffset.x) &&
                      dstMaxY > static_cast<int32_t>(desc.dstOffset.y) &&
                      dstMaxZ > static_cast<int32_t>(desc.dstOffset.z),
            "Destination blit region must have non-zero extent");

        URHI_VALIDATE(srcMaxX <= static_cast<int32_t>(vkSrc->width(desc.srcMipLevel)) &&
                      srcMaxY <= static_cast<int32_t>(vkSrc->height(desc.srcMipLevel)) &&
                      srcMaxZ <= static_cast<int32_t>(vkSrc->depth(desc.srcMipLevel)),
            "Source blit region out of bounds for source mip {} (size: {}, {}, {})",
            desc.srcMipLevel,
            vkSrc->width(desc.srcMipLevel),
            vkSrc->height(desc.srcMipLevel),
            vkSrc->depth(desc.srcMipLevel));

        URHI_VALIDATE(dstMaxX <= static_cast<int32_t>(vkDst->width(desc.dstMipLevel)) &&
                      dstMaxY <= static_cast<int32_t>(vkDst->height(desc.dstMipLevel)) &&
                      dstMaxZ <= static_cast<int32_t>(vkDst->depth(desc.dstMipLevel)),
            "Destination blit region out of bounds for destination mip {} (size: {}, {}, {})",
            desc.dstMipLevel,
            vkDst->width(desc.dstMipLevel),
            vkDst->height(desc.dstMipLevel),
            vkDst->depth(desc.dstMipLevel));

        blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        blit.srcSubresource.mipLevel = desc.srcMipLevel;
        blit.srcSubresource.baseArrayLayer = desc.srcArrayLayer;
        blit.srcSubresource.layerCount = 1;
        blit.srcOffsets[0] = vk::Offset3D{static_cast<int32_t>(desc.srcOffset.x), static_cast<int32_t>(desc.srcOffset.y), static_cast<int32_t>(desc.srcOffset.z)};
        blit.srcOffsets[1] = vk::Offset3D{srcMaxX, srcMaxY, srcMaxZ};

        blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        blit.dstSubresource.mipLevel = desc.dstMipLevel;
        blit.dstSubresource.baseArrayLayer = desc.dstArrayLayer;
        blit.dstSubresource.layerCount = 1;
        blit.dstOffsets[0] = vk::Offset3D{static_cast<int32_t>(desc.dstOffset.x), static_cast<int32_t>(desc.dstOffset.y), static_cast<int32_t>(desc.dstOffset.z)};
        blit.dstOffsets[1] = vk::Offset3D{dstMaxX, dstMaxY, dstMaxZ};

        vk::BlitImageInfo2 blitInfo{};
        blitInfo.srcImage       = vkSrc->getHandle();
        blitInfo.srcImageLayout = vk::ImageLayout::eTransferSrcOptimal;
        blitInfo.dstImage       = vkDst->getHandle();
        blitInfo.dstImageLayout = vk::ImageLayout::eTransferDstOptimal;
        blitInfo.regionCount    = 1;
        blitInfo.pRegions       = &blit;
        blitInfo.filter         = VkConvert::filter(desc.filter);

        m_cmd.blitImage2(blitInfo);

        vkSrc->transitionLayout(m_cmd, srcOldLayout, srcOldStage, srcOldAccess);
        vkDst->transitionLayout(m_cmd, dstOldLayout, dstOldStage, dstOldAccess);

        vkSrc->lifetime().markUsed(m_commandQueue, m_submitValue);
        vkDst->lifetime().markUsed(m_commandQueue, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdDispatchCompute &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline set — compute dispatch requires a compute pipeline to be set first");

        URHI_VALIDATE(m_boundPipelineBindPoint == vk::PipelineBindPoint::eCompute,
            "Compute dispatch requires a compute pipeline, but the currently bound pipeline uses bind point {}",
            vk::to_string(m_boundPipelineBindPoint));

        pushDescriptors();
        m_cmd.dispatchBase(0, 0, 0, c.groupsX, c.groupsY, c.groupsZ);

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdEndRenderPass &c)
    {
        m_renderPassActive = false;
        m_cmd.endRendering();
        m_boundPipeline = nullptr;

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdBeginComputePass &c)
    {
        m_currentBindings.clear();
        m_computePassActive = true;
        m_boundPipeline = nullptr;

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdEndComputePass &c)
    {
        m_computePassActive = false;
        m_boundPipeline = nullptr;

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdBeginCommandBuffer &c)
    {
        constexpr vk::CommandBufferBeginInfo info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        m_cmd.begin(info);

        m_idx++;
    }


    void VkCommandListEmitter::emit(const CmdSetGraphicsPipeline &c)
    {
        URHI_VALIDATE(c.pipeline != nullptr, "Pipeline must not be null");
        m_boundPipeline = std::static_pointer_cast<VkPipeline>(c.pipeline);

        m_boundPipelineBindPoint = vk::PipelineBindPoint::eGraphics;

        #if defined(URHI_ENABLE_VALIDATION)
            const auto& pipelineDesc = static_cast<VkGraphicsPipeline*>(m_boundPipeline.get())->m_desc;
            const auto& renderPassDesc = m_currentRenderPassDesc;

            URHI_VALIDATE(
                pipelineDesc.colorAttachments.size() == renderPassDesc.colorAttachments.size(),
                "Color attachment count mismatch between graphics pipeline and active render pass\n"
                "Pipeline color attachment count: {}\n"
                "Render pass color attachment count: {}",
                pipelineDesc.colorAttachments.size(),
                renderPassDesc.colorAttachments.size()
            );

            URHI_VALIDATE(
                pipelineDesc.depthAttachmentFormat.has_value() == renderPassDesc.depthAttachment.has_value(),
                "Depth attachment presence mismatch between graphics pipeline and active render pass\n"
                "Pipeline has depth attachment: {}\n"
                "Render pass has depth attachment: {}",
                pipelineDesc.depthAttachmentFormat.has_value(),
                renderPassDesc.depthAttachment.has_value()
            );

            for(uint32_t i = 0; i < renderPassDesc.colorAttachments.size(); ++i)
            {
                const auto vkTex = static_cast<VkTextureView*>(renderPassDesc.colorAttachments[i].target.get());

                URHI_VALIDATE(
                    vkTex != nullptr,
                    "Render pass color attachment at slot {} is not a VkTextureView",
                    i
                );

                URHI_VALIDATE(
                    pipelineDesc.colorAttachments[i].format == vkTex->format(),
                    "Color attachment format mismatch between graphics pipeline and active render pass at slot {}\n"
                    "Pipeline format: {}\n"
                    "Render pass format: {}",
                    i,
                    urhi::toString(pipelineDesc.colorAttachments[i].format),
                    urhi::toString(vkTex->format())
                );
            }

            if(renderPassDesc.depthAttachment.has_value())
            {
                const auto vkTex = static_cast<VkTextureView*>(renderPassDesc.depthAttachment->target.get());

                URHI_VALIDATE(
                    vkTex != nullptr,
                    "Render pass depth attachment is not a VkTextureView"
                );

                URHI_VALIDATE(
                    pipelineDesc.depthAttachmentFormat.value() == vkTex->format(),
                    "Depth attachment format mismatch between graphics pipeline and active render pass\n"
                    "Pipeline depth format: {}\n"
                    "Render pass depth format: {}",
                    urhi::toString(pipelineDesc.depthAttachmentFormat.value()),
                    urhi::toString(vkTex->format())
                );
            }
        #endif

        m_cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, m_boundPipeline->handle());
        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdSetComputePipeline &c)
    {

    }

    void VkCommandListEmitter::pushDescriptors()
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline set - descriptors cannot be pushed before binding a pipeline");

#if defined(URHI_ENABLE_VALIDATION)
        for (const auto& [resourceName, expectedLayout] : m_boundPipeline->bindingInfo())
        {
            URHI_VALIDATE(m_currentBindings.contains(resourceName),
                "Missing Shader Resource! The pipeline expects a resource named '{}' (Set: {}, Binding: {}), "
                "but it was never bound to the command list.",
                resourceName, expectedLayout.set, expectedLayout.binding);
        }
#endif


        auto& allocator = m_commandQueue->submissionContext().descriptorAllocator();
        const uint32_t numSets = m_boundPipeline->descriptorSetLayouts().size();
        std::vector<vk::DescriptorSet> allocatedSets(numSets);

        for(uint32_t i = 0; i < numSets; i++)
        {
            allocatedSets[i] = allocator.allocate(m_boundPipeline->descriptorSetLayout(i));
        }

        std::vector<vk::WriteDescriptorSet> writes;
        writes.reserve(m_currentBindings.size());

        for (auto &rb: m_currentBindings | std::views::values)
        {
            vk::WriteDescriptorSet write{};
            write.dstSet = allocatedSets[rb.set];
            write.dstBinding = rb.binding;
            write.dstArrayElement = 0;
            write.descriptorCount = 1;
            write.descriptorType = rb.type;

            if (rb.isImage)
            {
                write.pImageInfo = &rb.imageInfo;
            } else
            {
                write.pBufferInfo = &rb.bufferInfo;
            }

            writes.push_back(write);
        }

        m_device->handle().updateDescriptorSets(writes, {});

        m_cmd.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            m_boundPipeline->layout(),
            0,
            allocatedSets,
            {}
        );
    }
}

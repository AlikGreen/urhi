#include "vkCommandListEmitter.h"

#include <numeric>
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
    VkCommandListEmitter::VkCommandListEmitter(VkDevice* device, QueueType queueType, uint64_t submitValue, const vk::Semaphore timeline, const vk::CommandBuffer cmd, VkCommandListTracker tracker, grl::Rc<VkLinearStagingAllocator> stagingAllocator)
        : m_device(device), m_cmd(cmd), m_tracker(std::move(tracker)), m_submitValue(submitValue), m_timeline(timeline), m_queueType(queueType), m_stagingAllocator(std::move(stagingAllocator)) { }

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
        m_isRendering = true;
        m_currentRenderPassDesc = c.desc;
        m_boundResources.clear();

        for (auto& [texture, useList] : m_tracker.m_textureUses)
        {
            for (const auto& use : useList)
            {
                if (use.commandIndex >= m_idx)
                {
                    const auto tex = dynamic_cast<VkTexture*>(texture);
                    tex->transitionLayout(m_cmd,
                        use.requiredLayout,
                        use.requiredStageMask,
                        use.requiredAccessMask);
                    break;
                }
            }
        }

        const auto& desc = c.desc;


        std::vector<vk::RenderingAttachmentInfo> colorAttachments;
        colorAttachments.reserve(desc.colorAttachments.size());

        Rect2D renderArea = desc.renderArea;

        for(const auto & attachment : desc.colorAttachments)
        {
            auto vkView = dynamic_cast<VkTextureView*>(attachment.target.get());
            auto vkTex = dynamic_cast<VkTexture*>(vkView->texture().get());
            vkView->markUsed(m_queueType, m_submitValue);
            vkTex->transitionLayout(m_cmd,
                vk::ImageLayout::eColorAttachmentOptimal,
                vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                vk::AccessFlagBits2::eColorAttachmentWrite | vk::AccessFlagBits2::eColorAttachmentRead);


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
            const auto vkView = dynamic_cast<VkTextureView*>(desc.depthAttachment->target.get());
            const auto vkTex = dynamic_cast<VkTexture*>(vkView->texture().get());
            vkView->markUsed(m_queueType, m_submitValue);
            vkTex->transitionLayout(m_cmd,
                vk::ImageLayout::eDepthAttachmentOptimal,
                vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
                vk::AccessFlagBits2::eDepthStencilAttachmentWrite | vk::AccessFlagBits2::eDepthStencilAttachmentRead);

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
            &depthAttachment,
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
        const auto desc = c.desc;
        const auto vkTex = dynamic_cast<VkTexture*>(desc.texture.get());

        const auto oldLayout = vkTex->getLayout();
        const auto oldStage  = vkTex->getStage();
        const auto oldAccess = vkTex->getAccess();

        URHI_VALIDATE(desc.texture != nullptr,
            "Texture must not be null");

        URHI_VALIDATE(desc.mipLevel < vkTex->mipLevelCount(),
            "Readback mip level {} out of range for texture with {} mip levels",
            desc.mipLevel, vkTex->mipLevelCount());

        URHI_VALIDATE(desc.baseArrayLayer + desc.arrayLayerCount <= vkTex->arrayLayerCount(),
            "Readback array layer range [{}, {}) out of bounds for texture with {} layers",
            desc.baseArrayLayer,
            desc.baseArrayLayer + desc.arrayLayerCount,
            vkTex->arrayLayerCount());

        URHI_VALIDATE(desc.x + desc.width  <= vkTex->width(0),  "Readback region out of bounds - x offset ({}) + width ({}) is not less than texture width ({})", desc.x, desc.width, vkTex->width(0));
        URHI_VALIDATE(desc.y + desc.height <= vkTex->height(0), "Readback region out of bounds - y offset ({}) + height ({}) is not less than texture height ({})", desc.y, desc.height, vkTex->height(0));
        URHI_VALIDATE(desc.z + desc.depth  <= vkTex->depth(0),  "Readback region out of bounds - z offset ({}) + depth ({}) is not less than texture depth ({})", desc.z, desc.depth, vkTex->depth(0));

        vk::BufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = desc.mipLevel;
        region.imageSubresource.baseArrayLayer = desc.baseArrayLayer;
        region.imageSubresource.layerCount = desc.arrayLayerCount;
        region.imageOffset = vk::Offset3D{ static_cast<int32_t>(desc.x), static_cast<int32_t>(desc.y), static_cast<int32_t>(desc.z) };
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
            m_device->getAllocator(),
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

        c.request->m_device = m_device;
        c.request->m_mapped = allocRes.pMappedData;
        c.request->m_buffer = buffer;
        c.request->m_size = size;
        c.request->m_bufferAllocation = allocation;
        c.request->m_timeline = m_timeline;
        c.request->m_waitValue = m_submitValue;

        vkTex->transitionLayout(m_cmd, oldLayout, oldStage, oldAccess);

        m_idx++;

        vkTex->lifetime().markUsed(m_queueType, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdReadbackBuffer &c)
    {
        const auto desc = c.desc;
        const auto vkBuffer = dynamic_cast<VkBuffer*>(desc.buffer.get());

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
            m_device->getAllocator(),
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

        c.request->m_device = m_device;
        c.request->m_mapped = allocRes.pMappedData;
        c.request->m_buffer = buffer;
        c.request->m_size = c.desc.size;
        c.request->m_bufferAllocation = allocation;
        c.request->m_timeline = m_timeline;
        c.request->m_waitValue = m_submitValue;

        m_idx++;

        vkBuffer->lifetime().markUsed(m_queueType, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdUpdateBuffer &c)
    {
        if(const auto vkStaged = dynamic_cast<VkStagedBuffer*>(c.buffer.get()))
        {
            m_stagingAllocator->upload(c.data.data(), c.data.size(), vkStaged->handle(), 0, m_cmd);
            vkStaged->barrierAfterUpload(m_cmd);
            vkStaged->lifetime().markUsed(m_queueType, m_submitValue);
        }
        else if(const auto vkMapped = dynamic_cast<VkMappedBuffer*>(c.buffer.get()))
        {
            vkMapped->upload(c.data.data(), c.data.size());
            vkMapped->lifetime().markUsed(m_queueType, m_submitValue);
        }else
        {
            URHI_VALIDATE(false, "Failed to update buffer - Buffer was not of type VkStagedBuffer or VkMappedBuffer");
        }

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdUpdateTexture &c)
    {
        auto desc = c.desc;
        desc.data = c.data.data();

        m_stagingAllocator->uploadToImage(desc, m_cmd);

        m_idx++;

       dynamic_cast<VkTexture*>(c.desc.texture.get())->lifetime().markUsed(m_queueType, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdSetUniformBuffer &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, " No pipeline set — compute dispatch requires a compute pipeline to be set first");

        const auto vkBuffer = dynamic_cast<VkBuffer*>(c.buffer.get());

        m_boundResources[c.name] = BoundResource{
            .type = ShaderReflection::ResourceType::ConstantBuffer,
            .bufferInfo = { vkBuffer->handle(), 0, vkBuffer->size() }
        };

        m_idx++;

        vkBuffer->lifetime().markUsed(m_queueType, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdSetStorageBuffer &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline set — compute dispatch requires a compute pipeline to be set first");

        const auto vkBuffer = dynamic_cast<VkBuffer*>(c.buffer.get());

        m_boundResources[c.name] = BoundResource{
            .type = ShaderReflection::ResourceType::StorageBuffer,
            .bufferInfo = { vkBuffer->handle(), 0, vkBuffer->size() }
        };

        m_idx++;

        vkBuffer->lifetime().markUsed(m_queueType, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdPushConstants &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline set - push constants require a pipeline to be bound first");
        URHI_VALIDATE(!c.data.empty(), "No data uploaded");
        URHI_VALIDATE(m_boundPipeline->m_pushConstantRange != nullptr, "This shader attached to the bound pipeline has no push constants");
        URHI_VALIDATE(m_boundPipeline->m_pushConstantRange->size == c.data.size(), "Size of uploaded data ({} bytes) doesnt match shader struct ({} bytes)", c.data.size(), m_boundPipeline->m_pushConstantRange->size);

        m_cmd.pushConstants(m_boundPipeline->getLayout(), m_boundPipeline->m_pushConstantRange->stageFlags, m_boundPipeline->m_pushConstantRange->offset, c.data.size(), c.data.data());

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdSetTexture &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "Pipeline must be set before setting texture.");

        const auto vkView = dynamic_cast<VkTextureView*>(c.texture.get());

        m_boundResources[c.name] = BoundResource{
            .type = ShaderReflection::ResourceType::Texture,
            .imageInfo = { nullptr, vkView->getHandle(), vk::ImageLayout::eShaderReadOnlyOptimal }
        };

        m_idx++;

        vkView->markUsed(m_queueType, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdSetSampler &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "Pipeline must be set before setting sampler.");

        const auto vkSampler = dynamic_cast<VkSampler*>(c.sampler.get());

        m_boundResources[c.name] = BoundResource{
            .type = ShaderReflection::ResourceType::Sampler,
            .imageInfo = { vkSampler->getHandle(), nullptr, vk::ImageLayout::eUndefined }
        };

        m_idx++;

        vkSampler->lifetime().markUsed(m_queueType, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdSetImage &c)
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "Pipeline must be set before setting image.");

        const auto vkView = dynamic_cast<VkTextureView*>(c.texture.get());
        const auto vkTex  = dynamic_cast<VkTexture*>(vkView->texture().get());

        vkTex->transitionLayout(m_cmd,
            vk::ImageLayout::eGeneral,
            vk::PipelineStageFlagBits2::eComputeShader | vk::PipelineStageFlagBits2::eFragmentShader,
           vk::AccessFlagBits2::eShaderStorageWrite | vk::AccessFlagBits2::eShaderStorageRead
           );

        m_boundResources[c.name] = BoundResource{
            .type      = ShaderReflection::ResourceType::StorageImage,
            .imageInfo = { nullptr, vkView->getHandle(), vk::ImageLayout::eGeneral }
        };

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdSetVertexBuffer &c)
    {
        const auto vkBuffer = dynamic_cast<VkStagedBuffer*>(c.buffer.get());
        m_cmd.bindVertexBuffers(0, {vkBuffer->handle()}, {0});

        m_idx++;

        vkBuffer->lifetime().markUsed(m_queueType, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdSetIndexBuffer &c)
    {
        const auto vkBuffer = dynamic_cast<VkStagedBuffer*>(c.buffer.get());
        m_cmd.bindIndexBuffer(vkBuffer->handle(), 0, VkConvert::indexFormat(c.format));

        m_idx++;

        vkBuffer->lifetime().markUsed(m_queueType, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdSetScissor &c)
    {
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
        const auto texture = dynamic_cast<VkTexture*>(c.texture.get());
        const auto oldLayout = texture->getLayout();
        const auto oldStage  = texture->getStage();
        const auto oldAccess = texture->getAccess();

        const uint32_t mipLevels = texture->mipLevelCount();

        URHI_VALIDATE(c.texture != nullptr,
            "Texture must not be null");

        URHI_VALIDATE(mipLevels > 1,
            "Cannot generate mipmaps for a texture with only {} mip level",
            mipLevels);

        URHI_VALIDATE(texture->arrayLayerCount() > 0,
            "Cannot generate mipmaps for a texture with zero array layers");

        const vk::FormatProperties formatProperties = m_device->getPhysicalDevice().getFormatProperties(VkConvert::pixelFormat(texture->format(), m_device));
        URHI_VALIDATE(static_cast<bool>(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear), "Texture format is not blitable - generate mipmaps requires the texture format to be blitable");

        auto mipWidth = static_cast<int32_t>(texture->width(0));
        auto mipHeight = static_cast<int32_t>(texture->height(0));

        texture->transitionLayout(m_cmd,
            vk::ImageLayout::eTransferDstOptimal,
            vk::PipelineStageFlagBits2::eTransfer,
            vk::AccessFlagBits2::eTransferWrite);

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
                { vk::ImageAspectFlagBits::eColor, i - 1, 1, 0, texture->arrayLayerCount() }
            };

            m_cmd.pipelineBarrier2(vk::DependencyInfo{}.setImageMemoryBarriers(srcBarrier));

            vk::ImageBlit2 blit{};
            blit.srcOffsets = std::array{ vk::Offset3D{ 0, 0, 0 }, vk::Offset3D{ mipWidth, mipHeight, 1 } };
            blit.srcSubresource = vk::ImageSubresourceLayers
            {
                vk::ImageAspectFlagBits::eColor,
                i - 1,
                0,
                texture->arrayLayerCount()
            };

            blit.dstOffsets = std::array{ vk::Offset3D{ 0, 0, 0 }, vk::Offset3D{ mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 } };
            blit.dstSubresource = vk::ImageSubresourceLayers
            {
                vk::ImageAspectFlagBits::eColor,
                i,
                0,
                texture->arrayLayerCount()
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
            { vk::ImageAspectFlagBits::eColor, mipLevels - 1, 1, 0, texture->arrayLayerCount() }
        };

        m_cmd.pipelineBarrier2(vk::DependencyInfo{}.setImageMemoryBarriers(lastMipBarrier));

        texture->resetTrackedState(
            vk::ImageLayout::eTransferSrcOptimal,
            vk::PipelineStageFlagBits2::eTransfer,
            vk::AccessFlagBits2::eTransferRead);

        texture->transitionLayout(m_cmd, oldLayout, oldStage, oldAccess);

        m_idx++;

        texture->lifetime().markUsed(m_queueType, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdBlitTexture &c) const
    {
        const auto vkSrc = dynamic_cast<VkTexture*>(c.desc.src.get());
        const auto vkDst = dynamic_cast<VkTexture*>(c.desc.dst.get());

        const auto srcOldLayout = vkSrc->getLayout();
        const auto srcOldStage  = vkSrc->getStage();
        const auto srcOldAccess = vkSrc->getAccess();

        const auto dstOldLayout = vkDst->getLayout();
        const auto dstOldStage  = vkDst->getStage();
        const auto dstOldAccess = vkDst->getAccess();

        URHI_VALIDATE(c.desc.src != nullptr, "Source texture must not be null");
        URHI_VALIDATE(c.desc.dst != nullptr, "Destination texture must not be null");
        URHI_VALIDATE(c.desc.srcMipLevel < vkSrc->mipLevelCount(),
            "Source mip level {} out of range for texture with {} mip levels",
            c.desc.srcMipLevel, vkSrc->mipLevelCount());

        URHI_VALIDATE(c.desc.dstMipLevel < vkDst->mipLevelCount(),
            "Destination mip level {} out of range for texture with {} mip levels",
            c.desc.dstMipLevel, vkDst->mipLevelCount());

        URHI_VALIDATE(c.desc.srcArrayLayer < vkSrc->arrayLayerCount(),
            "Source array layer {} out of range for texture with {} layers",
            c.desc.srcArrayLayer, vkSrc->arrayLayerCount());

        URHI_VALIDATE(c.desc.dstArrayLayer < vkDst->arrayLayerCount(),
            "Destination array layer {} out of range for texture with {} layers",
            c.desc.dstArrayLayer, vkDst->arrayLayerCount());

        URHI_VALIDATE(
        static_cast<int32_t>(c.desc.srcOffset.x) >= 0 &&
        static_cast<int32_t>(c.desc.srcOffset.y) >= 0 &&
        static_cast<int32_t>(c.desc.srcOffset.z) >= 0,
        "Source offset must be non-negative");

        URHI_VALIDATE(
            static_cast<int32_t>(c.desc.dstOffset.x) >= 0 &&
            static_cast<int32_t>(c.desc.dstOffset.y) >= 0 &&
            static_cast<int32_t>(c.desc.dstOffset.z) >= 0,
            "Destination offset must be non-negative");

        URHI_VALIDATE(c.desc.src != c.desc.dst, "Source and destination textures are the same — blit requires distinct textures");

        vkSrc->transitionLayout(m_cmd,
            vk::ImageLayout::eTransferSrcOptimal,
            vk::PipelineStageFlagBits2::eTransfer,
            vk::AccessFlagBits2::eTransferRead);

        vkDst->transitionLayout(m_cmd,
            vk::ImageLayout::eTransferDstOptimal,
            vk::PipelineStageFlagBits2::eTransfer,
            vk::AccessFlagBits2::eTransferWrite);

        vk::ImageBlit2 blit{};

        const int32_t srcMaxX = c.desc.srcExtent.x != 0 ? c.desc.srcExtent.x + static_cast<int32_t>(c.desc.srcOffset.x) : static_cast<int32_t>(vkSrc->width(c.desc.srcMipLevel));
        const int32_t srcMaxY = c.desc.srcExtent.y != 0 ? c.desc.srcExtent.y + static_cast<int32_t>(c.desc.srcOffset.y) : static_cast<int32_t>(vkSrc->height(c.desc.srcMipLevel));
        const int32_t srcMaxZ = c.desc.srcExtent.z != 0 ? c.desc.srcExtent.z + static_cast<int32_t>(c.desc.srcOffset.z) : static_cast<int32_t>(vkSrc->depth(c.desc.srcMipLevel)) ;

        const int32_t dstMaxX = c.desc.dstExtent.x != 0 ? c.desc.dstExtent.x + static_cast<int32_t>(c.desc.dstOffset.x) : static_cast<int32_t>(vkDst->width(c.desc.dstMipLevel));
        const int32_t dstMaxY = c.desc.dstExtent.y != 0 ? c.desc.dstExtent.y + static_cast<int32_t>(c.desc.dstOffset.y) : static_cast<int32_t>(vkDst->height(c.desc.dstMipLevel));
        const int32_t dstMaxZ = c.desc.dstExtent.z != 0 ? c.desc.dstExtent.z + static_cast<int32_t>(c.desc.dstOffset.z) : static_cast<int32_t>(vkDst->depth(c.desc.dstMipLevel));

        URHI_VALIDATE(srcMaxX > static_cast<int32_t>(c.desc.srcOffset.x) &&
              srcMaxY > static_cast<int32_t>(c.desc.srcOffset.y) &&
              srcMaxZ > static_cast<int32_t>(c.desc.srcOffset.z),
            "Source blit region must have non-zero extent");

        URHI_VALIDATE(dstMaxX > static_cast<int32_t>(c.desc.dstOffset.x) &&
                      dstMaxY > static_cast<int32_t>(c.desc.dstOffset.y) &&
                      dstMaxZ > static_cast<int32_t>(c.desc.dstOffset.z),
            "Destination blit region must have non-zero extent");

        URHI_VALIDATE(srcMaxX <= static_cast<int32_t>(vkSrc->width(c.desc.srcMipLevel)) &&
                      srcMaxY <= static_cast<int32_t>(vkSrc->height(c.desc.srcMipLevel)) &&
                      srcMaxZ <= static_cast<int32_t>(vkSrc->depth(c.desc.srcMipLevel)),
            "Source blit region out of bounds for source mip {} (size: {}, {}, {})",
            c.desc.srcMipLevel,
            vkSrc->width(c.desc.srcMipLevel),
            vkSrc->height(c.desc.srcMipLevel),
            vkSrc->depth(c.desc.srcMipLevel));

        URHI_VALIDATE(dstMaxX <= static_cast<int32_t>(vkDst->width(c.desc.dstMipLevel)) &&
                      dstMaxY <= static_cast<int32_t>(vkDst->height(c.desc.dstMipLevel)) &&
                      dstMaxZ <= static_cast<int32_t>(vkDst->depth(c.desc.dstMipLevel)),
            "Destination blit region out of bounds for destination mip {} (size: {}, {}, {})",
            c.desc.dstMipLevel,
            vkDst->width(c.desc.dstMipLevel),
            vkDst->height(c.desc.dstMipLevel),
            vkDst->depth(c.desc.dstMipLevel));

        blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        blit.srcSubresource.mipLevel = c.desc.srcMipLevel;
        blit.srcSubresource.baseArrayLayer = c.desc.srcArrayLayer;
        blit.srcSubresource.layerCount = 1;
        blit.srcOffsets[0] = vk::Offset3D{static_cast<int32_t>(c.desc.srcOffset.x), static_cast<int32_t>(c.desc.srcOffset.y), static_cast<int32_t>(c.desc.srcOffset.z)};
        blit.srcOffsets[1] = vk::Offset3D{srcMaxX, srcMaxY, srcMaxZ};

        blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        blit.dstSubresource.mipLevel = c.desc.dstMipLevel;
        blit.dstSubresource.baseArrayLayer = c.desc.dstArrayLayer;
        blit.dstSubresource.layerCount = 1;
        blit.dstOffsets[0] = vk::Offset3D{static_cast<int32_t>(c.desc.dstOffset.x), static_cast<int32_t>(c.desc.dstOffset.y), static_cast<int32_t>(c.desc.dstOffset.z)};
        blit.dstOffsets[1] = vk::Offset3D{dstMaxX, dstMaxY, dstMaxZ};

        vk::BlitImageInfo2 blitInfo{};
        blitInfo.srcImage       = vkSrc->getHandle();
        blitInfo.srcImageLayout = vk::ImageLayout::eTransferSrcOptimal;
        blitInfo.dstImage       = vkDst->getHandle();
        blitInfo.dstImageLayout = vk::ImageLayout::eTransferDstOptimal;
        blitInfo.regionCount    = 1;
        blitInfo.pRegions       = &blit;
        blitInfo.filter         = VkConvert::filter(c.desc.filter);

        m_cmd.blitImage2(blitInfo);

        vkSrc->transitionLayout(m_cmd, srcOldLayout, srcOldStage, srcOldAccess);
        vkDst->transitionLayout(m_cmd, dstOldLayout, dstOldStage, dstOldAccess);

        vkSrc->lifetime().markUsed(m_queueType, m_submitValue);
        vkDst->lifetime().markUsed(m_queueType, m_submitValue);
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
        m_isRendering = false;
        m_cmd.endRendering();

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdBeginComputePass &c)
    {
        m_boundResources.clear();
        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdEndComputePass &c)
    {
        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdBeginCommandBuffer &c)
    {
        constexpr vk::CommandBufferBeginInfo info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        m_cmd.begin(info);

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdSetPipeline &c)
    {
        URHI_VALIDATE(c.pipeline != nullptr, "Pipeline must not be null");
        m_boundPipeline = std::dynamic_pointer_cast<VkPipeline>(c.pipeline);
        m_boundPipelineBindPoint = c.bindPoint;

        #if defined(URHI_ENABLE_VALIDATION)
            if(m_isRendering && c.bindPoint == vk::PipelineBindPoint::eGraphics)
            {
                const auto& pipelineDesc = dynamic_cast<VkGraphicsPipeline*>(m_boundPipeline.get())->m_desc;
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
                    const auto vkTex = dynamic_cast<VkTextureView*>(renderPassDesc.colorAttachments[i].target.get());

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
                    const auto vkTex = dynamic_cast<VkTextureView*>(renderPassDesc.depthAttachment->target.get());

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
            }
        #endif

        m_cmd.bindPipeline(c.bindPoint, m_boundPipeline->getHandle());
        m_idx++;
    }

    void VkCommandListEmitter::pushDescriptors()
    {
        URHI_VALIDATE(m_boundPipeline != nullptr, "No pipeline set - descriptors cannot be pushed before binding a pipeline");
        std::unordered_set<uint32_t> seenBindings;
        std::vector<vk::WriteDescriptorSet> writes;
        writes.reserve(m_boundResources.size());

        for(const auto& [stage, shader] : m_boundPipeline->m_shaderMap)
        {
            for (const auto& resource : shader->entryPoint().reflection.resources)
            {
                if (!seenBindings.insert(resource.binding).second) continue;

                auto it = m_boundResources.find(resource.name);
                URHI_VALIDATE(it != m_boundResources.end(), "Resource ({}) not bound - expected a resource to be bound to every shader resource ", resource.name);

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
            m_boundPipelineBindPoint,
            m_boundPipeline->getLayout(),
            0, // always 0 for now might change
            writes
        );
    }
}

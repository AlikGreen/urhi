#include "vkCommandListEmitter.h"

#include <numeric>
#include <ranges>
#include <unordered_set>

#include "clogr.h"
#include "vkCommandListTracker.h"
#include "vkConvert.h"
#include "vkDevice.h"
#include "vkMappedBuffer.h"
#include "vkPipeline.h"
#include "vkStagedBuffer.h"
#include "vkTextureView.h"
#include "vkSampler.h"

namespace urhi
{
    VkCommandListEmitter::VkCommandListEmitter(VkDevice* device, QueueType queueType, uint64_t submitValue, vk::Semaphore timeline, vk::CommandBuffer cmd, VkCommandListTracker tracker, grl::Rc<VkLinearStagingAllocator> stagingAllocator)
        : m_device(device), m_cmd(cmd), m_tracker(tracker), m_submitValue(submitValue), m_timeline(timeline), m_queueType(queueType), m_stagingAllocator(stagingAllocator) { }

    void VkCommandListEmitter::emit(const CmdDrawIndexed &c)
    {
        pushDescriptors();
        m_cmd.drawIndexed(c.indexCount, c.instanceCount, c.firstIndex, c.vertexOffset, c.firstInstance);

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdDraw &c)
    {
        pushDescriptors();
        m_cmd.draw(c.vertexCount, c.instanceCount, c.firstVertex, c.firstInstance);

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdBeginRenderPass &c)
    {
        for (auto& attachment : c.desc.colorAttachments)
        {
            const auto tex = dynamic_cast<VkTexture*>(dynamic_cast<VkTextureView*>(attachment.target.get())->texture().get());
            tex->transitionLayout(m_cmd, vk::ImageLayout::eColorAttachmentOptimal);
        }

        for (auto& [texture, useList] : m_tracker.m_textureUses)
        {
            for (const auto& use : useList)
            {
                if (use.commandIndex >= m_idx)
                {
                    const auto tex = dynamic_cast<VkTexture*>(texture);
                    tex->transitionLayout(m_cmd, use.requiredLayout);
                    break;
                }
            }
        }

        const auto& desc = c.desc;

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

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdReadbackTexture &c)
    {
        const auto desc = c.desc;

        const auto vkTex = dynamic_cast<VkTexture*>(desc.texture.get());
        const uint32_t width = std::min(vkTex->width(), desc.width);
        const uint32_t height = std::min(vkTex->height(), desc.height);
        const uint32_t depth = std::min(vkTex->depth(), desc.depth);

        vk::BufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = desc.mipLevel;
        region.imageSubresource.baseArrayLayer = desc.baseArrayLayer;
        region.imageSubresource.layerCount = desc.arrayLayerCount;
        region.imageOffset = vk::Offset3D{ desc.x, desc.y, desc.z };
        region.imageExtent = vk::Extent3D{ width, height, depth };

        const uint32_t size = width * height * depth * VkConvert::pixelFormatBytes(vkTex->format());

        // create buffer
        const VkBufferCreateInfo bufferInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size,
            .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT
        };

        constexpr VmaAllocationCreateInfo allocInfo = {
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                     VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO
        };

        VmaAllocation allocation;
        VmaAllocationInfo allocationResult;
        vk::Buffer buffer;

        vmaCreateBuffer(
            m_device->getAllocator(),
            &bufferInfo,
            &allocInfo,
            reinterpret_cast<::VkBuffer*>(&buffer),
            &allocation,
            &allocationResult
        );

        vkTex->transitionLayout(m_cmd, vk::ImageLayout::eTransferSrcOptimal);

        m_cmd.copyImageToBuffer(
        vkTex->getHandle(),
            vk::ImageLayout::eTransferSrcOptimal,
           buffer,
           1,
           &region
       );

        c.request->m_device = m_device;
        c.request->m_mapped = allocationResult.pMappedData;
        c.request->m_buffer = buffer;
        c.request->m_size = size;
        c.request->m_bufferAllocation = allocation;
        c.request->m_timeline = m_timeline;
        c.request->m_waitValue = m_submitValue;

        m_idx++;

        vkTex->lifetime().markUsed(m_queueType, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdUpdateBuffer &c)
    {
        if(const auto vkStaged = dynamic_cast<VkStagedBuffer*>(c.buffer.get()))
        {
            m_stagingAllocator->upload(c.data.data(), c.data.size(), vkStaged->handle(), 0, m_cmd);
            vkStaged->lifetime().markUsed(m_queueType, m_submitValue);
        }
        else if(const auto vkMapped = dynamic_cast<VkMappedBuffer*>(c.buffer.get()))
        {
            vkMapped->upload(c.data.data(), c.data.size());
            vkMapped->lifetime().markUsed(m_queueType, m_submitValue);
        }else
        {
            clogr::abort("Buffer was not a VkStagedBuffer or VkMappedBuffer");
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
        clogr::ensure(m_currentPipeline != nullptr, "No pipeline set");

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
        clogr::ensure(m_currentPipeline != nullptr, "No pipeline set");

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
        clogr::ensure(c.data.size() != 0, "No data uploaded");
        clogr::ensure(m_currentPipeline->m_pushConstantRange->size == c.data.size(), "Size of uploaded data doesnt match shader");
        m_cmd.pushConstants(m_currentPipeline->getLayout(), m_currentPipeline->m_pushConstantRange->stageFlags, m_currentPipeline->m_pushConstantRange->offset, c.data.size(), c.data.data());

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdSetTexture &c)
    {
        clogr::ensure(m_currentPipeline != nullptr, "Pipeline must be set before setting texture.");

        const auto vkView = dynamic_cast<VkTextureView*>(c.texture.get());

        m_boundResources[c.name] = BoundResource{
            .type = ShaderReflection::ResourceType::Texture,
            .imageInfo = { nullptr, vkView->getHandle(), vk::ImageLayout::eShaderReadOnlyOptimal }
        };

        m_idx++;

        vkView->lifetime().markUsed(m_queueType, m_submitValue);
    }

    void VkCommandListEmitter::emit(const CmdSetSampler &c)
    {
        clogr::ensure(m_currentPipeline != nullptr, "Pipeline must be set before setting sampler.");

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
        clogr::ensure(m_currentPipeline != nullptr, "Pipeline must be set before setting image.");

        const auto vkView = dynamic_cast<VkTextureView*>(c.texture.get());
        const auto vkTex  = dynamic_cast<VkTexture*>(vkView->texture().get());

        vkTex->transitionLayout(m_cmd, vk::ImageLayout::eGeneral);

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
        auto texture = dynamic_cast<VkTexture*>(c.texture.get());
        const uint32_t mipLevels = texture->mipLevelCount();

        const vk::FormatProperties formatProperties = m_device->getPhysicalDevice().getFormatProperties(VkConvert::pixelFormat(texture->format()));
        clogr::ensure(static_cast<bool>(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear), "Texture format is not blitable (so cannot generate mipmaps) if mipmaps are needed generate them manually");

        int32_t mipWidth = texture->width();
        int32_t mipHeight = texture->height();

        texture->transitionLayout(m_cmd, vk::ImageLayout::eTransferDstOptimal);

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

            vk::ImageMemoryBarrier2 dstBarrier{
                vk::PipelineStageFlagBits2::eTransfer,
                vk::AccessFlagBits2::eTransferWrite,
                vk::PipelineStageFlagBits2::eTransfer,
                vk::AccessFlagBits2::eTransferRead,
                vk::ImageLayout::eTransferSrcOptimal,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                VK_QUEUE_FAMILY_IGNORED,
                VK_QUEUE_FAMILY_IGNORED,
                texture->getHandle(),
                { vk::ImageAspectFlagBits::eColor, i - 1, 1, 0, texture->arrayLayerCount() }
            };

            m_cmd.pipelineBarrier2(vk::DependencyInfo{}.setImageMemoryBarriers(dstBarrier));
        }

        vk::ImageMemoryBarrier2 dstBarrier{
            vk::PipelineStageFlagBits2::eTransfer,
            vk::AccessFlagBits2::eTransferWrite,
            vk::PipelineStageFlagBits2::eTransfer,
            vk::AccessFlagBits2::eTransferRead,
            vk::ImageLayout::eTransferDstOptimal,
            vk::ImageLayout::eShaderReadOnlyOptimal,
            VK_QUEUE_FAMILY_IGNORED,
            VK_QUEUE_FAMILY_IGNORED,
            texture->getHandle(),
            { vk::ImageAspectFlagBits::eColor, mipLevels - 1, 1, 0, texture->arrayLayerCount() }
        };

        m_cmd.pipelineBarrier2(vk::DependencyInfo{}.setImageMemoryBarriers(dstBarrier));

        texture->m_currentLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdDispatchCompute &c)
    {
        pushDescriptors();
        m_cmd.dispatchBase(0, 0, 0, c.groupsX, c.groupsY, c.groupsZ);

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdEndRenderPass &c)
    {
        m_cmd.endRendering();

        m_idx++;
    }

    void VkCommandListEmitter::emit(const CmdBeginComputePass &c)
    {
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
        m_currentPipeline = std::dynamic_pointer_cast<VkPipeline>(c.pipeline);
        m_currentPipelineBindPoint = c.bindPoint;
        clogr::ensure(m_currentPipeline != nullptr, "Pipeline is not a graphics pipeline");
        m_cmd.bindPipeline(c.bindPoint, m_currentPipeline->getHandle());

        m_idx++;
    }

    void VkCommandListEmitter::pushDescriptors()
    {
        std::unordered_set<uint32_t> seenBindings;
        std::vector<vk::WriteDescriptorSet> writes;
        writes.reserve(m_boundResources.size());


        for(const auto& shader: m_currentPipeline->m_shaderMap | std::ranges::views::values)
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
            m_currentPipelineBindPoint,
            m_currentPipeline->getLayout(),
            0, // always 0 for now might change
            writes
        );

        m_boundResources.clear();
    }
}

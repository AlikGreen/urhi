#include "vkLinearStagingAllocator.h"

#include "clogr.h"
#include "vkConvert.h"
#include "vkDevice.h"
#include "vkTexture.h"


namespace urhi
{
    VkLinearStagingAllocator::VkLinearStagingAllocator(VkDevice *device)
        : m_device(device)
    {
        allocateNewPage(kDefaultBlockSize);
    }

    void VkLinearStagingAllocator::reset()
    {
        m_highWatermark = std::max(m_highWatermark, m_activePageIndex + 1);

        for (auto& page : m_pages)
        {
            page.offset = 0;
        }
        m_activePageIndex = 0;

        m_unusedFrames++;

        if (m_unusedFrames > kDecayFrames && m_pages.size() > m_highWatermark && m_pages.size() > 1)
        {
            vmaDestroyBuffer(
                m_device->getAllocator(),
                m_pages.back().buffer,
                m_pages.back().allocation
            );
            m_pages.pop_back();
            m_unusedFrames = 0;
            m_highWatermark = 1;
        }
    }

    void VkLinearStagingAllocator::upload(const void *srcData, const size_t size, const vk::Buffer dstBuffer, const size_t dstOffset, const vk::CommandBuffer cmd)
    {
        const StagingAllocation allocation = allocate(size);

        std::memcpy(allocation.mapped, srcData, size);

        vmaFlushAllocation(
           m_device->getAllocator(),
           m_pages[allocation.pageIndex].allocation,
           allocation.offset,
           size
        );

        const vk::BufferCopy region{
            allocation.offset,
            dstOffset,
            size
        };

        cmd.copyBuffer(allocation.buffer, dstBuffer, 1, &region);
    }

    void VkLinearStagingAllocator::uploadToImage(
        const TextureUploadDesc& uploadDesc,
        VkTexture* texture,
        const vk::CommandBuffer cmd)
    {
        clogr::ensure(uploadDesc.data != nullptr, "Trying to upload nullptr data to texture.");
        const uint32_t size = uploadDesc.width*uploadDesc.height*uploadDesc.depth*VkConvert::pixelFormatBytes(texture->getFormat());
        const StagingAllocation allocation = allocate(size);

        std::memcpy(allocation.mapped, uploadDesc.data, size);

        vmaFlushAllocation(
            m_device->getAllocator(),
            m_pages[allocation.pageIndex].allocation,
            allocation.offset,
            size
        );

        texture->transitionLayout(cmd, vk::ImageLayout::eTransferDstOptimal);

        vk::BufferImageCopy region;
        region.bufferOffset = allocation.offset;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = uploadDesc.mipLevel;
        region.imageSubresource.baseArrayLayer = uploadDesc.baseArrayLayer;
        region.imageSubresource.layerCount = uploadDesc.layerCount;

        region.imageOffset = vk::Offset3D{ uploadDesc.x, uploadDesc.y, uploadDesc.z };
        region.imageExtent = vk::Extent3D{ uploadDesc.width, uploadDesc.height, uploadDesc.depth };

        cmd.copyBufferToImage(
            m_pages[allocation.pageIndex].buffer,
            texture->getHandle(),
            vk::ImageLayout::eTransferDstOptimal,
            1,
            &region
        );

        texture->transitionLayout(cmd, vk::ImageLayout::eShaderReadOnlyOptimal);
    }


    VkLinearStagingAllocator::StagingAllocation VkLinearStagingAllocator::allocate(const size_t size)
    {
        StagingAllocation allocation;
        StagingPage& page = m_pages[m_activePageIndex];
        const size_t aligned = (page.offset + kAlignment - 1) & ~(kAlignment - 1);

        if (aligned + size <= page.capacity)
        {
            allocation.buffer = page.buffer;
            allocation.offset = aligned;
            allocation.mapped = page.mappedData + aligned;
            allocation.pageIndex = m_activePageIndex;
            page.offset = aligned + size;

            return allocation;
        }

        const size_t newPageSize = std::max(kDefaultBlockSize, size);
        allocateNewPage(newPageSize);
        m_activePageIndex++;

        page = m_pages[m_activePageIndex];
        allocation.buffer = page.buffer;
        allocation.offset = 0;
        allocation.mapped = page.mappedData;
        allocation.pageIndex = m_activePageIndex;

        return allocation;
    }

    void VkLinearStagingAllocator::allocateNewPage(const size_t capacity)
    {
        const VkBufferCreateInfo bufferInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = capacity,
            .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        };

        constexpr VmaAllocationCreateInfo allocInfo = {
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                     VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO
        };

        VmaAllocationInfo allocResult;
        VkBuffer rawBuffer;
        StagingPage page;

        vmaCreateBuffer(
            m_device->getAllocator(),
            &bufferInfo,
            &allocInfo,
            &rawBuffer,
            &page.allocation,
            &allocResult
        );

        page.buffer = vk::Buffer(rawBuffer);
        page.mappedData = static_cast<uint8_t*>(allocResult.pMappedData);
        page.capacity = capacity;
        page.offset = 0;

        m_pages.push_back(page);

        if(m_pages.size() > 16)
            clogr::warn("Over 16 pages in VkLinearStagingAllocator could be a memory leak");
    }

}

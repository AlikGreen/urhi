#pragma once

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include "descriptions/textureUploadDesc.h"

namespace urhi
{
class VkCommandList;
class VkDevice;
class VkCommandList;
class VkTexture;

class VkLinearStagingAllocator
{
public:
    VkLinearStagingAllocator() = default;
    void init(VkDevice* device);
    void destroy();

    VkLinearStagingAllocator(const VkLinearStagingAllocator&) = delete;
    VkLinearStagingAllocator& operator=(const VkLinearStagingAllocator&) = delete;

    void reset();
    void upload(const void* srcData, size_t size, vk::Buffer dstBuffer, size_t dstOffset, vk::CommandBuffer cmd);

    void uploadToImage(const TextureUploadDesc &uploadDesc, vk::CommandBuffer cmd);
private:
    static constexpr size_t kDefaultBlockSize = 64 * 1024 * 1024;
    static constexpr size_t kAlignment = 256;
    static constexpr size_t kDecayFrames = 16;

    struct StagingPage
    {
        vk::Buffer buffer;
        VmaAllocation allocation;
        uint8_t* mappedData = nullptr;
        size_t capacity = 0;
        size_t offset = 0;
    };

    struct StagingAllocation
    {
        void* mapped;
        vk::Buffer buffer;
        uint64_t offset;
        size_t pageIndex;
    };

    StagingAllocation allocate(size_t size);
    void allocateNewPage(size_t capacity);

    size_t m_highWatermark = 1;
    size_t m_unusedFrames = 0;

    std::vector<StagingPage> m_pages;
    size_t m_activePageIndex = 0;

    VkDevice* m_device;
};
}

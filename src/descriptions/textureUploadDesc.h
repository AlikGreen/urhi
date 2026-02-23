#pragma once

namespace urhi
{
    struct TextureUploadDesc
    {
        const void* data = nullptr;

        int32_t x = 0;
        int32_t y = 0;
        int32_t z = 0;
        uint32_t width  = 1;
        uint32_t height = 1;
        uint32_t depth  = 1;

        uint32_t baseArrayLayer  = 0;
        uint32_t layerCount = 1;

        uint32_t mipLevel = 0;
    };
}

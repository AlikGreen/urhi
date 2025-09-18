#pragma once

namespace Neon::RHI
{
    enum class TextureFormat
    {
        R8Unorm,
        R8Snorm,
        R8Uint,
        R8Int,

        R16Unorm,
        R16Snorm,
        R16Uint,
        R16Int,
        R16Float,

        R8G8Unorm,
        R8G8Snorm,
        R8G8Uint,
        R8G8Int,

        R16G16Unorm,
        R16G16Snorm,
        R16G16Uint,
        R16G16Int,
        R16G16Float,

        R32G32Float,
        R32G32Uint,
        R32G32Int,

        R32Float,
        R32Uint,
        R32Int,

        R8G8B8Unorm,
        R8G8B8Snorm,
        R8G8B8Uint,
        R8G8B8Int,
        R8G8B8UnormSrgb,
        B8G8R8Unorm,
        B8G8R8UnormSrgb,

        R16G16B16Unorm,
        R16G16B16Snorm,
        R16G16B16Uint,
        R16G16B16Int,
        R16G16B16Float,

        R32G32B32Float,
        R32G32B32Uint,
        R32G32B32Int,

        R8G8B8A8Unorm,
        R8G8B8A8Snorm,
        R8G8B8A8Uint,
        R8G8B8A8Int,
        R8G8B8A8UnormSrgb,
        B8G8R8A8Unorm,
        B8G8R8A8UnormSrgb,

        R16G16B16A16Unorm,
        R16G16B16A16Snorm,
        R16G16B16A16Uint,
        R16G16B16A16Int,
        R16G16B16A16Float,

        R32G32B32A32Float,
        R32G32B32A32Uint,
        R32G32B32A32Int,

        R10G10B10A2Unorm,
        R11G11B10Ufloat,

        BC1RgbaUnorm,
        BC1RgbaUnormSrgb,
        BC2RgbaUnorm,
        BC2RgbaUnormSrgb,
        BC3RgbaUnorm,
        BC3RgbaUnormSrgb,
        BC4RUnorm,
        BC5RgUnorm,
        BC7RgbaUnorm,
        BC7RgbaUnormSrgb,

        D24UnormS8Uint,
        D32FloatS8Uint,

        Invalid,
    };
}

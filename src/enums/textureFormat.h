#pragma once

namespace NRHI
{
enum class TextureFormat
{
    // 8‑bit normalized / integer
    R8Unorm,
    R8Snorm,
    R8Uint,
    R8Int,

    // 16‑bit normalized / integer / float
    R16Unorm,
    R16Snorm,
    R16Uint,
    R16Int,
    R16Float,

    // 8‑bit two‑channel
    R8G8Unorm,
    R8G8Snorm,
    R8G8Uint,
    R8G8Int,

    // 16‑bit two‑channel
    R16G16Unorm,
    R16G16Snorm,
    R16G16Uint,
    R16G16Int,
    R16G16Float,

    // 32‑bit two‑channel
    R32G32Float,
    R32G32Uint,
    R32G32Int,

    // 32‑bit single‑channel
    R32Float,
    R32Uint,
    R32Int,

    // 8‑bit four‑channel
    R8G8B8A8Unorm,
    R8G8B8A8Snorm,
    R8G8B8A8Uint,
    R8G8B8A8Int,
    R8G8B8A8UnormSrgb,
    B8G8R8A8Unorm,
    B8G8R8A8UnormSrgb,

    // 16‑bit four‑channel
    R16G16B16A16Unorm,
    R16G16B16A16Snorm,
    R16G16B16A16Uint,
    R16G16B16A16Int,
    R16G16B16A16Float,

    // 32‑bit four‑channel
    R32G32B32A32Float,
    R32G32B32A32Uint,
    R32G32B32A32Int,

    // Packed / special
    R10G10B10A2Unorm,
    R11G11B10Ufloat,

    // Block‑compressed
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

    // Depth‑stencil
    D24UnormS8Uint,
    D32FloatS8Uint,

    Invalid,
};
}

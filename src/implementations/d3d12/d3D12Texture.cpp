#include "d3D12Texture.h"

#include "d3D12Convert.h"

namespace urhi
{
    D3D12Texture::D3D12Texture(D3D12Device* device, const TextureDesc &desc)
        : m_device(device), m_width(desc.width), m_depth(desc.depth), m_height(desc.height), m_format(desc.format), m_type(desc.type)
    {
        m_mipLevels = std::min(desc.maxMipLevels, static_cast<uint32_t>(std::floor(std::log2(std::max(m_width, m_height)))) + 1);

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension          = D3D12Convert::resourceDimension(desc.type);
        resourceDesc.Alignment          = 0;
        resourceDesc.Width              = m_width;
        resourceDesc.Height             = m_height;
        resourceDesc.DepthOrArraySize   = m_depth;
        resourceDesc.MipLevels          = m_mipLevels;
        resourceDesc.Format             = D3D12Convert::format(desc.format);
        resourceDesc.SampleDesc.Count   = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resourceDesc.Flags              = D3D12_RESOURCE_FLAG_NONE;

        D3D12MA::ALLOCATION_DESC allocationDesc = {};
        allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;


        const auto hr = device->allocator()->CreateResource(
            &allocationDesc,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            &m_allocation,
            IID_PPV_ARGS(&m_resource)
        );

        URHI_VALIDATE(hr, "Failed to create texture");
    }

    D3D12Texture::~D3D12Texture()
    {
        m_device->destroy(m_life,
        [resource = m_resource, alloc = m_allocation](D3D12Device*)
        {
            resource->Release();
            alloc->Release();
        });
    }
}

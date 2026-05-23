#include "D3D12TextureView.h"

namespace urhi
{
    D3D12TextureView::D3D12TextureView(D3D12Device *device, const TextureViewDesc &desc)
        : m_firstMip(desc.firstMip), m_mipCount(desc.mipCount),
            m_firstLayer(desc.layerCount), m_layerCount(desc.layerCount),
            m_format(desc.format),
            m_texture(std::static_pointer_cast<D3D12Texture>(desc.texture))
    {
        if(m_format == PixelFormat::Unknown)
            m_format = m_texture->format();

        D3D12_SHADER_RESOURCE_VIEW_DESC srv = {};

        switch (m_texture->type())
        {
            case TextureType::Texture1D:
                srv.ViewDimension             = D3D12_SRV_DIMENSION_TEXTURE1D;
                srv.Texture1D.MostDetailedMip = desc.firstMip;
                srv.Texture1D.MipLevels       = desc.mipCount;
                break;
            case TextureType::Texture2D:
                srv.ViewDimension             = D3D12_SRV_DIMENSION_TEXTURE2D;
                srv.Texture2D.MostDetailedMip = desc.firstMip;
                srv.Texture2D.MipLevels       = desc.mipCount;
                break;
            case TextureType::Texture2DArray:
                srv.ViewDimension                  = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
                srv.Texture2DArray.MostDetailedMip = desc.firstMip;
                srv.Texture2DArray.MipLevels       = desc.mipCount;
                srv.Texture2DArray.FirstArraySlice = desc.firstLayer;
                srv.Texture2DArray.ArraySize       = desc.layerCount;
                break;
            case TextureType::TextureCube:
                srv.ViewDimension               = D3D12_SRV_DIMENSION_TEXTURECUBE;
                srv.TextureCube.MostDetailedMip = desc.firstMip;
                srv.TextureCube.MipLevels       = desc.mipCount;
                break;
            case TextureType::TextureCubeArray:
                srv.ViewDimension                     = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
                srv.TextureCubeArray.MostDetailedMip  = desc.firstMip;
                srv.TextureCubeArray.MipLevels        = desc.mipCount;
                srv.TextureCubeArray.First2DArrayFace = desc.firstLayer;
                srv.TextureCubeArray.NumCubes         = desc.layerCount;
                break;
            case TextureType::Texture3D:
                srv.ViewDimension             = D3D12_SRV_DIMENSION_TEXTURE3D;
                srv.Texture3D.MostDetailedMip = desc.firstMip;
                srv.Texture3D.MipLevels       = desc.mipCount;
                break;
            default:
                URHI_VALIDATE(false, "Texture type not supported to create texture view from");
        }
    }

    D3D12TextureView::~D3D12TextureView()
    {
    }
}

#include "d3D12Device.h"


namespace urhi
{
    D3D12Device::D3D12Device(const DeviceDesc &desc)
    {
        auto hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
        URHI_VALIDATE(SUCCEEDED(hr), "Unable to create D3D12 device");


    }
}

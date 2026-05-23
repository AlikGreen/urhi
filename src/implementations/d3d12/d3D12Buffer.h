#pragma once
#include "buffer.h"
#include "d3D12Helper.h"
#include "d3D12Lifetime.h"
#include "enums/queueType.h"

namespace urhi
{
    class D3D12Device;

    class D3D12Buffer : public Buffer
{
public:
    D3D12Buffer(D3D12Device* device, const uint64_t size) : m_device(device), m_size(size) { }
    ~D3D12Buffer() override = default;
    [[nodiscard]] uint64_t size() const override { return m_size; };
    D3D12Lifetime& life() { return m_life; }
protected:
    D3D12Device* m_device;
    uint64_t m_size;

    ID3D12Resource* m_buffer{};
    D3D12MA::Allocation* m_allocation{};

    D3D12Lifetime m_life{};
};
}

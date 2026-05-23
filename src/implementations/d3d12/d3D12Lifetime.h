#pragma once
#include <cstdint>


namespace urhi
{
class D3D12CommandQueue;

struct D3D12Lifetime
{
    uint64_t lastSubmit = 0;
    D3D12CommandQueue* lastQueue = nullptr;

    void use(D3D12CommandQueue* queue, const uint64_t submitValue)
    {
        lastQueue = queue;
        lastSubmit = submitValue;
    }
};
}

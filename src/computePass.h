#pragma once
#include <cstdint>

namespace urhi
{
class ComputePass
{
public:
    ComputePass() = default;
    virtual ~ComputePass() = default;

    ComputePass(const ComputePass&) = delete;
    ComputePass& operator= (const ComputePass&) = delete;

    virtual void dispatch(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ) = 0;
};
}

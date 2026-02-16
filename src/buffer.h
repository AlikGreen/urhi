#pragma once

namespace urhi
{
class Buffer
{
public:
    Buffer() = default;
    virtual ~Buffer() = default;

    Buffer(const Buffer&) = delete;
    Buffer& operator= (const Buffer&) = delete;
};
}

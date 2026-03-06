#pragma once
#include <cstdint>

namespace urhi
{
class Buffer
{
public:
    Buffer() = default;
    virtual ~Buffer() = default;

    [[nodiscard]] virtual uint64_t size() const = 0;

    Buffer(const Buffer&) = delete;
    Buffer& operator= (const Buffer&) = delete;
};
}

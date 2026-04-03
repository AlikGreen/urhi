#pragma once
#include <cstdint>
#include <span>

namespace urhi
{
class ReadbackRequest
{
public:
    ReadbackRequest() = default;
    virtual ~ReadbackRequest() = default;

    ReadbackRequest(const ReadbackRequest&) = delete;
    ReadbackRequest& operator= (const ReadbackRequest&) = delete;

    [[nodiscard]] virtual bool isReady() const = 0;
    virtual void wait() const = 0;

    [[nodiscard]] virtual const void* data() const = 0;
    [[nodiscard]] virtual size_t size() const = 0;

    template<typename T>
    std::span<const T> as() const
    {
        return { static_cast<const T*>(data()), size() / sizeof(T) };
    }

    template<typename T>
    T at(const size_t offset) const
    {
        return *reinterpret_cast<const T*>(static_cast<const uint8_t*>(data()) + offset);
    }
};
}

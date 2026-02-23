#pragma once

namespace urhi
{
class Sampler
{
public:
    Sampler() = default;
    virtual ~Sampler() = default;

    Sampler(const Sampler&) = delete;
    Sampler& operator= (const Sampler&) = delete;
};
}

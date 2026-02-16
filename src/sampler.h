#pragma once

namespace urhi
{
class Sampler
{
public:
    virtual ~Sampler() = default;

    Sampler(const Sampler&) = delete;
    Sampler& operator= (const Sampler&) = delete;
};
}

#pragma once

namespace urhi
{
class Pipeline
{
public:
    Pipeline() = default;
    virtual ~Pipeline() = default;

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator= (const Pipeline&) = delete;
};
}

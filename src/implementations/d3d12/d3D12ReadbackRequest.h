#pragma once
#include "readbackRequest.h"

namespace urhi
{
class D3D12ReadbackRequest final : public ReadbackRequest
{
public:
    [[nodiscard]] bool isReady() const override;

    void wait() const override;

    [[nodiscard]] const void* data() const override;
    [[nodiscard]] size_t size() const override;
};
}

#pragma once
#include <cstdint>

#include "vkCommandQueue.h"

namespace urhi
{
class VkDevice;
struct VkLifetime
{
    uint64_t lastSubmitValue = 0;
    VkCommandQueue* lastSubmitQueue = nullptr;

    void markUsed(VkCommandQueue* q, const uint64_t value)
    {
        lastSubmitQueue = q;
        if (value > lastSubmitValue) lastSubmitValue = value;
    }
};
}

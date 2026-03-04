#pragma once
#include <cstdint>

#include "enums/queueType.h"

namespace urhi
{
class VkDevice;
struct VkLifetime
{
    uint64_t lastSubmitValue = 0;
    QueueType lastSubmitQueue = QueueType::Graphics;

    void markUsed(const QueueType q, const uint64_t value)
    {
        lastSubmitQueue = q;
        if (value > lastSubmitValue) lastSubmitValue = value;
    }
};
}

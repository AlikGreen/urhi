#pragma once
#include "enums/bufferUsage.h"
#include <cstdint>

namespace urhi
{
struct BufferDesc
{
    BufferDesc(const BufferUsage usage, const size_t size)
        : usage(usage), size(size) { }

    BufferUsage usage;
    size_t size; // size in bytes
};
}

#pragma once
#include "enums/bufferUsage.h"
#include <cstdint>

namespace urhi
{
struct BufferDesc
{
    BufferDesc(const size_t size, const BufferUsage usage)
        : size(size), usage(usage) { }

    size_t size; // size in bytes
    BufferUsage usage;
};
}

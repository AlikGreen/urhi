#pragma once

#include <stdint.h>

namespace urhi
{
    struct DrawIndexedIndirectCommand
    {
        uint32_t indexCount;
        uint32_t instanceCount;
        uint32_t firstIndex;
        int32_t  baseVertex;
        uint32_t baseInstance;
    };

    struct DrawIndirectCommand
    {
        uint32_t vertexCount;
        uint32_t instanceCount;
        int32_t  baseVertex;
        uint32_t baseInstance;
    };
}

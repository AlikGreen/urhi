#pragma once
#include <grl/mem.h>

#include "buffer.h"

namespace urhi
{
struct BufferReadbackDesc
{
    grl::Rc<Buffer> buffer;
    size_t offset = 0;
    size_t size = ~0;
};
}

#pragma once

namespace NRHI
{
struct VertexBufferBinding
{
    Ref<Buffer> vertexBuffer{};
    int offset{};
};
}

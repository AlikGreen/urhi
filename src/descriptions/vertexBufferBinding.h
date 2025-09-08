#pragma once

namespace Neon::RHI
{
struct VertexBufferBinding
{
    Ref<Buffer> vertexBuffer{};
    int offset{};
};
}

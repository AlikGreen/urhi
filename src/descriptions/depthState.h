#pragma once

namespace Neon::RHI
{
struct DepthState
{
    bool hasDepthTarget = false;
    bool enableDepthTest = false;
    bool enableDepthWrite = false;
};
}

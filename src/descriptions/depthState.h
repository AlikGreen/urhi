#pragma once
#include "enums/compareOp.h"

namespace urhi
{
struct DepthState
{
    bool hasDepthTarget = false;
    bool enableDepthTest = false;
    bool enableDepthWrite = false;
    CompareOp compareOp = CompareOp::Less;
};
}

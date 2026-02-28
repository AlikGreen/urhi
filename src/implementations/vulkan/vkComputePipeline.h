#pragma once
#include "vkPipeline.h"

namespace urhi
{
class VkComputePipeline final : public VkPipeline
{
public:
    VkComputePipeline(VkDevice *device, const ComputePipelineDesc& desc);
private:

};
}

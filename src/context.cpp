#include "context.h"

#include "implementations/d3d12/d3D12Context.h"
#include "implementations/vulkan/vkContext.h"

namespace urhi
{
    grl::Rc<Context> Context::create(const ContextDesc& desc)
    {
        switch (desc.api)
        {
            case BackendAPI::Vulkan:
                return grl::makeRc<VkContext>(desc);
            case BackendAPI::D3D12:
                return grl::makeRc<D3D12Context>(desc);
            default:
                return nullptr;
        }
    }
}

#include "context.h"

#include "implementations/vulkan/vkContext.h"

namespace urhi
{
    grl::Rc<Context> Context::create(const BackendAPI api)
    {
        switch (api)
        {
            case BackendAPI::Vulkan:
                return grl::makeRc<VkContext>();
            default:
                return nullptr;
        }
    }
}

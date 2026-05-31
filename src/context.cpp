#include "context.h"

#include "implementations/opengl/glContext.h"
#include "implementations/vulkan/vkContext.h"

namespace urhi
{
    grl::Rc<Context> Context::create(const ContextDesc& desc)
    {
        switch (desc.api)
        {
            case BackendAPI::Vulkan:
                return grl::makeRc<VkContext>(desc);
            case BackendAPI::OpenGL:
                return grl::makeRc<GlContext>(desc);
            default:
                return nullptr;
        }
    }
}

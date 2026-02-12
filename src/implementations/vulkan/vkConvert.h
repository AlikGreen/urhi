#pragma once
#include <vulkan/vulkan_core.h>

#include "enums/colorSpace.h"
#include "enums/presentMode.h"
#include "input/keyCodes.h"

namespace urhi
{
class VkConvert
{
public:
    static KeyCode     keyCode(int glfwKey);
    static KeyMod      keyMod(int glfwMods);
    static MouseButton mouseButton(int glfwButton);

    static VkPresentModeKHR presentMode(PresentMode presentMode);
    static VkColorSpaceKHR colorSpace(ColorSpace colorSpace);
};
}

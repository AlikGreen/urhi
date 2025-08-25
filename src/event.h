#pragma once
#include "input/keyCodes.h"

namespace NRHI
{
    class Event
    {
    public:
        enum Type
        {
            Quit,
            KeyDown,
            KeyUp,
            MouseButtonDown,
            MouseButtonUp,
            MouseMotion,
            MouseWheel,
            WindowResize,
            TextInput
        };

        struct KeyEvent
        {
            KeyCode key;
            bool repeat;
        };

        struct MouseButtonEvent
        {
            MouseButton button;
            int x, y;
            int clicks;
        };

        struct MouseMotionEvent
        {
            float x, y;
        };

        struct MouseWheelEvent
        {
            int x, y;
        };

        struct WindowResizeEvent
        {
            int width, height;
        };

        struct TextInputEvent
        {
            char text[32];
        };

        Type type;

        union
        {
            KeyEvent key;
            MouseButtonEvent button;
            MouseMotionEvent motion;
            MouseWheelEvent wheel;
            WindowResizeEvent window;
            TextInputEvent text;
        };
    };
}

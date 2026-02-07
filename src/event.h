#pragma once
#include "input/keyCodes.h"
#include <string>
#include <variant>

namespace urhi
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
            TextInput,
            DropFile
        };

        struct KeyEvent { KeyCode key; bool repeat; };
        struct MouseButtonEvent { MouseButton button; int x, y; int clicks; };
        struct MouseMotionEvent { float x, y; };
        struct MouseWheelEvent { int x, y; };
        struct WindowResizeEvent { int width, height; };
        struct TextInputEvent { uint32_t codepoint; };
        struct DropFileEvent { std::string path; };

        using Data = std::variant<
            std::monostate,
            KeyEvent,
            MouseButtonEvent,
            MouseMotionEvent,
            MouseWheelEvent,
            WindowResizeEvent,
            TextInputEvent,
            DropFileEvent
        >;

        Type type = Quit;
        Data data;
    };
}

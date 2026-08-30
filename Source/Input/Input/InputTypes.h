#pragma once
#include <Base/Types.h>

#include <limits>

enum class InputDevice : u8
{
    None,
    Keyboard,
    Mouse,
    MouseWheel
};

enum class PointerSource : u8
{
    None,
    Mouse,
    Pen,
    Touch
};

enum class Key : u16
{
    Unknown = 0,
    Space = 32,
    Apostrophe = 39,
    Comma = 44,
    Minus = 45,
    Period = 46,
    Slash = 47,
    Num0 = 48,
    Num1 = 49,
    Num2 = 50,
    Num3 = 51,
    Num4 = 52,
    Num5 = 53,
    Num6 = 54,
    Num7 = 55,
    Num8 = 56,
    Num9 = 57,
    Semicolon = 59,
    Equal = 61,
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    LeftBracket = 91,
    Backslash = 92,
    RightBracket = 93,
    GraveAccent = 96,
    World1 = 161,
    World2 = 162,
    Escape = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,
    Insert = 260,
    Delete = 261,
    Right = 262,
    Left = 263,
    Down = 264,
    Up = 265,
    PageUp = 266,
    PageDown = 267,
    Home = 268,
    End = 269,
    CapsLock = 280,
    ScrollLock = 281,
    NumLock = 282,
    PrintScreen = 283,
    Pause = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    F13 = 302,
    F14 = 303,
    F15 = 304,
    F16 = 305,
    F17 = 306,
    F18 = 307,
    F19 = 308,
    F20 = 309,
    F21 = 310,
    F22 = 311,
    F23 = 312,
    F24 = 313,
    F25 = 314,
    Keypad0 = 320,
    Keypad1 = 321,
    Keypad2 = 322,
    Keypad3 = 323,
    Keypad4 = 324,
    Keypad5 = 325,
    Keypad6 = 326,
    Keypad7 = 327,
    Keypad8 = 328,
    Keypad9 = 329,
    KeypadDecimal = 330,
    KeypadDivide = 331,
    KeypadMultiply = 332,
    KeypadSubtract = 333,
    KeypadAdd = 334,
    KeypadEnter = 335,
    KeypadEqual = 336,
    LeftShift = 340,
    LeftControl = 341,
    LeftAlt = 342,
    LeftSuper = 343,
    RightShift = 344,
    RightControl = 345,
    RightAlt = 346,
    RightSuper = 347,
    Menu = 348
};

enum class MouseButton : u8
{
    Left,
    Right,
    Middle,
    Button4,
    Button5,
    Button6,
    Button7,
    Button8
};

enum class MouseWheelDirection : u8
{
    Up,
    Down,
    Left,
    Right
};

inline constexpr u32 INPUT_KEY_COUNT = static_cast<u32>(Key::Menu) + 1;
inline constexpr u32 INPUT_MOUSE_BUTTON_COUNT = static_cast<u32>(MouseButton::Button8) + 1;
inline constexpr u32 INPUT_MOUSE_WHEEL_DIRECTION_COUNT = static_cast<u32>(MouseWheelDirection::Right) + 1;
inline constexpr u32 INPUT_CONTROL_COUNT = INPUT_KEY_COUNT + INPUT_MOUSE_BUTTON_COUNT + INPUT_MOUSE_WHEEL_DIRECTION_COUNT;

enum class InputModifier : u32
{
    None = 0,
    Shift = 1 << 0,
    Control = 1 << 1,
    Alt = 1 << 2,
    Super = 1 << 3
};
DECLARE_GENERIC_BITWISE_OPERATORS(InputModifier);

enum class CursorMode : u8
{
    Hardware,
    Software,
    Captured
};

enum class ModifierMatch : u8
{
    Exact,
    AtLeast,
    Any
};

enum class InputEventType : u8
{
    Button,
    CursorMove,
    Scroll,
    Text,
    FocusChanged
};

enum class InputPhase : u8
{
    None,
    Pressed,
    Repeated,
    Released,
    Canceled,
    Triggered
};

enum class InputReply : u8
{
    Ignored,
    Handled,
    Consumed
};

struct InputControl
{
public:
    static InputControl Keyboard(Key key)
    {
        return { InputDevice::Keyboard, static_cast<u16>(key) };
    }

    static InputControl Mouse(MouseButton button)
    {
        return { InputDevice::Mouse, static_cast<u16>(button) };
    }

    static InputControl MouseWheel(MouseWheelDirection direction)
    {
        return { InputDevice::MouseWheel, static_cast<u16>(direction) };
    }

    bool IsValid() const
    {
        return device != InputDevice::None;
    }

    bool operator==(const InputControl&) const = default;

public:
    InputDevice device = InputDevice::None;
    u16 code = 0;
};

inline constexpr u32 GetInputControlIndex(InputControl control)
{
    if (control.device == InputDevice::Keyboard && control.code < INPUT_KEY_COUNT)
        return control.code;

    if (control.device == InputDevice::Mouse && control.code < INPUT_MOUSE_BUTTON_COUNT)
        return INPUT_KEY_COUNT + control.code;

    if (control.device == InputDevice::MouseWheel && control.code < INPUT_MOUSE_WHEEL_DIRECTION_COUNT)
        return INPUT_KEY_COUNT + INPUT_MOUSE_BUTTON_COUNT + control.code;

    return INPUT_CONTROL_COUNT;
}

struct InputBinding
{
public:
    static InputBinding Keyboard(Key key, InputModifier modifiers = InputModifier::None, ModifierMatch modifierMatch = ModifierMatch::Exact)
    {
        return { InputControl::Keyboard(key), modifiers, modifierMatch };
    }

    static InputBinding Mouse(MouseButton button, InputModifier modifiers = InputModifier::None, ModifierMatch modifierMatch = ModifierMatch::Exact)
    {
        return { InputControl::Mouse(button), modifiers, modifierMatch };
    }

    static InputBinding MouseWheel(MouseWheelDirection direction, InputModifier modifiers = InputModifier::None, ModifierMatch modifierMatch = ModifierMatch::Exact)
    {
        return { InputControl::MouseWheel(direction), modifiers, modifierMatch };
    }

    bool Matches(InputControl eventControl, InputModifier eventModifiers) const
    {
        if (control != eventControl)
            return false;

        if (modifierMatch == ModifierMatch::Any)
            return true;

        if (modifierMatch == ModifierMatch::Exact)
            return modifiers == eventModifiers;

        return (eventModifiers & modifiers) == modifiers;
    }

    bool operator==(const InputBinding&) const = default;

public:
    InputControl control;
    InputModifier modifiers = InputModifier::None;
    ModifierMatch modifierMatch = ModifierMatch::Exact;
};

struct InputEvent
{
public:
    InputEventType type = InputEventType::Button;
    InputControl control;
    InputPhase phase = InputPhase::None;
    InputModifier modifiers = InputModifier::None;
    PointerSource pointerSource = PointerSource::None;
    vec2 position = vec2(0.0f);
    vec2 delta = vec2(0.0f);
    u32 codepoint = 0;
    bool focused = true;
};

struct PenState
{
public:
    f32 pressure = 0.0f;
    vec2 delta = vec2(0.0f);
    bool inRange = false;
    bool inContact = false;
};

struct InputContextHandle
{
public:
    bool IsValid() const
    {
        return index != std::numeric_limits<u8>::max();
    }

    bool operator==(const InputContextHandle&) const = default;

public:
    u8 index = std::numeric_limits<u8>::max();
    u16 generation = 0;
};

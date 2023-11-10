#pragma once
#include <Base/Types.h>

#include <Base/Util/StringUtils.h>

#include <robinhood/robinhood.h>

struct GLFWwindow;
enum class KeybindAction
{
    Press = 1,
    Release,
    Click
};

enum class KeybindModifier
{
    Invalid,
    ModNone = 1 << 0,
    Shift = 1 << 1,
    Ctrl = 1 << 2,
    Alt = 1 << 3,
    Any = 1 << 4,
};
DECLARE_GENERIC_BITWISE_OPERATORS(KeybindModifier);

typedef bool MousePositionUpdateFunc(f32 x, f32 y);
typedef bool MouseScrollUpdateFunc(f32 x, f32 y);
typedef bool KeyboardInputCallbackFunc(i32 key, KeybindAction action, KeybindModifier modifier);
typedef bool CharInputCallbackFunc(u32 unicode);

class KeybindGroup
{
public:
    struct Keybind
    {
    public:
        std::string keybindName;
        u32 keybindNameHash;
        i32 glfwKey;
        KeybindAction actionMask;
        KeybindModifier modifierMask;
        bool isPressed;

        std::function<KeyboardInputCallbackFunc> callback;
    };

    struct InputConsumedInfo
    {
    public:
        const std::string* keybindGroupName = nullptr;
        u32 keybindGroupNameHash = 0;

        const std::string* keybindName = nullptr;
        u32 keybindNameHash = 0;
    };

public:
    const std::string& GetDebugName();
    const u32& GetDebugNameHash();
    const u32& GetPriority();
    const std::vector<Keybind*>& GetKeybinds();

    bool IsActive();
    void SetActive(bool state);

    bool AddKeyboardCallback(const std::string& keybindName, i32 glfwKey, KeybindAction actionMask, KeybindModifier modifierMask, std::function<KeyboardInputCallbackFunc> callback);
    void AddKeyboardInputValidator(const std::string& keybindName, std::function<KeyboardInputCallbackFunc> callback);
    bool AddUnicodeCallback(u32 unicode, std::function<CharInputCallbackFunc> callback);
    void AddAnyUnicodeCallback(std::function<CharInputCallbackFunc> callback);
    void AddMousePositionCallback(std::function<MousePositionUpdateFunc> callback);
    void AddMouseScrollCallback(std::function<MouseScrollUpdateFunc> callback);
    void AddMouseInputValidator(const std::string& keybindName, std::function<KeyboardInputCallbackFunc> callback);
    void AddMousePositionValidator(std::function<MousePositionUpdateFunc> callback);
    void AddMouseScrollValidator(std::function<MouseScrollUpdateFunc> callback);

    bool IsKeybindPressed(u32 keybindNameHash);

private:
    bool MousePositionUpdate(f32 x, f32 y, bool wasConsumed, InputConsumedInfo& inputConsumedInfo);
    bool MouseScrollUpdate(f32 x, f32 y, bool wasConsumed, InputConsumedInfo& inputConsumedInfo);
    bool MouseInputHandler(i32 button, i32 actionMask, i32 modifierMask, bool wasConsumed, InputConsumedInfo& inputConsumedInfo);
    bool KeyboardInputCallback(i32 glfwKey, i32 actionMask, i32 modifierMask, bool wasConsumed, InputConsumedInfo& inputConsumedInfo);
    bool CharInputCallback(u32 unicodeKey, InputConsumedInfo& inputConsumedInfo);

private:
    KeybindGroup(const std::string& debugName, u32 priority);

private:
    friend class InputManager;

    const std::string* consumerInfoNameKeyboardInputValidator = new std::string("Keyboard Input Validator");
    const std::string* consumerInfoNameMouseInputValidator = new std::string("Mouse Input Validator");
    const std::string* consumerInfoNameMousePositionValidator = new std::string("Mouse Position Validator");
    const std::string* consumerInfoNameMouseScrollValidator = new std::string("Mouse Scroll Validator");
    const std::string* consumerInfoNameAnyUnicodeInput = new std::string("Any Unicode Input");
    const u32 consumerInfoNameHashKeyboardInputValidator = StringUtils::fnv1a_32(consumerInfoNameKeyboardInputValidator->c_str(), consumerInfoNameKeyboardInputValidator->length());
    const u32 consumerInfoNameHashMouseInputValidator = StringUtils::fnv1a_32(consumerInfoNameMouseInputValidator->c_str(), consumerInfoNameMouseInputValidator->length());
    const u32 consumerInfoNameHashMousePositionValidator = StringUtils::fnv1a_32(consumerInfoNameMousePositionValidator->c_str(), consumerInfoNameMousePositionValidator->length());
    const u32 consumerInfoNameHashMouseScrollValidator = StringUtils::fnv1a_32(consumerInfoNameMouseScrollValidator->c_str(), consumerInfoNameMouseScrollValidator->length());
    const u32 consumerInfoNameHashAnyUnicodeInput = "Any Unicode Input"_h;

    std::string _debugName;
    u32 _debugNameHash;
    u32 _priority;
    bool _isActive = false;

    std::vector<Keybind*> _keybinds;
    robin_hood::unordered_map<u32, std::function<CharInputCallbackFunc>> _unicodeToCallback;

    Keybind* _keyboardInputValidator = nullptr;
    Keybind* _mouseInputValidator = nullptr;
    std::function<MousePositionUpdateFunc> _mousePositionValidator = nullptr;
    std::function<MouseScrollUpdateFunc> _mouseScrollValidator = nullptr;

    std::function<CharInputCallbackFunc> _anyUnicodeInputCallback = nullptr;
    std::function<MousePositionUpdateFunc> _mousePositionUpdateCallback = nullptr;
    std::function<MouseScrollUpdateFunc> _mouseScrollUpdateCallback = nullptr;
};

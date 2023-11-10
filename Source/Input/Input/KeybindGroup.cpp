#include "KeybindGroup.h"
#include "InputManager.h"

#include <Base/Util/DebugHandler.h>
#include <Base/Util/StringUtils.h>

#include <GLFW/glfw3.h>

KeybindGroup::KeybindGroup(const std::string& debugName, u32 priority) : _debugName(debugName), _priority(priority)
{
    _debugNameHash = StringUtils::fnv1a_32(debugName.c_str(), debugName.length());
}

const std::vector<KeybindGroup::Keybind*>& KeybindGroup::GetKeybinds()
{
    return _keybinds;
}

const std::string& KeybindGroup::GetDebugName()
{
    return _debugName;
}

const u32& KeybindGroup::GetDebugNameHash()
{
    return _debugNameHash;
}

const u32& KeybindGroup::GetPriority()
{
    return _priority;
}

bool KeybindGroup::IsActive()
{
    return _isActive;
}

void KeybindGroup::SetActive(bool state)
{
    if (state == _isActive)
        return;

    _isActive = state;
    if (_isActive == false)
    {
        u32 numKeybinds = static_cast<u32>(_keybinds.size());
        for (u32 i = 0; i < numKeybinds; i++)
        {
            Keybind* keybind = _keybinds[i];
            if (keybind->isPressed)
            {
                if (keybind->callback)
                    keybind->callback(keybind->glfwKey, KeybindAction::Release, KeybindModifier::ModNone);

                keybind->isPressed = false;
            }
        }
    }
}

bool KeybindGroup::AddKeyboardCallback(const std::string& keybindName, i32 glfwKey, KeybindAction actionMask, KeybindModifier modifierMask, std::function<KeyboardInputCallbackFunc> callback)
{
    u32 numKeybinds = static_cast<u32>(_keybinds.size());
    for (u32 i = 0; i < numKeybinds; i++)
    {
        Keybind* keybind = _keybinds[i];

        if (keybind->glfwKey         == glfwKey &&
            keybind->actionMask      == actionMask &&
            keybind->modifierMask    == modifierMask)
            return false;
    }

    Keybind* keybind = new Keybind();
    {
        keybind->keybindName = keybindName;
        keybind->keybindNameHash = StringUtils::fnv1a_32(keybindName.c_str(), keybindName.length());
        keybind->glfwKey = glfwKey;
        keybind->actionMask = actionMask;
        keybind->modifierMask = modifierMask;
        keybind->isPressed = false;
        keybind->callback = callback;
    }

    _keybinds.push_back(keybind);

    return true;
}

void KeybindGroup::AddKeyboardInputValidator(const std::string& keybindName, std::function<KeyboardInputCallbackFunc> callback)
{
    if (_keyboardInputValidator)
    {
        DebugHandler::PrintFatal("KeybindGroup : Attempted to install KeyboardInputValidator \"{0}\" for KeybindGroup \"{1}\" but KeyboardInputValidator\"{2}\" is already installed", keybindName, _debugName, _keyboardInputValidator->keybindName);
    }

    _keyboardInputValidator = new Keybind();
    {
        _keyboardInputValidator->keybindName = keybindName;
        _keyboardInputValidator->keybindNameHash = StringUtils::fnv1a_32(keybindName.c_str(), keybindName.length());
        _keyboardInputValidator->glfwKey = 0;
        _keyboardInputValidator->actionMask = KeybindAction::Press;
        _keyboardInputValidator->modifierMask = KeybindModifier::ModNone;
        _keyboardInputValidator->isPressed = false;
        _keyboardInputValidator->callback = callback;
    }
}

bool KeybindGroup::AddUnicodeCallback(u32 unicode, std::function<CharInputCallbackFunc> callback)
{
    auto itr = _unicodeToCallback.find(unicode);
    if (itr == _unicodeToCallback.end())
        return false;

    _unicodeToCallback[unicode] = callback;
    return true;
}

void KeybindGroup::AddAnyUnicodeCallback(std::function<CharInputCallbackFunc> callback)
{
    _anyUnicodeInputCallback = callback;
}

void KeybindGroup::AddMousePositionCallback(std::function<MousePositionUpdateFunc> callback)
{
    _mousePositionUpdateCallback = callback;
}

void KeybindGroup::AddMouseScrollCallback(std::function<MouseScrollUpdateFunc> callback)
{
    _mouseScrollUpdateCallback = callback;
}
void KeybindGroup::AddMouseInputValidator(const std::string& keybindName, std::function<KeyboardInputCallbackFunc> callback)
{
    if (_mouseInputValidator)
    {
        DebugHandler::PrintFatal("KeybindGroup : Attempted to install MouseInputValidator \"{0}\" for KeybindGroup \"{1}\" but MouseInputValidator\"{2}\" is already installed", keybindName, _debugName, _mouseInputValidator->keybindName);
    }

    _mouseInputValidator = new Keybind();
    {
        _mouseInputValidator->keybindName = keybindName;
        _mouseInputValidator->keybindNameHash = StringUtils::fnv1a_32(keybindName.c_str(), keybindName.length());
        _mouseInputValidator->glfwKey = 0;
        _mouseInputValidator->actionMask = KeybindAction::Press;
        _mouseInputValidator->modifierMask = KeybindModifier::ModNone;
        _mouseInputValidator->isPressed = false;
        _mouseInputValidator->callback = callback;
    }
}
void KeybindGroup::AddMousePositionValidator(std::function<MousePositionUpdateFunc> callback)
{
    if (_mousePositionValidator && callback != nullptr)
    {
        DebugHandler::PrintFatal("KeybindGroup : Attempted to install a MousePositionValidator for KeybindGroup \"{0}\" but a MousePositionValidator is already installed", _debugName);
    }

    _mousePositionValidator = callback;
}
void KeybindGroup::AddMouseScrollValidator(std::function<MouseScrollUpdateFunc> callback)
{
    if (_mouseScrollValidator && callback != nullptr)
    {
        DebugHandler::PrintFatal("KeybindGroup : Attempted to install a MouseScrollValidator for KeybindGroup \"{0}\" but a MouseScrollValidator is already installed", _debugName);
    }

    _mouseScrollValidator = callback;
}

bool KeybindGroup::IsKeybindPressed(u32 keybindHash)
{
    if (IsActive())
    {
        u32 numKeybinds = static_cast<u32>(_keybinds.size());
        for (u32 i = 0; i < numKeybinds; i++)
        {
            Keybind* keybind = _keybinds[i];

            if (keybind->keybindNameHash == keybindHash)
                return keybind->isPressed;
        }
    }

    return false;
}

bool KeybindGroup::MousePositionUpdate(f32 x, f32 y, bool wasConsumed, InputConsumedInfo& inputConsumedInfo)
{
    bool positionValidatorResult = false;
    if (_mousePositionValidator && !wasConsumed)
    {
        positionValidatorResult = _mousePositionValidator(x, y);

        if (!positionValidatorResult)
            return false;
    }

    if (_mousePositionUpdateCallback && !wasConsumed)
    {
        if (_mousePositionUpdateCallback(x, y))
        {
            return true;
        }
    }

    return positionValidatorResult;
}

bool KeybindGroup::MouseScrollUpdate(f32 x, f32 y, bool wasConsumed, InputConsumedInfo& inputConsumedInfo)
{
    bool scrollValidatorResult = false;
    if (_mouseScrollValidator && !wasConsumed)
    {
        scrollValidatorResult = _mouseScrollValidator(x, y);

        if (!scrollValidatorResult)
            return false;
    }

    if (_mouseScrollUpdateCallback && !wasConsumed)
    {
        if (_mouseScrollUpdateCallback(x, y))
            return true;
    }

    return scrollValidatorResult;
}

bool KeybindGroup::MouseInputHandler(i32 button, i32 actionMask, i32 modifierMask, bool wasConsumed, InputConsumedInfo& inputConsumedInfo)
{
    bool inputValidatorResult = false;

    if (_mouseInputValidator && !wasConsumed)
    {
        Keybind* keybind = _mouseInputValidator;

        modifierMask &= GLFW_MOD_SHIFT | GLFW_MOD_CONTROL | GLFW_MOD_ALT;
        KeybindModifier modifiers = static_cast<KeybindModifier>(modifierMask << 1);

        if (actionMask == GLFW_RELEASE)
        {
            inputValidatorResult = keybind->callback(button, KeybindAction::Release, modifiers);
        }
        else
        {
            inputValidatorResult = keybind->callback(button, KeybindAction::Press, modifiers);
        }

        if (!inputValidatorResult)
        {
            return false;
        }
    }

    u32 numKeybinds = static_cast<u32>(_keybinds.size());
    for (u32 i = 0; i < numKeybinds; i++)
    {
        Keybind* keybind = _keybinds[i];

        if (keybind->glfwKey == button)
        {
            if (keybind->isPressed && wasConsumed)
            {
                keybind->isPressed = false;

                if (!keybind->callback)
                    return false;

                KeybindModifier modifiers = static_cast<KeybindModifier>(modifierMask << 1);
                keybind->callback(button, KeybindAction::Release, modifiers);
            }
            else if (!wasConsumed)
            {
                keybind->isPressed = actionMask;

                if (!keybind->callback)
                {
                    inputConsumedInfo.keybindName = &keybind->keybindName;
                    inputConsumedInfo.keybindNameHash = keybind->keybindNameHash;
                    return true;
                }

                modifierMask &= GLFW_MOD_SHIFT | GLFW_MOD_CONTROL | GLFW_MOD_ALT;
                KeybindModifier modifiers = static_cast<KeybindModifier>(modifierMask << 1);
                KeybindModifier modifierExcludingNone = keybind->modifierMask & ~KeybindModifier::ModNone;

                bool hasRequiredModifiers = keybind->modifierMask == KeybindModifier::Any ||
                                            ((keybind->modifierMask & KeybindModifier::ModNone) != KeybindModifier::Invalid && modifiers == KeybindModifier::Invalid) ||
                    (modifierExcludingNone & modifiers) == modifierExcludingNone;

                if (actionMask == GLFW_RELEASE &&
                    (keybind->actionMask == KeybindAction::Release || keybind->actionMask == KeybindAction::Click))
                {
                    if (keybind->callback(button, KeybindAction::Release, modifiers))
                    {
                        inputConsumedInfo.keybindName = &keybind->keybindName;
                        inputConsumedInfo.keybindNameHash = keybind->keybindNameHash;

                        return true;
                    }
                }
                else if (actionMask == GLFW_PRESS &&
                    hasRequiredModifiers &&
                    (keybind->actionMask == KeybindAction::Press || keybind->actionMask == KeybindAction::Click))
                {
                    if (keybind->callback(button, KeybindAction::Press, modifiers))
                    {
                        inputConsumedInfo.keybindName = &keybind->keybindName;
                        inputConsumedInfo.keybindNameHash = keybind->keybindNameHash;

                        return true;
                    }
                }
            }
        }
    }

    return inputValidatorResult;
}

bool KeybindGroup::KeyboardInputCallback(i32 glfwKey, i32 actionMask, i32 modifierMask, bool wasConsumed, InputConsumedInfo& inputConsumedInfo)
{
    bool inputValidatorResult = false;

    if (_keyboardInputValidator && !wasConsumed)
    {
        Keybind* keybind = _keyboardInputValidator;

        modifierMask &= GLFW_MOD_SHIFT | GLFW_MOD_CONTROL | GLFW_MOD_ALT;
        KeybindModifier modifiers = static_cast<KeybindModifier>(modifierMask << 1);

        if (actionMask == GLFW_RELEASE)
        {
            inputValidatorResult = keybind->callback(glfwKey, KeybindAction::Release, modifiers);
        }
        else
        {
            inputValidatorResult = keybind->callback(glfwKey, KeybindAction::Press, modifiers);
        }

        if (!inputValidatorResult)
        {
            return false;
        }
    }

    u32 numKeybinds = static_cast<u32>(_keybinds.size());
    for (u32 i = 0; i < numKeybinds; i++)
    {
        Keybind* keybind = _keybinds[i];

        if (keybind->glfwKey == glfwKey)
        {
            if (keybind->isPressed && wasConsumed)
            {
                keybind->isPressed = false;

                if (!keybind->callback)
                    return false;

                KeybindModifier modifiers = static_cast<KeybindModifier>(modifierMask << 1);
                keybind->callback(glfwKey, KeybindAction::Release, modifiers);
            }
            else if (!wasConsumed)
            {
                keybind->isPressed = actionMask;

                if (!keybind->callback)
                {
                    inputConsumedInfo.keybindName = &keybind->keybindName;
                    inputConsumedInfo.keybindNameHash = keybind->keybindNameHash;
                    return true;
                }

                modifierMask &= GLFW_MOD_SHIFT | GLFW_MOD_CONTROL | GLFW_MOD_ALT;
                KeybindModifier modifiers = static_cast<KeybindModifier>(modifierMask << 1);
                KeybindModifier modifierExcludingNone = keybind->modifierMask & ~KeybindModifier::ModNone;

                bool hasRequiredModifiers = keybind->modifierMask == KeybindModifier::Any ||
                                            ((keybind->modifierMask & KeybindModifier::ModNone) != KeybindModifier::Invalid && modifiers == KeybindModifier::Invalid) ||
                                            (modifierExcludingNone & modifiers) == modifierExcludingNone;
                
                if (actionMask == GLFW_RELEASE  &&
                   (keybind->actionMask == KeybindAction::Release || keybind->actionMask == KeybindAction::Click))
                {
                    if (keybind->callback(glfwKey, KeybindAction::Release, modifiers))
                    {
                        inputConsumedInfo.keybindName = &keybind->keybindName;
                        inputConsumedInfo.keybindNameHash = keybind->keybindNameHash;

                        return true;
                    }
                }
                else if (actionMask == GLFW_PRESS   &&
                         hasRequiredModifiers       &&
                        (keybind->actionMask == KeybindAction::Press || keybind->actionMask == KeybindAction::Click))
                {
                    if (keybind->callback(glfwKey, KeybindAction::Press, modifiers))
                    {
                        inputConsumedInfo.keybindName = &keybind->keybindName;
                        inputConsumedInfo.keybindNameHash = keybind->keybindNameHash;

                        return true;
                    }
                }
            }
        }
    }

    return inputValidatorResult;
}

bool KeybindGroup::CharInputCallback(u32 unicode, InputConsumedInfo& inputConsumedInfo)
{
    if (_anyUnicodeInputCallback)
    {
        if (_anyUnicodeInputCallback(unicode))
        {
            if (inputConsumedInfo.keybindNameHash != consumerInfoNameHashAnyUnicodeInput)
            {
                inputConsumedInfo.keybindName = consumerInfoNameAnyUnicodeInput;
                inputConsumedInfo.keybindNameHash = consumerInfoNameHashAnyUnicodeInput;
            }

            return true;
        }
    }

    auto itr = _unicodeToCallback.find(unicode);
    if (itr == _unicodeToCallback.end())
        return false;

    if (!itr->second)
        return true;

    return itr->second(unicode);
}

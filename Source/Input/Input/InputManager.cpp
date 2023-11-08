#include "InputManager.h"

#include <Base/Util/StringUtils.h>

#include <GLFW/glfw3.h>

InputManager::InputManager()
{
    _mouseInputConsumeInfo.keybindGroupName = consumerInfoNameDefault;
    _mouseInputConsumeInfo.keybindGroupNameHash = consumerInfoNameHashDefault;
    _mouseInputConsumeInfo.keybindName = consumerInfoNameDefault;
    _mouseInputConsumeInfo.keybindNameHash = consumerInfoNameHashDefault;

    _mousePositionConsumeInfo.keybindGroupName = consumerInfoNameDefault;
    _mousePositionConsumeInfo.keybindGroupNameHash = consumerInfoNameHashDefault;
    _mousePositionConsumeInfo.keybindName = consumerInfoNameDefault;
    _mousePositionConsumeInfo.keybindNameHash = consumerInfoNameHashDefault;

    _mouseScrollConsumeInfo.keybindGroupName = consumerInfoNameDefault;
    _mouseScrollConsumeInfo.keybindGroupNameHash = consumerInfoNameHashDefault;
    _mouseScrollConsumeInfo.keybindName = consumerInfoNameDefault;
    _mouseScrollConsumeInfo.keybindNameHash = consumerInfoNameHashDefault;

    _keyboardInputConsumeInfo.keybindGroupName = consumerInfoNameDefault;
    _keyboardInputConsumeInfo.keybindGroupNameHash = consumerInfoNameHashDefault;
    _keyboardInputConsumeInfo.keybindName = consumerInfoNameDefault;
    _keyboardInputConsumeInfo.keybindNameHash = consumerInfoNameHashDefault;

    _unicodeInputConsumeInfo.keybindGroupName = consumerInfoNameDefault;
    _unicodeInputConsumeInfo.keybindGroupNameHash = consumerInfoNameHashDefault;
    _unicodeInputConsumeInfo.keybindName = consumerInfoNameDefault;
    _unicodeInputConsumeInfo.keybindNameHash = consumerInfoNameHashDefault;
}

KeybindGroup* InputManager::CreateKeybindGroup(const std::string& debugName, u32 priority)
{
    KeybindGroup* keybindGroup = new KeybindGroup(debugName, priority);
    _keybindGroups.push_back(keybindGroup);

    // Ensure we sort the vector as such that KeybindGroups with a higher priority comes first
    std::sort(_keybindGroups.begin(), _keybindGroups.end(), [&](KeybindGroup* a, KeybindGroup* b) { return a->GetPriority() > b->GetPriority(); });

    return keybindGroup;
}

KeybindGroup* InputManager::GetKeybindGroupByHash(const u32 hash)
{
    u32 numKeybinds = static_cast<u32>(_keybindGroups.size());
    for (u32 i = 0; i < numKeybinds; i++)
    {
        KeybindGroup* keybindGroup = _keybindGroups[i];

        if (keybindGroup->_debugNameHash == hash)
            return keybindGroup;
    }

    return nullptr;
}

const std::vector<KeybindGroup*>& InputManager::GetKeybindGroups()
{
    return _keybindGroups;
}

void InputManager::KeyboardInputHandler(i32 key, i32 /*scanCode*/, i32 actionMask, i32 modifierMask)
{
    u32 numKeybinds = static_cast<u32>(_keybindGroups.size());
    bool wasConsumed = false;

    for (u32 i = 0; i < numKeybinds; i++)
    {
        KeybindGroup* keybindGroup = _keybindGroups[i];
        if (!keybindGroup->IsActive())
            continue;

        bool previousWasAbsorbed = wasConsumed;
        wasConsumed |= keybindGroup->KeyboardInputCallback(key, actionMask, modifierMask, wasConsumed, _keyboardInputConsumeInfo);

        if (!previousWasAbsorbed && wasConsumed)
        {
            if (_keyboardInputConsumeInfo.keybindGroupNameHash != keybindGroup->GetDebugNameHash())
            {
                _keyboardInputConsumeInfo.keybindGroupName = &keybindGroup->GetDebugName();
                _keyboardInputConsumeInfo.keybindGroupNameHash = keybindGroup->GetDebugNameHash();
            }
        }
    }
}
void InputManager::CharInputHandler(u32 unicode)
{
    u32 numKeybinds = static_cast<u32>(_keybindGroups.size());
    for (u32 i = 0; i < numKeybinds; i++)
    {
        KeybindGroup* keybindGroup = _keybindGroups[i];

        if (keybindGroup->CharInputCallback(unicode, _unicodeInputConsumeInfo))
        {
            if (_unicodeInputConsumeInfo.keybindGroupNameHash != keybindGroup->GetDebugNameHash())
            {
                _unicodeInputConsumeInfo.keybindGroupName = &keybindGroup->GetDebugName();
                _unicodeInputConsumeInfo.keybindGroupNameHash = keybindGroup->GetDebugNameHash();
            }

            return;
        }
    }
}
void InputManager::MouseInputHandler(i32 button, i32 actionMask, i32 modifierMask)
{
    _mouseState = actionMask == GLFW_RELEASE ? 0 : 1;

    u32 numKeybinds = static_cast<u32>(_keybindGroups.size());
    bool wasConsumed = false;

    for (u32 i = 0; i < numKeybinds; i++)
    {
        KeybindGroup* keybindGroup = _keybindGroups[i];
        if (!keybindGroup->IsActive())
            continue;

        bool previousWasAbsorbed = wasConsumed;
        wasConsumed |= keybindGroup->MouseInputHandler(button, actionMask, modifierMask, wasConsumed, _mouseInputConsumeInfo);

        if (!previousWasAbsorbed && wasConsumed)
        {
            if (_mouseInputConsumeInfo.keybindGroupNameHash != keybindGroup->GetDebugNameHash())
            {
                _mouseInputConsumeInfo.keybindGroupName = &keybindGroup->GetDebugName();
                _mouseInputConsumeInfo.keybindGroupNameHash = keybindGroup->GetDebugNameHash();
            }
        }
    }
}
void InputManager::MousePositionHandler(f32 x, f32 y)
{
    _mousePositionX = x;
    _mousePositionY = y;

    bool wasConsumed = false;
    u32 numKeybinds = static_cast<u32>(_keybindGroups.size());

    for (u32 i = 0; i < numKeybinds; i++)
    {
        KeybindGroup* keybindGroup = _keybindGroups[i];
        if (!keybindGroup->IsActive())
            continue;

        if (keybindGroup->_mousePositionUpdateCallback)
        {
            // If the callback returns true, we consume the input
            bool previousWasAbsorbed = wasConsumed;
            wasConsumed |= keybindGroup->MousePositionUpdate(x, y, wasConsumed, _mousePositionConsumeInfo);

            if (!previousWasAbsorbed && wasConsumed)
            {
                if (_mousePositionConsumeInfo.keybindGroupNameHash != keybindGroup->GetDebugNameHash())
                {
                    _mousePositionConsumeInfo.keybindGroupName = &keybindGroup->GetDebugName();
                    _mousePositionConsumeInfo.keybindGroupNameHash = keybindGroup->GetDebugNameHash();
                }
            }
        }
    }
}
void InputManager::MouseScrollHandler(f32 x, f32 y)
{
    u32 numKeybinds = static_cast<u32>(_keybindGroups.size());
    bool wasConsumed = false;

    for (u32 i = 0; i < numKeybinds; i++)
    {
        KeybindGroup* keybindGroup = _keybindGroups[i];
        if (!keybindGroup->IsActive())
            continue;

        if (keybindGroup->_mouseScrollUpdateCallback)
        {
            // If the callback returns true, we consume the input
            bool previousWasAbsorbed = wasConsumed;
            wasConsumed |= keybindGroup->MouseScrollUpdate(x, y, wasConsumed, _mouseScrollConsumeInfo);

            if (!previousWasAbsorbed && wasConsumed)
            {
                if (_mouseScrollConsumeInfo.keybindGroupNameHash != keybindGroup->GetDebugNameHash())
                {
                    _mouseScrollConsumeInfo.keybindGroupName = &keybindGroup->GetDebugName();
                    _mouseScrollConsumeInfo.keybindGroupNameHash = keybindGroup->GetDebugNameHash();
                }
            }
        }
    }
}
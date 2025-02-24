#pragma once
#include "KeybindGroup.h"

#include <Base/Types.h>
#include <Base/Util/StringUtils.h>

#include <robinhood/robinhood.h>

#include <memory>

struct CursorState
{
    u32 buttonPressed : 31 = 0;
    u32 isVirtual : 1 = 0;
};

class InputManager
{
public:
    InputManager();

    KeybindGroup* CreateKeybindGroup(const std::string& debugName, u32 priority);
    KeybindGroup* GetKeybindGroupByHash(const u32 hash);
    const std::vector<KeybindGroup*>& GetKeybindGroups();

    void KeyboardInputHandler(i32 key, i32 scancode, i32 actionMask, i32 modifierMask);
    void CharInputHandler(u32 unicodeKey);
    void MouseInputHandler(i32 button, i32 actionMask, i32 modifierMask);
    void MousePositionHandler(f32 x, f32 y);
    void MouseScrollHandler(f32 x, f32 y);

    bool IsMouseButtonDown(i32 button);

    bool IsCursorVirtual();
    void SetCursorVirtual(bool isVirtual);

    vec2 GetMousePosition();
    void SetMousePosition(f32 x, f32 y);

    const KeybindGroup::InputConsumedInfo& GetMouseInputConsumeInfo() { return _mouseInputConsumeInfo; }
    const KeybindGroup::InputConsumedInfo& GetMousePositionConsumeInfo() { return _mousePositionConsumeInfo; }
    const KeybindGroup::InputConsumedInfo& GetMouseScrollConsumeInfo() { return _mouseScrollConsumeInfo; }
    const KeybindGroup::InputConsumedInfo& GetKeyboardInputConsumeInfo() { return _keyboardInputConsumeInfo; }
    const KeybindGroup::InputConsumedInfo& GetUnicodeInputConsumeInfo() { return _unicodeInputConsumeInfo; }

private:
    KeybindGroup::InputConsumedInfo _mouseInputConsumeInfo;
    KeybindGroup::InputConsumedInfo _mousePositionConsumeInfo;
    KeybindGroup::InputConsumedInfo _mouseScrollConsumeInfo;
    KeybindGroup::InputConsumedInfo _keyboardInputConsumeInfo;
    KeybindGroup::InputConsumedInfo _unicodeInputConsumeInfo;

    const std::string* consumerInfoNameDefault = new std::string("None");
    const std::string* consumerInfoNameImGui   = new std::string("ImGui");
    const u32 consumerInfoNameHashDefault = "None"_h;
    const u32 consumerInfoNameHashImGui = "ImGui"_h;

    std::vector<KeybindGroup*> _keybindGroups;
    CursorState _cursorState = { 0 };
    vec2 _cursorPosition = vec2(0.0f);
    vec2 _cursorVirtualPosition = vec2(0.0f);
};

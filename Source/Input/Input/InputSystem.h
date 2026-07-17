#pragma once
#include "InputTypes.h"

#include <array>
#include <bitset>
#include <functional>
#include <string>
#include <vector>

using InputEventCallback = std::function<InputReply(const InputEvent&)>;

struct InputSystemFrameMetrics
{
public:
    u32 queuedEvents = 0;
    u32 contextCallbacks = 0;
};

class InputSystem
{
public:
    static constexpr u32 MAX_CONTEXTS = 64;
    static constexpr u32 MAX_EVENTS_PER_FRAME = 1024;

    InputSystem();

    void BeginFrame();
    void ProcessEvents();

    void QueueKeyboardEvent(Key key, InputPhase phase, InputModifier modifiers = InputModifier::None);
    void QueueTextEvent(u32 codepoint);
    void QueueMouseButtonEvent(MouseButton button, InputPhase phase, InputModifier modifiers = InputModifier::None);
    void QueueCursorPositionEvent(f32 x, f32 y);
    void QueueScrollEvent(f32 x, f32 y);
    void QueueFocusEvent(bool focused);

    InputContextHandle CreateContext(const std::string& debugName, i32 priority, InputEventCallback callback);
    bool DestroyContext(InputContextHandle handle);
    bool SetContextActive(InputContextHandle handle, bool active);
    bool IsContextActive(InputContextHandle handle) const;

    bool IsDown(InputControl control) const;
    bool WasPressed(InputControl control) const;
    bool WasReleased(InputControl control) const;

    CursorMode GetCursorMode() const { return _cursorMode; }
    void SetCursorMode(CursorMode mode);
    bool IsMouseCaptured() const { return _cursorMode == CursorMode::Captured; }

    const vec2& GetMousePosition() const { return _mousePosition; }
    const vec2& GetMouseDelta() const { return _mouseDelta; }
    const vec2& GetScrollDelta() const { return _scrollDelta; }
    void SetMousePosition(const vec2& position);

    void SetMetricsEnabled(bool enabled)
    {
        if (enabled && !_metricsEnabled)
            _frameMetrics = {};

        _metricsEnabled = enabled;
    }
    const InputSystemFrameMetrics& GetFrameMetrics() const { return _frameMetrics; }

private:
    static constexpr u32 BUTTON_CONTROL_COUNT = INPUT_KEY_COUNT + INPUT_MOUSE_BUTTON_COUNT;
    static constexpr u32 INVALID_EVENT_INDEX = std::numeric_limits<u32>::max();

    struct Context
    {
    public:
        std::string debugName;
        InputEventCallback callback;
        i32 priority = 0;
        u32 creationOrder = 0;
        u16 generation = 1;
        bool occupied = false;
        bool active = false;
    };

    enum class ContextMutationType : u8
    {
        Destroy,
        SetActive
    };

    struct PendingContextMutation
    {
    public:
        InputContextHandle handle;
        ContextMutationType type = ContextMutationType::SetActive;
        bool active = false;
    };

    bool IsContextHandleValid(InputContextHandle handle) const;
    bool DestroyContextNow(InputContextHandle handle);
    bool SetContextActiveNow(InputContextHandle handle, bool active);
    void FlushContextMutations();
    void RebuildActiveContexts();
    void CancelContextInputs(u8 contextIndex);

    void QueueButtonEvent(InputControl control, InputPhase phase, InputModifier modifiers);
    bool AppendEvent(const InputEvent& event);
    void DispatchEvent(const InputEvent& event);
    void DispatchButtonEvent(const InputEvent& event);
    InputReply InvokeContext(Context& context, const InputEvent& event);
    InputModifier GetCurrentModifiers() const;

private:
    std::array<InputEvent, MAX_EVENTS_PER_FRAME> _events;
    u32 _eventCount = 0;
    u32 _cursorEventIndex = INVALID_EVENT_INDEX;
    u32 _scrollEventIndex = INVALID_EVENT_INDEX;
    bool _eventOverflowed = false;

    std::array<Context, MAX_CONTEXTS> _contexts;
    std::array<u8, MAX_CONTEXTS> _activeContexts;
    u32 _activeContextCount = 0;
    u32 _nextContextCreationOrder = 0;
    bool _activeContextsDirty = false;
    std::vector<PendingContextMutation> _pendingContextMutations;
    u32 _callbackDispatchDepth = 0;
    u32 _inputDispatchDepth = 0;
    bool _flushingContextMutations = false;

    std::array<u64, BUTTON_CONTROL_COUNT> _controlOwners = {};
    std::bitset<BUTTON_CONTROL_COUNT> _downControls;
    std::bitset<BUTTON_CONTROL_COUNT> _pressedControls;
    std::bitset<BUTTON_CONTROL_COUNT> _releasedControls;

    vec2 _mousePosition = vec2(0.0f);
    vec2 _mouseDelta = vec2(0.0f);
    vec2 _scrollDelta = vec2(0.0f);
    vec2 _lastCursorSample = vec2(0.0f);
    bool _hasCursorSample = false;
    CursorMode _cursorMode = CursorMode::Hardware;
    bool _focused = true;

    InputSystemFrameMetrics _frameMetrics;
    bool _metricsEnabled = false;
};

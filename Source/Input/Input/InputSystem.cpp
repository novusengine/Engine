#include "InputSystem.h"

#include <Base/Util/DebugHandler.h>

#include <algorithm>

InputSystem::InputSystem()
{
    _pendingContextMutations.reserve(8);

    for (u32 i = 0; i < MAX_CONTEXTS; i++)
    {
        _activeContexts[i] = static_cast<u8>(i);
    }
}

void InputSystem::BeginFrame()
{
    _eventCount = 0;
    _cursorEventIndex = INVALID_EVENT_INDEX;
    _scrollEventIndex = INVALID_EVENT_INDEX;
    _eventOverflowed = false;

    _pressedControls.reset();
    _releasedControls.reset();
    _mouseDelta = vec2(0.0f);
    _scrollDelta = vec2(0.0f);

    if (_metricsEnabled)
        _frameMetrics = {};
}

void InputSystem::ProcessEvents()
{
    if (_metricsEnabled)
        _frameMetrics.queuedEvents = _eventCount;

    if (_activeContextsDirty)
        RebuildActiveContexts();

    for (u32 i = 0; i < _eventCount; i++)
    {
        _inputDispatchDepth++;
        DispatchEvent(_events[i]);

        _inputDispatchDepth--;
        FlushContextMutations();

        if (_activeContextsDirty)
            RebuildActiveContexts();
    }
}

void InputSystem::QueueKeyboardEvent(Key key, InputPhase phase, InputModifier modifiers)
{
    if (key <= Key::Unknown || key > Key::Menu)
        return;

    QueueButtonEvent(InputControl::Keyboard(key), phase, modifiers);
}

void InputSystem::QueueTextEvent(u32 codepoint)
{
    InputEvent event;
    event.type = InputEventType::Text;
    event.codepoint = codepoint;
    AppendEvent(event);
}

void InputSystem::QueueMouseButtonEvent(MouseButton button, InputPhase phase, InputModifier modifiers)
{
    if (button > MouseButton::Button8)
        return;

    QueueButtonEvent(InputControl::Mouse(button), phase, modifiers);
}

void InputSystem::QueueCursorPositionEvent(f32 x, f32 y)
{
    const vec2 position(x, y);
    vec2 delta(0.0f);

    if (_hasCursorSample)
        delta = position - _lastCursorSample;

    _lastCursorSample = position;
    _hasCursorSample = true;
    _mouseDelta += delta;

    if (!IsMouseCaptured())
        _mousePosition = position;

    if (_cursorEventIndex == INVALID_EVENT_INDEX)
    {
        InputEvent event;
        event.type = InputEventType::CursorMove;
        event.position = _mousePosition;
        event.delta = delta;

        if (AppendEvent(event))
            _cursorEventIndex = _eventCount - 1;
    }
    else
    {
        InputEvent& event = _events[_cursorEventIndex];
        event.position = _mousePosition;
        event.delta += delta;
    }
}

void InputSystem::QueueScrollEvent(f32 x, f32 y)
{
    const vec2 delta(x, y);
    const InputModifier modifiers = GetCurrentModifiers();
    _scrollDelta += delta;

    if (_scrollEventIndex == INVALID_EVENT_INDEX || _events[_scrollEventIndex].modifiers != modifiers)
    {
        InputEvent event;
        event.type = InputEventType::Scroll;
        event.delta = delta;
        event.modifiers = modifiers;

        if (AppendEvent(event))
            _scrollEventIndex = _eventCount - 1;
    }
    else
    {
        _events[_scrollEventIndex].delta += delta;
    }
}

void InputSystem::QueueFocusEvent(bool focused)
{
    if (_focused == focused)
        return;

    _focused = focused;
    if (!focused)
    {
        for (u32 controlIndex = 0; controlIndex < BUTTON_CONTROL_COUNT; controlIndex++)
        {
            if (!_downControls.test(controlIndex))
                continue;

            InputControl control;
            if (controlIndex < INPUT_KEY_COUNT)
            {
                control = InputControl::Keyboard(static_cast<Key>(controlIndex));
            }
            else
            {
                control = InputControl::Mouse(static_cast<MouseButton>(controlIndex - INPUT_KEY_COUNT));
            }

            QueueButtonEvent(control, InputPhase::Canceled, InputModifier::None);
        }

        _hasCursorSample = false;
    }

    InputEvent event;
    event.type = InputEventType::FocusChanged;
    event.focused = focused;
    AppendEvent(event);
}

InputContextHandle InputSystem::CreateContext(const std::string& debugName, i32 priority, InputEventCallback callback)
{
    for (u32 i = 0; i < MAX_CONTEXTS; i++)
    {
        Context& context = _contexts[i];
        if (context.occupied)
            continue;

        context.debugName = debugName;
        context.callback = std::move(callback);
        context.priority = priority;
        context.creationOrder = _nextContextCreationOrder++;
        context.occupied = true;
        context.active = false;
        _activeContextsDirty = true;

        return { static_cast<u8>(i), context.generation };
    }

    NC_LOG_CRITICAL("InputSystem: Cannot create input context '{}'; the maximum of {} contexts has been reached", debugName, MAX_CONTEXTS);
    return {};
}

bool InputSystem::DestroyContext(InputContextHandle handle)
{
    if (!IsContextHandleValid(handle))
        return false;

    if (_callbackDispatchDepth != 0)
    {
        _pendingContextMutations.push_back({ handle, ContextMutationType::Destroy, false });
        return true;
    }

    return DestroyContextNow(handle);
}

bool InputSystem::DestroyContextNow(InputContextHandle handle)
{
    if (!IsContextHandleValid(handle))
        return false;

    Context& context = _contexts[handle.index];
    CancelContextInputs(handle.index);
    context.debugName.clear();
    context.callback = nullptr;
    context.priority = 0;
    context.creationOrder = 0;
    context.occupied = false;
    context.active = false;
    context.generation++;

    if (context.generation == 0)
        context.generation = 1;

    _activeContextsDirty = true;
    return true;
}

bool InputSystem::SetContextActive(InputContextHandle handle, bool active)
{
    if (!IsContextHandleValid(handle))
        return false;

    if (_callbackDispatchDepth != 0)
    {
        _pendingContextMutations.push_back({ handle, ContextMutationType::SetActive, active });
        return true;
    }

    return SetContextActiveNow(handle, active);
}

bool InputSystem::SetContextActiveNow(InputContextHandle handle, bool active)
{
    if (!IsContextHandleValid(handle))
        return false;

    Context& context = _contexts[handle.index];
    if (context.active == active)
        return true;

    if (!active)
        CancelContextInputs(handle.index);

    context.active = active;
    _activeContextsDirty = true;
    return true;
}

void InputSystem::FlushContextMutations()
{
    if (_callbackDispatchDepth != 0 || _inputDispatchDepth != 0 || _flushingContextMutations)
        return;

    _flushingContextMutations = true;
    while (!_pendingContextMutations.empty())
    {
        const PendingContextMutation mutation = _pendingContextMutations.front();
        _pendingContextMutations.erase(_pendingContextMutations.begin());
        if (mutation.type == ContextMutationType::Destroy)
            DestroyContextNow(mutation.handle);
        else
            SetContextActiveNow(mutation.handle, mutation.active);
    }
    _flushingContextMutations = false;
}

bool InputSystem::IsContextActive(InputContextHandle handle) const
{
    return IsContextHandleValid(handle) && _contexts[handle.index].active;
}

bool InputSystem::IsDown(InputControl control) const
{
    const u32 index = GetInputControlIndex(control);
    return index < BUTTON_CONTROL_COUNT && _downControls.test(index);
}

bool InputSystem::WasPressed(InputControl control) const
{
    const u32 index = GetInputControlIndex(control);
    return index < BUTTON_CONTROL_COUNT && _pressedControls.test(index);
}

bool InputSystem::WasReleased(InputControl control) const
{
    const u32 index = GetInputControlIndex(control);
    return index < BUTTON_CONTROL_COUNT && _releasedControls.test(index);
}

void InputSystem::SetCursorMode(CursorMode mode)
{
    if (_cursorMode == mode)
        return;

    _cursorMode = mode;
    _hasCursorSample = false;
}

void InputSystem::SetMousePosition(const vec2& position)
{
    _mousePosition = position;

    if (!IsMouseCaptured())
    {
        _lastCursorSample = position;
        _hasCursorSample = true;
    }
}

bool InputSystem::IsContextHandleValid(InputContextHandle handle) const
{
    if (!handle.IsValid() || handle.index >= MAX_CONTEXTS)
        return false;

    const Context& context = _contexts[handle.index];
    return context.occupied && context.generation == handle.generation;
}

void InputSystem::RebuildActiveContexts()
{
    _activeContextCount = 0;

    for (u32 i = 0; i < MAX_CONTEXTS; i++)
    {
        const Context& context = _contexts[i];
        if (context.occupied && context.active)
            _activeContexts[_activeContextCount++] = static_cast<u8>(i);
    }

    std::sort(_activeContexts.begin(), _activeContexts.begin() + _activeContextCount, [this](u8 leftIndex, u8 rightIndex)
    {
        const Context& left = _contexts[leftIndex];
        const Context& right = _contexts[rightIndex];

        if (left.priority != right.priority)
            return left.priority > right.priority;

        return left.creationOrder > right.creationOrder;
    });

    _activeContextsDirty = false;
}

void InputSystem::CancelContextInputs(u8 contextIndex)
{
    const u64 contextMask = 1ull << contextIndex;
    Context& context = _contexts[contextIndex];

    _inputDispatchDepth++;
    for (u32 controlIndex = 0; controlIndex < BUTTON_CONTROL_COUNT; controlIndex++)
    {
        if ((_controlOwners[controlIndex] & contextMask) == 0)
            continue;

        InputEvent event;
        event.type = InputEventType::Button;
        event.phase = InputPhase::Canceled;

        if (controlIndex < INPUT_KEY_COUNT)
        {
            event.control = InputControl::Keyboard(static_cast<Key>(controlIndex));
        }
        else
        {
            event.control = InputControl::Mouse(static_cast<MouseButton>(controlIndex - INPUT_KEY_COUNT));
        }

        if (context.callback)
            InvokeContext(context, event);

        _controlOwners[controlIndex] &= ~contextMask;
    }
    _inputDispatchDepth--;

    FlushContextMutations();
}

void InputSystem::QueueButtonEvent(InputControl control, InputPhase phase, InputModifier modifiers)
{
    const u32 controlIndex = GetInputControlIndex(control);
    if (controlIndex >= BUTTON_CONTROL_COUNT)
        return;

    InputEvent event;
    event.type = InputEventType::Button;
    event.control = control;
    event.phase = phase;
    event.modifiers = modifiers;

    if (!AppendEvent(event))
        return;

    if (phase == InputPhase::Pressed)
    {
        _downControls.set(controlIndex);
        _pressedControls.set(controlIndex);
    }
    else if (phase == InputPhase::Released || phase == InputPhase::Canceled)
    {
        _downControls.reset(controlIndex);
        _releasedControls.set(controlIndex);
    }
}

bool InputSystem::AppendEvent(const InputEvent& event)
{
    if (_eventCount >= MAX_EVENTS_PER_FRAME)
    {
        if (!_eventOverflowed)
        {
            NC_LOG_WARNING("InputSystem: Input event buffer overflowed; additional events will be dropped for this frame");
            _eventOverflowed = true;
        }

        return false;
    }

    _events[_eventCount++] = event;
    return true;
}

void InputSystem::DispatchEvent(const InputEvent& event)
{
    if (event.type == InputEventType::Button)
    {
        DispatchButtonEvent(event);
        return;
    }

    for (u32 i = 0; i < _activeContextCount; i++)
    {
        Context& context = _contexts[_activeContexts[i]];
        if (!context.active || !context.callback)
            continue;

        if (InvokeContext(context, event) == InputReply::Consumed)
            return;
    }
}

void InputSystem::DispatchButtonEvent(const InputEvent& event)
{
    const u32 controlIndex = GetInputControlIndex(event.control);
    if (controlIndex >= BUTTON_CONTROL_COUNT)
        return;

    u64& owners = _controlOwners[controlIndex];
    u64 dispatchedContexts = 0;

    if (event.phase == InputPhase::Repeated || event.phase == InputPhase::Released || event.phase == InputPhase::Canceled)
    {
        bool ownerConsumed = false;

        for (u32 i = 0; i < _activeContextCount; i++)
        {
            const u8 contextIndex = _activeContexts[i];
            const u64 contextMask = 1ull << contextIndex;
            if ((owners & contextMask) == 0)
                continue;

            Context& context = _contexts[contextIndex];
            if (context.active && context.callback && InvokeContext(context, event) == InputReply::Consumed)
                ownerConsumed = true;

            dispatchedContexts |= contextMask;
        }

        if (event.phase == InputPhase::Released || event.phase == InputPhase::Canceled)
        {
            const bool hadOwners = owners != 0;
            owners = 0;

            if (hadOwners)
                return;
        }

        if (ownerConsumed)
            return;
    }

    for (u32 i = 0; i < _activeContextCount; i++)
    {
        const u8 contextIndex = _activeContexts[i];
        const u64 contextMask = 1ull << contextIndex;
        if ((dispatchedContexts & contextMask) != 0)
            continue;

        Context& context = _contexts[contextIndex];
        if (!context.active || !context.callback)
            continue;

        const InputReply reply = InvokeContext(context, event);
        if (event.phase == InputPhase::Pressed && reply != InputReply::Ignored)
            owners |= contextMask;

        if (reply == InputReply::Consumed)
            return;
    }
}

InputReply InputSystem::InvokeContext(Context& context, const InputEvent& event)
{
    if (_metricsEnabled)
        _frameMetrics.contextCallbacks++;

    _callbackDispatchDepth++;
    const InputReply reply = context.callback(event);
    _callbackDispatchDepth--;

    FlushContextMutations();
    return reply;
}

InputModifier InputSystem::GetCurrentModifiers() const
{
    InputModifier modifiers = InputModifier::None;

    if (_downControls.test(static_cast<u32>(Key::LeftShift)) || _downControls.test(static_cast<u32>(Key::RightShift)))
        modifiers |= InputModifier::Shift;

    if (_downControls.test(static_cast<u32>(Key::LeftControl)) || _downControls.test(static_cast<u32>(Key::RightControl)))
        modifiers |= InputModifier::Control;

    if (_downControls.test(static_cast<u32>(Key::LeftAlt)) || _downControls.test(static_cast<u32>(Key::RightAlt)))
        modifiers |= InputModifier::Alt;

    if (_downControls.test(static_cast<u32>(Key::LeftSuper)) || _downControls.test(static_cast<u32>(Key::RightSuper)))
        modifiers |= InputModifier::Super;

    return modifiers;
}

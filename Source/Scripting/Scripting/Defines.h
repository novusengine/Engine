#pragma once
#include <Base/Types.h>

#include <limits>

#include <robinhood/robinhood.h>

namespace Generated {
    template <u16 TEventDataID>
    struct LuaEventDataTraits;
}

struct lua_State;
typedef i32(*lua_CFunction)(lua_State* L);

namespace Scripting
{
    struct Zenith;

    using LuaHandlerID = u16;
    using LuaUserDataDtor = void(void*);

    template <typename T>
    concept LuaEventTypeConcept = requires(T t)
    {
        { Generated::EnumTraits<std::decay_t<T>>::Meta::EnumID } -> std::convertible_to<u16>;
    };

    template <typename T>
    concept LuaEventDataConcept = requires(T t, lua_State* state)
    {
        { std::decay_t<T>::StructID } -> std::convertible_to<u16>;
        { std::decay_t<T>::NumParameters } -> std::convertible_to<u16>;

        { t.Push(state) } -> std::same_as<void>;
    };

    template<typename T, typename EventDataType>
    concept LuaEventCallbackConcept = 
        std::same_as<std::remove_cvref_t<T>, std::nullptr_t> ||
        requires(T t, Zenith* z, EventDataType& data)
        {
            { t(z, data) } -> std::same_as<void>;
        };

    using LuaGenericEventCallback = std::function<void(Zenith*, void*)>;

    struct EventState
    {
    public:
        static const u16 EVENT_DATA_ID_INVALID = std::numeric_limits<u16>::max();

        u16 eventDataID = EVENT_DATA_ID_INVALID;
        LuaGenericEventCallback callback = nullptr;

        robin_hood::unordered_map<u16, std::vector<i32>> eventVariantToFuncRef;
    };

    struct EventTypeState
    {
    public:
        robin_hood::unordered_map<u16, EventState> eventIDToEventState;
    };

    class LuaHandlerBase
    {
    public:
        virtual void Register(Zenith* zenith) = 0;
        virtual void Clear(Zenith* zenith) = 0;

        virtual void PostLoad(Zenith* zenith) = 0;
        virtual void Update(Zenith* zenith, f32 deltaTime) = 0;
    };
}
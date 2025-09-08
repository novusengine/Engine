#pragma once
#include <Base/Types.h>

#include <limits>

#include <robinhood/robinhood.h>

template <u16 TEventDataID>
struct LuaEventDataTraits;

struct lua_State;
typedef i32(*lua_CFunction)(lua_State* L);

namespace Scripting
{
    using LuaHandlerID = u16;
    using LuaUserDataDtor = void(void*);

    template <typename T>
    concept LuaEventTypeConcept = requires(T t)
    {
        { EnumTraits<std::decay_t<T>>::Meta::EnumID } -> std::convertible_to<u16>;
    };

    template <typename T>
    concept LuaEventDataConcept = requires(T t, lua_State * state)
    {
        { std::decay_t<T>::StructID } -> std::convertible_to<u16>;
        { std::decay_t<T>::NumParameters } -> std::convertible_to<u16>;

        { t.Push(state) } -> std::same_as<void>;
    };

    struct EventState
    {
    public:
        static const u16 EVENT_DATA_ID_INVALID = std::numeric_limits<u16>::max();

        u16 eventDataID = EVENT_DATA_ID_INVALID;
        std::vector<i32> FuncRefList;
    };

    struct EventTypeState
    {
    public:
        robin_hood::unordered_map<u16, EventState> eventIDToEventState;
    };

    struct Zenith;
    class LuaHandlerBase
    {
    public:
        virtual void Register(Zenith* zenith) = 0;
        virtual void Clear(Zenith* zenith) = 0;

        virtual void PostLoad(Zenith* zenith) = 0;
        virtual void Update(Zenith* zenith, f32 deltaTime) = 0;
    };
}
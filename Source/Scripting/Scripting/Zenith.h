#pragma once
#include "Defines.h"

#include <Base/Types.h>

#include <lualib.h>
#include <robinhood/robinhood.h>

#include <limits>
#include <memory>

struct lua_State;

namespace Scripting
{
    struct ZenithEventState
    {
    public:
        void Clear()
        {
            eventTypeToState.clear();
        }

    public:
        robin_hood::unordered_map<u16, EventTypeState> eventTypeToState;
    };

    struct ZenithInfoKey
    {
    public:
        constexpr ZenithInfoKey() {};
        constexpr ZenithInfoKey(u64 key) : value(key) {}

        constexpr bool operator==(const ZenithInfoKey& other) const { return value == other.value; }
        constexpr bool operator!=(const ZenithInfoKey& other) const { return value != other.value; }

    public:
        static constexpr ZenithInfoKey Make(u16 mapID, u16 instanceID, u16 variantID)
        {
            u64 key = (static_cast<u64>(mapID) << 32) | (static_cast<u64>(instanceID) << 16) | static_cast<u64>(variantID);
            return ZenithInfoKey(key);
        }

        static constexpr ZenithInfoKey MakeGlobal(u16 instanceID, u16 variantID)
        {
            return Make(INVALID_MAP_ID, instanceID, variantID);
        }

        constexpr bool IsValid() const { return value != INVALID_KEY; }
        constexpr bool IsGlobal() const { return GetMapID() == INVALID_MAP_ID; }

        constexpr u16 GetMapID() const { return static_cast<u16>((value >> 32) & 0xFFFF); }
        constexpr u16 GetInstanceID() const { return static_cast<u16>((value >> 16) & 0xFFFF); }
        constexpr u16 GetVariantID() const { return static_cast<u16>(value & 0xFFFF); }

    public:
        static constexpr u64 INVALID_KEY = ::std::numeric_limits<u64>().max();
        static constexpr u16 INVALID_MAP_ID = ::std::numeric_limits<u16>().max();

        u64 value = INVALID_KEY;
    };

    struct Zenith
    {
    public:
        Zenith() : state(nullptr) {}

        void SetState(lua_State* state);
        void ClearState() { state = nullptr; }
        void Clear();

        bool IsDirty() const { return _isDirty; }
        void SetDirty() { _isDirty = true; }
        void ClearDirty() { _isDirty = false; }

    public: // Lua Wrapped API
        i32 GetStatus();
        i32 GetTop();
        void SetTop(i32 index);
        void GetRaw(i32 index = -1);
        void GetRawI(i32 index, i32 n);
        i32 GetRef(i32 index = -1);

        void GetGlobalKey(const char* key);

        template <typename T>
        T GetGlobalField(const char* key);

        void SetGlobalKey(const char* key);
        void AddGlobalField(const char* key, bool value);
        void AddGlobalField(const char* key, i32 value);
        void AddGlobalField(const char* key, u32 value);
        void AddGlobalField(const char* key, f32 value);
        void AddGlobalField(const char* key, f64 value);
        void AddGlobalField(const char* key, const char* value);
        void AddGlobalField(const char* key, const vec3& value);
        void AddGlobalField(const char* key, const lua_CFunction value);
        void SetField(const char* key, i32 index = -2);

        bool IsNil(i32 index);
        bool IsBoolean(i32 index);
        bool IsNumber(i32 index);
        bool IsString(i32 index);
        bool IsVector(i32 index);
        bool IsFunction(i32 index);
        bool IsTable(i32 index);
        bool IsUserData(i32 index);
        bool IsLightUserData(i32 index);

        bool ToBoolean(i32 index);
        i32 ToInteger(i32 index);
        u32 ToUnsigned(i32 index);
        f64 ToNumber(i32 index);
        const char* ToString(i32 index);
        vec3 ToVector(i32 index);
        lua_CFunction ToCFunction(i32 index);
        void* ToUserData(i32 index);
        void* ToLightUserData(i32 index);

        template<typename T>
        void Push(const T& value);

        template <typename T>
        T* PushUserData(LuaUserDataDtor dtor)
        {
            T* userData = reinterpret_cast<T*>(AllocateUserData(state, sizeof(T), dtor));
            new (userData) T(); // Placement New to call constructor
            return userData;
        }

        void PushLightUserData(void* data)
        {
            lua_pushlightuserdata(state, data);
        }

        void Push();
        void Push(bool value);
        void Push(i8 value);
        void Push(i16 value);
        void Push(i32 value);
        void Push(i64 value);
        void Push(u8 value);
        void Push(u16 value);
        void Push(u32 value);
        void Push(u64 value);
        void Push(f32 value);
        void Push(f64 value);
        void Push(const char* value);
        void Push(lua_CFunction value, const char* debugName = nullptr);
        void PushValue(i32 index = -1);
        void Pop(i32 numPops = 1);

        template<typename T>
        T Get(i32 index);

        template<typename T>
        T CheckVal(i32 index);

        template <typename T>
        T* GetUserData(T* fallback = nullptr, i32 index = -1)
        {
            if (!IsUserData(index))
            {
                return fallback;
            }

            return reinterpret_cast<T*>(ToUserData(index));
        }

        void CreateTable();
        void CreateTable(const char* name);
        void CreateMetaTable(const char* name);

        bool GetTableField(const char* key, i32 index = -1);
        void SetTableKey(i32 key);
        void SetTableKey(const char* key);
        void AddTableField(const char* key, const bool value);
        void AddTableField(const char* key, const i8 value);
        void AddTableField(const char* key, const i16 value);
        void AddTableField(const char* key, const i32 value);
        void AddTableField(const char* key, const i64 value);
        void AddTableField(const char* key, const u8 value);
        void AddTableField(const char* key, const u16 value);
        void AddTableField(const char* key, const u32 value);
        void AddTableField(const char* key, const u64 value);
        void AddTableField(const char* key, const f32 value);
        void AddTableField(const char* key, const f64 value);
        void AddTableField(const char* key, const char* value);
        void AddTableField(const char* key, const vec3& value);
        void AddTableField(const char* key, const lua_CFunction value);
        void AddTableField(i32 key, const bool value);
        void AddTableField(i32 key, const i8 value);
        void AddTableField(i32 key, const i16 value);
        void AddTableField(i32 key, const i32 value);
        void AddTableField(i32 key, const i64 value);
        void AddTableField(i32 key, const u8 value);
        void AddTableField(i32 key, const u16 value);
        void AddTableField(i32 key, const u32 value);
        void AddTableField(i32 key, const u64 value);
        void AddTableField(i32 key, const f32 value);
        void AddTableField(i32 key, const f64 value);
        void AddTableField(i32 key, const char* value);
        void AddTableField(i32 key, const vec3& value);
        void AddTableField(i32 key, const lua_CFunction value);

        bool PCall(i32 numArgs = 0, i32 numResults = 0, i32 errorfunc = 0);

        i32 LoadBytecode(const ::std::string& chunkName, const ::std::string& bytecode, i32 env = 0);
        i32 Resume(lua_State* from = nullptr, i32 nArg = 0);
        void MakeReadOnly();
        void ReportError();
        void Close();

    public: // Custom Helper API
        void RegisterDefaultLibraries();

        bool CallAllFunctions(const std::vector<i32>& funcList, u32 numArguments, bool hasResult)
        {
            bool result = true;

            u32 numFunctions = static_cast<u32>(funcList.size());
            for (i32 funcRef : funcList)
            {
                GetRawI(LUA_REGISTRYINDEX, funcRef);
            }

            while (numFunctions > 0)
            {
                i32 functionTop = GetTop();
                i32 firstFunctionIndex = functionTop - numFunctions + 1;
                i32 argumentTop = firstFunctionIndex - 1;
                i32 firstArgumentIndex = argumentTop - numArguments + 1;

                for (i32 argIndex = firstArgumentIndex; argIndex <= argumentTop; ++argIndex)
                {
                    PushValue(argIndex);
                }

                i32 top = GetTop();
                i32 funcIndex = top - numArguments;

                PCall(numArguments, 1 * hasResult);

                if (hasResult)
                {
                    if (lua_isboolean(state, functionTop) && lua_toboolean(state, functionTop) != 1)
                        result = false;

                    Pop();
                }

                --numFunctions;
            }

            Pop(numArguments);

            return result;
        }
        bool CallAllFunctionsBool(const std::vector<i32>& funcList, u32 numArguments)
        {
            bool result = true;

            u32 numFunctions = static_cast<u32>(funcList.size());
            for (i32 funcRef : funcList)
            {
                GetRawI(LUA_REGISTRYINDEX, funcRef);
            }

            while (result && numFunctions > 0)
            {
                i32 functionTop = GetTop();
                i32 firstFunctionIndex = functionTop - numFunctions + 1;
                i32 argumentTop = firstFunctionIndex - 1;
                i32 firstArgumentIndex = argumentTop - numArguments + 1;

                for (i32 argIndex = firstArgumentIndex; argIndex <= argumentTop; ++argIndex)
                {
                    PushValue(argIndex);
                }

                --numFunctions;
                i32 top = GetTop();
                i32 funcIndex = top - numArguments;

                PCall(numArguments, 1);

                bool hasReturnedBoolean = lua_isboolean(state, functionTop);
                result = hasReturnedBoolean ? lua_toboolean(state, functionTop) != 0 : false;

                if (hasReturnedBoolean)
                    Pop();
            }

            Pop(numArguments + numFunctions);

            return result;
        }

        template <LuaEventTypeConcept EventType>
        bool RegisterEventType()
        {
            auto& eventTypeMap = eventState.eventTypeToState;

            using Meta = typename EnumTraits<EventType>::Meta;
            u16 eventTypeID = Meta::EnumID;
            if (eventTypeMap.contains(eventTypeID))
                return false;

            eventTypeMap[eventTypeID] = EventTypeState();

            CreateTable(Meta::EnumName.data());

            for (const auto& pair : Meta::EnumList)
            {
                u32 eventID = static_cast<u32>(pair.second) | (static_cast<u32>(eventTypeID) << 16);
                AddTableField(pair.first.data(), eventID);
            }

            Pop();

            return true;
        }

        template <LuaEventDataConcept EventDataType, LuaEventTypeConcept EventType, typename Callback = std::nullptr_t> requires LuaEventCallbackConcept<Callback, EventDataType>
        bool RegisterEventTypeID(EventType eventType, Callback&& callback = nullptr)
        {
            u16 eventTypeID = EnumTraits<EventType>::Meta::EnumID;
            u16 eventDataID = std::decay_t<EventDataType>::StructID;
            u16 eventTypeVal = static_cast<u16>(eventType);

            auto& eventTypeMap = eventState.eventTypeToState;

            bool isValidEventType = (eventType > EventType::Invalid && eventType < EventType::Count) && eventTypeMap.contains(eventTypeID);
            if (!isValidEventType)
                return false;

            if (!eventTypeMap.contains(eventTypeID))
                return false;

            EventTypeState& eventTypeState = eventTypeMap[eventTypeID];
            if (eventTypeState.eventIDToEventState.contains(eventTypeVal))
                return false;

            EventState& eventState = eventTypeState.eventIDToEventState[eventTypeVal];
            if (eventState.eventDataID != EventState::EVENT_DATA_ID_INVALID)
                return false;

            eventState.eventDataID = eventDataID;
            eventState.callback = nullptr;

            if constexpr (!std::is_same_v<std::remove_cvref_t<Callback>, std::nullptr_t>)
            {
                auto cb = std::forward<Callback>(callback);
                eventState.callback = [cb = std::move(cb)](Zenith* zenith, void* rawData)
                {
                    EventDataType& data = *static_cast<EventDataType*>(rawData);
                    return cb(zenith, data);
                };
            }

            return true;
        }

        inline bool RegisterEventTypeIDRaw(u16 eventTypeID, u16 eventID, u16 eventDataID)
        {
            auto& eventTypeMap = eventState.eventTypeToState;
            if (!eventTypeMap.contains(eventTypeID))
                return false;

            EventTypeState& eventTypeState = eventTypeMap[eventTypeID];
            if (eventTypeState.eventIDToEventState.contains(eventID))
                return false;

            EventState& eventState = eventTypeState.eventIDToEventState[eventID];
            if (eventState.eventDataID != EventState::EVENT_DATA_ID_INVALID)
                return false;

            eventState.eventDataID = eventDataID;
            return true;
        }
        inline bool RegisterEventCallbackRaw(u16 eventTypeID, u16 eventID, u16 variantID, i32 funcRef)
        {
            if (funcRef == 0)
                return false;

            auto& eventTypeMap = eventState.eventTypeToState;
            if (!eventTypeMap.contains(eventTypeID))
                return false;

            EventTypeState& eventTypeState = eventTypeMap[eventTypeID];
            if (!eventTypeState.eventIDToEventState.contains(eventID))
                return false;

            EventState& eventState = eventTypeState.eventIDToEventState[eventID];
            eventState.eventVariantToFuncRef[variantID].push_back(funcRef);

            return true;
        }    
        inline bool HasEventCallbackRaw(u16 eventTypeID, u16 eventID, u16 variantID)
        {
            auto& eventTypeMap = eventState.eventTypeToState;
            if (!eventTypeMap.contains(eventTypeID))
                return false;

            EventTypeState& eventTypeState = eventTypeMap[eventTypeID];
            if (!eventTypeState.eventIDToEventState.contains(eventID))
                return false;

            EventState& eventState = eventTypeState.eventIDToEventState[eventID];
            const auto& eventVariantFuncList = eventState.eventVariantToFuncRef[variantID];
            if (eventVariantFuncList.empty())
                return false;

            return true;
        }

        template <LuaEventTypeConcept EventType>
        bool RegisterEventCallback(EventType eventType, u16 variantID, i32 funcRef)
        {
            u16 eventTypeID = EnumTraits<EventType>::Meta::EnumID;
            u16 eventTypeVal = static_cast<u16>(eventType);

            return RegisterEventCallbackRaw(eventTypeID, eventTypeVal, variantID, funcRef);
        }

        template <LuaEventTypeConcept EventType, LuaEventDataConcept EventDataType>
        bool CallEvent(EventType eventType, EventDataType&& eventData, u16 variantID = 0)
        {
            u16 eventTypeID = EnumTraits<EventType>::Meta::EnumID;
            u16 eventDataID = std::decay_t<EventDataType>::StructID;
            u16 eventTypeVal = static_cast<u16>(eventType);

            auto& eventTypeMap = eventState.eventTypeToState;
            if (!eventTypeMap.contains(eventTypeID))
                return false;

            EventTypeState& eventTypeState = eventTypeMap[eventTypeID];
            if (!eventTypeState.eventIDToEventState.contains(eventTypeVal))
                return false;

            EventState& eventState = eventTypeState.eventIDToEventState[eventTypeVal];
            if (eventState.eventDataID != eventDataID)
                return false;

            auto itr = eventState.eventVariantToFuncRef.find(variantID);
            if (itr == eventState.eventVariantToFuncRef.end())
                return false;

            auto& funcRefList = itr->second;
            if (funcRefList.empty())
                return false;

            u32 packedEventID = static_cast<u32>(eventTypeVal) | (static_cast<u32>(eventTypeID) << 16);
            bool hasParameters = EventDataType::NumParameters > 0;
            u32 numParametersToPush = 1 + (1 * hasParameters); // 1 for eventID, 1 for eventData if it has parameters

            Push((u32)packedEventID);

            if (eventState.callback)
            {
                eventState.callback(this, &eventData);
            }
            else
            {
                eventData.Push(state);
            }

            CallAllFunctions(funcRefList, numParametersToPush, false);

            return true;
        }

        template <LuaEventTypeConcept EventType, LuaEventDataConcept EventDataType>
        bool CallEventBool(EventType eventType, EventDataType&& eventData, u16 variantID = 0)
        {
            u16 eventTypeID = EnumTraits<EventType>::Meta::EnumID;
            u16 eventDataID = std::decay_t<EventDataType>::StructID;
            u16 eventTypeVal = static_cast<u16>(eventType);

            auto& eventTypeMap = eventState.eventTypeToState;
            if (!eventTypeMap.contains(eventTypeID))
                return false;

            EventTypeState& eventTypeState = eventTypeMap[eventTypeID];
            if (!eventTypeState.eventIDToEventState.contains(eventTypeVal))
                return false;

            EventState& eventState = eventTypeState.eventIDToEventState[eventTypeVal];
            if (eventState.eventDataID != eventDataID)
                return false;

            auto itr = eventState.eventVariantToFuncRef.find(variantID);
            if (itr == eventState.eventVariantToFuncRef.end())
                return false;

            auto& funcRefList = itr->second;
            if (funcRefList.empty())
                return false;

            u32 packedEventID = static_cast<u32>(eventTypeVal) | (static_cast<u32>(eventTypeID) << 16);
            bool hasParameters = EventDataType::NumParameters > 0;
            u32 numParametersToPush = 1 + (1 * hasParameters); // 1 for eventID, 1 for eventData if it has parameters

            Push((u32)packedEventID);
            if (eventState.callback)
            {
                eventState.callback(this, &eventData);
            }
            else
            {
                eventData.Push(state);
            }

            bool result = CallAllFunctionsBool(funcRefList, numParametersToPush);
            return result;
        }

    private: // Internal Helpers

        // Wrappers to get rid of GCC dependency on lua.h in this header
        void* AllocateUserData(lua_State* state, size_t size, LuaUserDataDtor dtor);

    private:
        bool _isDirty = false;

    public:
        ZenithInfoKey key;
        lua_State* state;
        ZenithEventState eventState;
        robin_hood::unordered_set<u32> loadedModuleHashes;
    };


    template<> bool Zenith::GetGlobalField<bool>(const char* key);
    template<> i32 Zenith::GetGlobalField<i32>(const char* key);
    template<> u32 Zenith::GetGlobalField<u32>(const char* key);
    template<> f32 Zenith::GetGlobalField<f32>(const char* key);
    template<> f64 Zenith::GetGlobalField<f64>(const char* key);
    template<> const char* Zenith::GetGlobalField<const char*>(const char* key);
    template<> vec3 Zenith::GetGlobalField<vec3>(const char* key);

    template<> void Zenith::Push<vec2>(const vec2& value);
    template<> void Zenith::Push<ivec2>(const ivec2& value);
    template<> void Zenith::Push<uvec2>(const uvec2& value);
    template<> void Zenith::Push<vec3>(const vec3& value);
    template<> void Zenith::Push<ivec3>(const ivec3& value);
    template<> void Zenith::Push<uvec3>(const uvec3& value);
    template<> void Zenith::Push<::std::string>(const ::std::string& value);

    template<> bool Zenith::Get<bool>(i32 index);
    template<> i32 Zenith::Get<i32>(i32 index);
    template<> u32 Zenith::Get<u32>(i32 index);
    template<> f32 Zenith::Get<f32>(i32 index);
    template<> f64 Zenith::Get<f64>(i32 index);
    template<> const char* Zenith::Get<const char*>(i32 index);
    template<> vec3 Zenith::Get<vec3>(i32 index);

    template<> bool Zenith::CheckVal<bool>(i32 index);
    template<> i8 Zenith::CheckVal<i8>(i32 index);
    template<> i16 Zenith::CheckVal<i16>(i32 index);
    template<> i32 Zenith::CheckVal<i32>(i32 index);
    template<> i64 Zenith::CheckVal<i64>(i32 index);
    template<> u8 Zenith::CheckVal<u8>(i32 index);
    template<> u16 Zenith::CheckVal<u16>(i32 index);
    template<> u32 Zenith::CheckVal<u32>(i32 index);
    template<> u64 Zenith::CheckVal<u64>(i32 index);
    template<> f32 Zenith::CheckVal<f32>(i32 index);
    template<> f64 Zenith::CheckVal<f64>(i32 index);
    template<> const char* Zenith::CheckVal<const char*>(i32 index);
    template<> vec3 Zenith::CheckVal<vec3>(i32 index);
}

namespace std
{
    template <>
    struct hash<Scripting::ZenithInfoKey>
    {
        size_t operator()(const Scripting::ZenithInfoKey& key) const
        {
            return key.value;
        }
    };
}

namespace Scripting
{
    struct ZenithBytecodeEntry
    {
    public:
        u32 hash = std::numeric_limits<u32>().max();
        const std::string fileName;
        const std::string filePath;
        const std::string bytecode;
    };

    struct ZenithBytecodeCache
    {
    public:
        ZenithBytecodeCache()
        {
            bytecodeList.reserve(1024);
            apiBytecodeList.reserve(1024);
        }

    public:
        std::vector<ZenithBytecodeEntry> bytecodeList;
        std::vector<ZenithBytecodeEntry> apiBytecodeList;
        robin_hood::unordered_map<u32, u32> apiHashToBytecodeIndex;
    };
    struct ZenithStateManager
    {
    public:
        bool Add(const ZenithInfoKey& key);
        bool Remove(const ZenithInfoKey& key);

        bool Add(const ZenithInfoKey& key, const lua_State* state);
        bool Remove(const lua_State* state);

        Zenith* Get(const ZenithInfoKey& key);
        Zenith* Get(lua_State* state);

        ZenithInfoKey GetKey(lua_State* state);

        auto begin() { return _keyToZenithMap.begin(); }
        auto end() { return _keyToZenithMap.end(); }
        size_t size() const { return _keyToZenithMap.size(); }

        ZenithBytecodeCache& GetBytecodeCache() { return _bytecodeCache; }

    private:
        robin_hood::unordered_map<ZenithInfoKey, std::unique_ptr<Zenith>> _keyToZenithMap;
        robin_hood::unordered_map<u64, ZenithInfoKey> _luaStateToKey;
        ZenithBytecodeCache _bytecodeCache;
    };
}
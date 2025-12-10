#pragma once

#include <Array>
#include <lua.h>
#include <Base/Types.h>

namespace MetaGen::Game::Lua
{
    enum class LuaHandlerTypeEnum : u16
    {
        Global = 0,
        Event = 1,
        Database = 2,
        UI = 3,
        Game = 4,
        Unit = 5,
        Count = 6
    };
    struct LuaHandlerTypeEnumMeta
    {
        using Type = u16;

        static constexpr u16 ENUM_ID = 0;
        static constexpr std::string_view ENUM_NAME = "LuaHandlerTypeEnum";
        static constexpr std::array<std::pair<std::string_view, u16>, 7> ENUM_FIELD_LIST = { std::pair("Global", 0u), std::pair("Event", 1u), std::pair("Database", 2u), std::pair("UI", 3u), std::pair("Game", 4u), std::pair("Unit", 5u), std::pair("Count", 6u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(LuaHandlerTypeEnum);

    enum class GameEvent : u8
    {
        Invalid = 0,
        Loaded = 1,
        Updated = 2,
        CharacterListChanged = 3,
        MapLoading = 4,
        ChatMessageReceived = 5,
        LocalMoverChanged = 6,
        Count = 7
    };
    struct GameEventMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 1;
        static constexpr std::string_view ENUM_NAME = "GameEvent";
        static constexpr std::array<std::pair<std::string_view, u8>, 8> ENUM_FIELD_LIST = { std::pair("Invalid", 0u), std::pair("Loaded", 1u), std::pair("Updated", 2u), std::pair("CharacterListChanged", 3u), std::pair("MapLoading", 4u), std::pair("ChatMessageReceived", 5u), std::pair("LocalMoverChanged", 6u), std::pair("Count", 7u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(GameEvent);

    enum class UnitEvent : u8
    {
        Invalid = 0,
        Add = 1,
        Remove = 2,
        TargetChanged = 3,
        PowerUpdate = 4,
        ResistanceUpdate = 5,
        StatUpdate = 6,
        AuraAdd = 7,
        AuraUpdate = 8,
        AuraRemove = 9,
        Count = 10
    };
    struct UnitEventMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 2;
        static constexpr std::string_view ENUM_NAME = "UnitEvent";
        static constexpr std::array<std::pair<std::string_view, u8>, 11> ENUM_FIELD_LIST = { std::pair("Invalid", 0u), std::pair("Add", 1u), std::pair("Remove", 2u), std::pair("TargetChanged", 3u), std::pair("PowerUpdate", 4u), std::pair("ResistanceUpdate", 5u), std::pair("StatUpdate", 6u), std::pair("AuraAdd", 7u), std::pair("AuraUpdate", 8u), std::pair("AuraRemove", 9u), std::pair("Count", 10u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(UnitEvent);

    enum class ContainerEvent : u8
    {
        Invalid = 0,
        Add = 1,
        AddToSlot = 2,
        RemoveFromSlot = 3,
        SwapSlots = 4,
        Count = 5
    };
    struct ContainerEventMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 3;
        static constexpr std::string_view ENUM_NAME = "ContainerEvent";
        static constexpr std::array<std::pair<std::string_view, u8>, 6> ENUM_FIELD_LIST = { std::pair("Invalid", 0u), std::pair("Add", 1u), std::pair("AddToSlot", 2u), std::pair("RemoveFromSlot", 3u), std::pair("SwapSlots", 4u), std::pair("Count", 5u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(ContainerEvent);

    enum class TriggerEvent : u8
    {
        Invalid = 0,
        OnEnter = 1,
        OnExit = 2,
        OnStay = 3,
        Count = 4
    };
    struct TriggerEventMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 4;
        static constexpr std::string_view ENUM_NAME = "TriggerEvent";
        static constexpr std::array<std::pair<std::string_view, u8>, 5> ENUM_FIELD_LIST = { std::pair("Invalid", 0u), std::pair("OnEnter", 1u), std::pair("OnExit", 2u), std::pair("OnStay", 3u), std::pair("Count", 4u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(TriggerEvent);

    struct GameEventDataLoaded
    {
    public:
        std::string motd;

    public:
        static inline std::string NAME = "GameEventDataLoaded";
        static constexpr u32 NAME_HASH = 899233088;
        static constexpr u16 STRUCT_ID = 1;
        static constexpr u16 NUM_PARAMETERS = 1;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "motd");
            lua_pushstring(state, motd.c_str());
            lua_settable(state, -3);
        }
    };

    struct GameEventDataUpdated
    {
    public:
        f32 deltaTime;

    public:
        static inline std::string NAME = "GameEventDataUpdated";
        static constexpr u32 NAME_HASH = 864565456;
        static constexpr u16 STRUCT_ID = 2;
        static constexpr u16 NUM_PARAMETERS = 1;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "deltaTime");
            lua_pushnumber(state, deltaTime);
            lua_settable(state, -3);
        }
    };

    struct GameEventDataCharacterListChanged
    {
    public:
        static inline std::string NAME = "GameEventDataCharacterListChanged";
        static constexpr u32 NAME_HASH = 3529411444;
        static constexpr u16 STRUCT_ID = 3;
        static constexpr u16 NUM_PARAMETERS = 0;

        void Push(lua_State* state) const
        {
            lua_newtable(state);
        }
    };

    struct GameEventDataMapLoading
    {
    public:
        std::string mapInternalName;

    public:
        static inline std::string NAME = "GameEventDataMapLoading";
        static constexpr u32 NAME_HASH = 3662037065;
        static constexpr u16 STRUCT_ID = 4;
        static constexpr u16 NUM_PARAMETERS = 1;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "mapInternalName");
            lua_pushstring(state, mapInternalName.c_str());
            lua_settable(state, -3);
        }
    };

    struct GameEventDataChatMessageReceived
    {
    public:
        std::string sender;
        std::string channel;
        std::string message;

    public:
        static inline std::string NAME = "GameEventDataChatMessageReceived";
        static constexpr u32 NAME_HASH = 3510514491;
        static constexpr u16 STRUCT_ID = 5;
        static constexpr u16 NUM_PARAMETERS = 3;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "sender");
            lua_pushstring(state, sender.c_str());
            lua_settable(state, -3);

            lua_pushstring(state, "channel");
            lua_pushstring(state, channel.c_str());
            lua_settable(state, -3);

            lua_pushstring(state, "message");
            lua_pushstring(state, message.c_str());
            lua_settable(state, -3);
        }
    };

    struct GameEventDataLocalMoverChanged
    {
    public:
        u32 moverID;

    public:
        static inline std::string NAME = "GameEventDataLocalMoverChanged";
        static constexpr u32 NAME_HASH = 4192981633;
        static constexpr u16 STRUCT_ID = 6;
        static constexpr u16 NUM_PARAMETERS = 1;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "moverID");
            lua_pushnumber(state, moverID);
            lua_settable(state, -3);
        }
    };

    struct UnitEventDataAdd
    {
    public:
        u32 unitID;

    public:
        static inline std::string NAME = "UnitEventDataAdd";
        static constexpr u32 NAME_HASH = 3665733852;
        static constexpr u16 STRUCT_ID = 7;
        static constexpr u16 NUM_PARAMETERS = 1;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "unitID");
            lua_pushnumber(state, unitID);
            lua_settable(state, -3);
        }
    };

    struct UnitEventDataRemove
    {
    public:
        u32 unitID;

    public:
        static inline std::string NAME = "UnitEventDataRemove";
        static constexpr u32 NAME_HASH = 1179462279;
        static constexpr u16 STRUCT_ID = 8;
        static constexpr u16 NUM_PARAMETERS = 1;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "unitID");
            lua_pushnumber(state, unitID);
            lua_settable(state, -3);
        }
    };

    struct UnitEventDataTargetChanged
    {
    public:
        u32 unitID;
        u32 targetID;

    public:
        static inline std::string NAME = "UnitEventDataTargetChanged";
        static constexpr u32 NAME_HASH = 3048880068;
        static constexpr u16 STRUCT_ID = 9;
        static constexpr u16 NUM_PARAMETERS = 2;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "unitID");
            lua_pushnumber(state, unitID);
            lua_settable(state, -3);

            lua_pushstring(state, "targetID");
            lua_pushnumber(state, targetID);
            lua_settable(state, -3);
        }
    };

    struct UnitEventDataPowerUpdate
    {
    public:
        u32 unitID;
        u8 powerType;
        f64 base;
        f64 current;
        f64 max;

    public:
        static inline std::string NAME = "UnitEventDataPowerUpdate";
        static constexpr u32 NAME_HASH = 170205789;
        static constexpr u16 STRUCT_ID = 10;
        static constexpr u16 NUM_PARAMETERS = 5;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "unitID");
            lua_pushnumber(state, unitID);
            lua_settable(state, -3);

            lua_pushstring(state, "powerType");
            lua_pushnumber(state, powerType);
            lua_settable(state, -3);

            lua_pushstring(state, "base");
            lua_pushnumber(state, base);
            lua_settable(state, -3);

            lua_pushstring(state, "current");
            lua_pushnumber(state, current);
            lua_settable(state, -3);

            lua_pushstring(state, "max");
            lua_pushnumber(state, max);
            lua_settable(state, -3);
        }
    };

    struct UnitEventDataResistanceUpdate
    {
    public:
        u32 unitID;
        u8 resistanceType;
        f64 base;
        f64 current;
        f64 max;

    public:
        static inline std::string NAME = "UnitEventDataResistanceUpdate";
        static constexpr u32 NAME_HASH = 2048383187;
        static constexpr u16 STRUCT_ID = 11;
        static constexpr u16 NUM_PARAMETERS = 5;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "unitID");
            lua_pushnumber(state, unitID);
            lua_settable(state, -3);

            lua_pushstring(state, "resistanceType");
            lua_pushnumber(state, resistanceType);
            lua_settable(state, -3);

            lua_pushstring(state, "base");
            lua_pushnumber(state, base);
            lua_settable(state, -3);

            lua_pushstring(state, "current");
            lua_pushnumber(state, current);
            lua_settable(state, -3);

            lua_pushstring(state, "max");
            lua_pushnumber(state, max);
            lua_settable(state, -3);
        }
    };

    struct UnitEventDataStatUpdate
    {
    public:
        u32 unitID;
        u8 statType;
        f64 base;
        f64 current;

    public:
        static inline std::string NAME = "UnitEventDataStatUpdate";
        static constexpr u32 NAME_HASH = 3614547218;
        static constexpr u16 STRUCT_ID = 12;
        static constexpr u16 NUM_PARAMETERS = 4;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "unitID");
            lua_pushnumber(state, unitID);
            lua_settable(state, -3);

            lua_pushstring(state, "statType");
            lua_pushnumber(state, statType);
            lua_settable(state, -3);

            lua_pushstring(state, "base");
            lua_pushnumber(state, base);
            lua_settable(state, -3);

            lua_pushstring(state, "current");
            lua_pushnumber(state, current);
            lua_settable(state, -3);
        }
    };

    struct UnitEventDataAuraAdd
    {
    public:
        u32 unitID;
        u32 auraID;
        u32 spellID;
        f32 duration;
        u16 stacks;

    public:
        static inline std::string NAME = "UnitEventDataAuraAdd";
        static constexpr u32 NAME_HASH = 2429153799;
        static constexpr u16 STRUCT_ID = 13;
        static constexpr u16 NUM_PARAMETERS = 5;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "unitID");
            lua_pushnumber(state, unitID);
            lua_settable(state, -3);

            lua_pushstring(state, "auraID");
            lua_pushnumber(state, auraID);
            lua_settable(state, -3);

            lua_pushstring(state, "spellID");
            lua_pushnumber(state, spellID);
            lua_settable(state, -3);

            lua_pushstring(state, "duration");
            lua_pushnumber(state, duration);
            lua_settable(state, -3);

            lua_pushstring(state, "stacks");
            lua_pushnumber(state, stacks);
            lua_settable(state, -3);
        }
    };

    struct UnitEventDataAuraUpdate
    {
    public:
        u32 unitID;
        u32 auraID;
        f32 duration;
        u16 stacks;

    public:
        static inline std::string NAME = "UnitEventDataAuraUpdate";
        static constexpr u32 NAME_HASH = 2171549957;
        static constexpr u16 STRUCT_ID = 14;
        static constexpr u16 NUM_PARAMETERS = 4;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "unitID");
            lua_pushnumber(state, unitID);
            lua_settable(state, -3);

            lua_pushstring(state, "auraID");
            lua_pushnumber(state, auraID);
            lua_settable(state, -3);

            lua_pushstring(state, "duration");
            lua_pushnumber(state, duration);
            lua_settable(state, -3);

            lua_pushstring(state, "stacks");
            lua_pushnumber(state, stacks);
            lua_settable(state, -3);
        }
    };

    struct UnitEventDataAuraRemove
    {
    public:
        u32 unitID;
        u32 auraID;

    public:
        static inline std::string NAME = "UnitEventDataAuraRemove";
        static constexpr u32 NAME_HASH = 2962661358;
        static constexpr u16 STRUCT_ID = 15;
        static constexpr u16 NUM_PARAMETERS = 2;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "unitID");
            lua_pushnumber(state, unitID);
            lua_settable(state, -3);

            lua_pushstring(state, "auraID");
            lua_pushnumber(state, auraID);
            lua_settable(state, -3);
        }
    };

    struct ContainerEventDataAdd
    {
    public:
        u32 index;
        u32 numSlots;
        u32 itemID;

    public:
        static inline std::string NAME = "ContainerEventDataAdd";
        static constexpr u32 NAME_HASH = 2931908545;
        static constexpr u16 STRUCT_ID = 16;
        static constexpr u16 NUM_PARAMETERS = 3;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "index");
            lua_pushnumber(state, index);
            lua_settable(state, -3);

            lua_pushstring(state, "numSlots");
            lua_pushnumber(state, numSlots);
            lua_settable(state, -3);

            lua_pushstring(state, "itemID");
            lua_pushnumber(state, itemID);
            lua_settable(state, -3);
        }
    };

    struct ContainerEventDataAddToSlot
    {
    public:
        u32 containerIndex;
        u32 slotIndex;
        u32 itemID;
        u32 count;

    public:
        static inline std::string NAME = "ContainerEventDataAddToSlot";
        static constexpr u32 NAME_HASH = 2493328694;
        static constexpr u16 STRUCT_ID = 17;
        static constexpr u16 NUM_PARAMETERS = 4;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "containerIndex");
            lua_pushnumber(state, containerIndex);
            lua_settable(state, -3);

            lua_pushstring(state, "slotIndex");
            lua_pushnumber(state, slotIndex);
            lua_settable(state, -3);

            lua_pushstring(state, "itemID");
            lua_pushnumber(state, itemID);
            lua_settable(state, -3);

            lua_pushstring(state, "count");
            lua_pushnumber(state, count);
            lua_settable(state, -3);
        }
    };

    struct ContainerEventDataRemoveFromSlot
    {
    public:
        u32 containerIndex;
        u32 slotIndex;

    public:
        static inline std::string NAME = "ContainerEventDataRemoveFromSlot";
        static constexpr u32 NAME_HASH = 2022385476;
        static constexpr u16 STRUCT_ID = 18;
        static constexpr u16 NUM_PARAMETERS = 2;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "containerIndex");
            lua_pushnumber(state, containerIndex);
            lua_settable(state, -3);

            lua_pushstring(state, "slotIndex");
            lua_pushnumber(state, slotIndex);
            lua_settable(state, -3);
        }
    };

    struct ContainerEventDataSwapSlots
    {
    public:
        u32 srcContainerIndex;
        u32 destContainerIndex;
        u32 srcSlotIndex;
        u32 destSlotIndex;

    public:
        static inline std::string NAME = "ContainerEventDataSwapSlots";
        static constexpr u32 NAME_HASH = 2154505540;
        static constexpr u16 STRUCT_ID = 19;
        static constexpr u16 NUM_PARAMETERS = 4;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "srcContainerIndex");
            lua_pushnumber(state, srcContainerIndex);
            lua_settable(state, -3);

            lua_pushstring(state, "destContainerIndex");
            lua_pushnumber(state, destContainerIndex);
            lua_settable(state, -3);

            lua_pushstring(state, "srcSlotIndex");
            lua_pushnumber(state, srcSlotIndex);
            lua_settable(state, -3);

            lua_pushstring(state, "destSlotIndex");
            lua_pushnumber(state, destSlotIndex);
            lua_settable(state, -3);
        }
    };

    struct TriggerEventDataOnEnter
    {
    public:
        u32 triggerID;
        u32 playerID;

    public:
        static inline std::string NAME = "TriggerEventDataOnEnter";
        static constexpr u32 NAME_HASH = 1955843646;
        static constexpr u16 STRUCT_ID = 20;
        static constexpr u16 NUM_PARAMETERS = 2;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "triggerID");
            lua_pushnumber(state, triggerID);
            lua_settable(state, -3);

            lua_pushstring(state, "playerID");
            lua_pushnumber(state, playerID);
            lua_settable(state, -3);
        }
    };

    struct TriggerEventDataOnExit
    {
    public:
        u32 triggerID;
        u32 playerID;

    public:
        static inline std::string NAME = "TriggerEventDataOnExit";
        static constexpr u32 NAME_HASH = 763368532;
        static constexpr u16 STRUCT_ID = 21;
        static constexpr u16 NUM_PARAMETERS = 2;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "triggerID");
            lua_pushnumber(state, triggerID);
            lua_settable(state, -3);

            lua_pushstring(state, "playerID");
            lua_pushnumber(state, playerID);
            lua_settable(state, -3);
        }
    };

    struct TriggerEventDataOnStay
    {
    public:
        u32 triggerID;
        u32 playerID;

    public:
        static inline std::string NAME = "TriggerEventDataOnStay";
        static constexpr u32 NAME_HASH = 1262119215;
        static constexpr u16 STRUCT_ID = 22;
        static constexpr u16 NUM_PARAMETERS = 2;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "triggerID");
            lua_pushnumber(state, triggerID);
            lua_settable(state, -3);

            lua_pushstring(state, "playerID");
            lua_pushnumber(state, playerID);
            lua_settable(state, -3);
        }
    };
}
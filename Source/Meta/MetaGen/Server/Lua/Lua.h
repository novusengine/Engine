#pragma once

#include <Array>
#include <lua.h>
#include <Base/Types.h>

namespace MetaGen::Server::Lua
{
    enum class LuaHandlerTypeEnum : u16
    {
        Global = 0,
        Event = 1,
        Message = 2,
        Unit = 3,
        Character = 4,
        Spell = 5,
        Item = 6,
        Creature = 7,
        Count = 8
    };
    struct LuaHandlerTypeEnumMeta
    {
        using Type = u16;

        static constexpr u16 ENUM_ID = 5;
        static constexpr std::string_view ENUM_NAME = "LuaHandlerTypeEnum";
        static constexpr std::array<std::pair<std::string_view, u16>, 9> ENUM_FIELD_LIST = { std::pair("Global", 0u), std::pair("Event", 1u), std::pair("Message", 2u), std::pair("Unit", 3u), std::pair("Character", 4u), std::pair("Spell", 5u), std::pair("Item", 6u), std::pair("Creature", 7u), std::pair("Count", 8u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(LuaHandlerTypeEnum);

    enum class ServerEvent : u8
    {
        Invalid = 0,
        Loaded = 1,
        Updated = 2,
        Count = 3
    };
    struct ServerEventMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 6;
        static constexpr std::string_view ENUM_NAME = "ServerEvent";
        static constexpr std::array<std::pair<std::string_view, u8>, 4> ENUM_FIELD_LIST = { std::pair("Invalid", 0u), std::pair("Loaded", 1u), std::pair("Updated", 2u), std::pair("Count", 3u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(ServerEvent);

    enum class CharacterEvent : u8
    {
        Invalid = 0,
        OnLogin = 1,
        OnLogout = 2,
        OnWorldChanged = 3,
        Count = 4
    };
    struct CharacterEventMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 7;
        static constexpr std::string_view ENUM_NAME = "CharacterEvent";
        static constexpr std::array<std::pair<std::string_view, u8>, 5> ENUM_FIELD_LIST = { std::pair("Invalid", 0u), std::pair("OnLogin", 1u), std::pair("OnLogout", 2u), std::pair("OnWorldChanged", 3u), std::pair("Count", 4u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(CharacterEvent);

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

        static constexpr u16 ENUM_ID = 8;
        static constexpr std::string_view ENUM_NAME = "TriggerEvent";
        static constexpr std::array<std::pair<std::string_view, u8>, 5> ENUM_FIELD_LIST = { std::pair("Invalid", 0u), std::pair("OnEnter", 1u), std::pair("OnExit", 2u), std::pair("OnStay", 3u), std::pair("Count", 4u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(TriggerEvent);

    enum class SpellEvent : u8
    {
        Invalid = 0,
        OnPrepare = 1,
        OnHandleEffect = 2,
        OnFinish = 3,
        Count = 4
    };
    struct SpellEventMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 9;
        static constexpr std::string_view ENUM_NAME = "SpellEvent";
        static constexpr std::array<std::pair<std::string_view, u8>, 5> ENUM_FIELD_LIST = { std::pair("Invalid", 0u), std::pair("OnPrepare", 1u), std::pair("OnHandleEffect", 2u), std::pair("OnFinish", 3u), std::pair("Count", 4u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(SpellEvent);

    enum class AuraEvent : u8
    {
        Invalid = 0,
        OnApply = 1,
        OnRemove = 2,
        OnHandleEffect = 3,
        Count = 4
    };
    struct AuraEventMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 10;
        static constexpr std::string_view ENUM_NAME = "AuraEvent";
        static constexpr std::array<std::pair<std::string_view, u8>, 5> ENUM_FIELD_LIST = { std::pair("Invalid", 0u), std::pair("OnApply", 1u), std::pair("OnRemove", 2u), std::pair("OnHandleEffect", 3u), std::pair("Count", 4u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(AuraEvent);

    enum class CreatureAIEvent : u8
    {
        Invalid = 0,
        OnInit = 1,
        OnDeinit = 2,
        OnEnterCombat = 3,
        OnLeaveCombat = 4,
        OnUpdate = 5,
        OnResurrect = 6,
        OnDied = 7,
        Count = 8
    };
    struct CreatureAIEventMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 11;
        static constexpr std::string_view ENUM_NAME = "CreatureAIEvent";
        static constexpr std::array<std::pair<std::string_view, u8>, 9> ENUM_FIELD_LIST = { std::pair("Invalid", 0u), std::pair("OnInit", 1u), std::pair("OnDeinit", 2u), std::pair("OnEnterCombat", 3u), std::pair("OnLeaveCombat", 4u), std::pair("OnUpdate", 5u), std::pair("OnResurrect", 6u), std::pair("OnDied", 7u), std::pair("Count", 8u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(CreatureAIEvent);

    struct ServerEventDataLoaded
    {
    public:
        std::string motd;

    public:
        static inline std::string NAME = "ServerEventDataLoaded";
        static constexpr u32 NAME_HASH = 2853609597;
        static constexpr u16 STRUCT_ID = 23;
        static constexpr u16 NUM_PARAMETERS = 1;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "motd");
            lua_pushstring(state, motd.c_str());
            lua_settable(state, -3);
        }
    };

    struct ServerEventDataUpdated
    {
    public:
        f32 deltaTime;

    public:
        static inline std::string NAME = "ServerEventDataUpdated";
        static constexpr u32 NAME_HASH = 2515502827;
        static constexpr u16 STRUCT_ID = 24;
        static constexpr u16 NUM_PARAMETERS = 1;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "deltaTime");
            lua_pushnumber(state, deltaTime);
            lua_settable(state, -3);
        }
    };

    struct CharacterEventDataOnLogin
    {
    public:
        u32 characterEntity;

    public:
        static inline std::string NAME = "CharacterEventDataOnLogin";
        static constexpr u32 NAME_HASH = 4139954922;
        static constexpr u16 STRUCT_ID = 25;
        static constexpr u16 NUM_PARAMETERS = 1;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "characterEntity");
            lua_pushnumber(state, characterEntity);
            lua_settable(state, -3);
        }
    };

    struct CharacterEventDataOnLogout
    {
    public:
        u32 characterEntity;

    public:
        static inline std::string NAME = "CharacterEventDataOnLogout";
        static constexpr u32 NAME_HASH = 3903045469;
        static constexpr u16 STRUCT_ID = 26;
        static constexpr u16 NUM_PARAMETERS = 1;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "characterEntity");
            lua_pushnumber(state, characterEntity);
            lua_settable(state, -3);
        }
    };

    struct CharacterEventDataOnWorldChanged
    {
    public:
        u32 characterEntity;

    public:
        static inline std::string NAME = "CharacterEventDataOnWorldChanged";
        static constexpr u32 NAME_HASH = 785031623;
        static constexpr u16 STRUCT_ID = 27;
        static constexpr u16 NUM_PARAMETERS = 1;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "characterEntity");
            lua_pushnumber(state, characterEntity);
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
        static constexpr u16 STRUCT_ID = 28;
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
        static constexpr u16 STRUCT_ID = 29;
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
        static constexpr u16 STRUCT_ID = 30;
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

    struct SpellEventDataOnPrepare
    {
    public:
        u32 casterID;
        u32 spellEntity;
        u32 spellID;

    public:
        static inline std::string NAME = "SpellEventDataOnPrepare";
        static constexpr u32 NAME_HASH = 1031983103;
        static constexpr u16 STRUCT_ID = 31;
        static constexpr u16 NUM_PARAMETERS = 3;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "casterID");
            lua_pushnumber(state, casterID);
            lua_settable(state, -3);

            lua_pushstring(state, "spellEntity");
            lua_pushnumber(state, spellEntity);
            lua_settable(state, -3);

            lua_pushstring(state, "spellID");
            lua_pushnumber(state, spellID);
            lua_settable(state, -3);
        }
    };

    struct SpellEventDataOnHandleEffect
    {
    public:
        u32 casterID;
        u32 spellEntity;
        u32 spellID;
        u32 procID;
        u8 effectIndex;
        u8 effectType;

    public:
        static inline std::string NAME = "SpellEventDataOnHandleEffect";
        static constexpr u32 NAME_HASH = 3312996611;
        static constexpr u16 STRUCT_ID = 32;
        static constexpr u16 NUM_PARAMETERS = 6;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "casterID");
            lua_pushnumber(state, casterID);
            lua_settable(state, -3);

            lua_pushstring(state, "spellEntity");
            lua_pushnumber(state, spellEntity);
            lua_settable(state, -3);

            lua_pushstring(state, "spellID");
            lua_pushnumber(state, spellID);
            lua_settable(state, -3);

            lua_pushstring(state, "procID");
            lua_pushnumber(state, procID);
            lua_settable(state, -3);

            lua_pushstring(state, "effectIndex");
            lua_pushnumber(state, effectIndex);
            lua_settable(state, -3);

            lua_pushstring(state, "effectType");
            lua_pushnumber(state, effectType);
            lua_settable(state, -3);
        }
    };

    struct SpellEventDataOnFinish
    {
    public:
        u32 casterID;
        u32 spellEntity;
        u32 spellID;

    public:
        static inline std::string NAME = "SpellEventDataOnFinish";
        static constexpr u32 NAME_HASH = 1438697475;
        static constexpr u16 STRUCT_ID = 33;
        static constexpr u16 NUM_PARAMETERS = 3;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "casterID");
            lua_pushnumber(state, casterID);
            lua_settable(state, -3);

            lua_pushstring(state, "spellEntity");
            lua_pushnumber(state, spellEntity);
            lua_settable(state, -3);

            lua_pushstring(state, "spellID");
            lua_pushnumber(state, spellID);
            lua_settable(state, -3);
        }
    };

    struct AuraEventDataOnApply
    {
    public:
        u32 casterID;
        u32 targetID;
        u32 auraEntity;
        u32 spellID;

    public:
        static inline std::string NAME = "AuraEventDataOnApply";
        static constexpr u32 NAME_HASH = 3161739871;
        static constexpr u16 STRUCT_ID = 34;
        static constexpr u16 NUM_PARAMETERS = 4;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "casterID");
            lua_pushnumber(state, casterID);
            lua_settable(state, -3);

            lua_pushstring(state, "targetID");
            lua_pushnumber(state, targetID);
            lua_settable(state, -3);

            lua_pushstring(state, "auraEntity");
            lua_pushnumber(state, auraEntity);
            lua_settable(state, -3);

            lua_pushstring(state, "spellID");
            lua_pushnumber(state, spellID);
            lua_settable(state, -3);
        }
    };

    struct AuraEventDataOnRemove
    {
    public:
        u32 casterID;
        u32 targetID;
        u32 auraEntity;
        u32 spellID;

    public:
        static inline std::string NAME = "AuraEventDataOnRemove";
        static constexpr u32 NAME_HASH = 219944173;
        static constexpr u16 STRUCT_ID = 35;
        static constexpr u16 NUM_PARAMETERS = 4;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "casterID");
            lua_pushnumber(state, casterID);
            lua_settable(state, -3);

            lua_pushstring(state, "targetID");
            lua_pushnumber(state, targetID);
            lua_settable(state, -3);

            lua_pushstring(state, "auraEntity");
            lua_pushnumber(state, auraEntity);
            lua_settable(state, -3);

            lua_pushstring(state, "spellID");
            lua_pushnumber(state, spellID);
            lua_settable(state, -3);
        }
    };

    struct AuraEventDataOnHandleEffect
    {
    public:
        u32 casterID;
        u32 targetID;
        u32 auraEntity;
        u32 spellID;
        u32 procID;
        u8 effectIndex;
        u8 effectType;

    public:
        static inline std::string NAME = "AuraEventDataOnHandleEffect";
        static constexpr u32 NAME_HASH = 1344379436;
        static constexpr u16 STRUCT_ID = 36;
        static constexpr u16 NUM_PARAMETERS = 7;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "casterID");
            lua_pushnumber(state, casterID);
            lua_settable(state, -3);

            lua_pushstring(state, "targetID");
            lua_pushnumber(state, targetID);
            lua_settable(state, -3);

            lua_pushstring(state, "auraEntity");
            lua_pushnumber(state, auraEntity);
            lua_settable(state, -3);

            lua_pushstring(state, "spellID");
            lua_pushnumber(state, spellID);
            lua_settable(state, -3);

            lua_pushstring(state, "procID");
            lua_pushnumber(state, procID);
            lua_settable(state, -3);

            lua_pushstring(state, "effectIndex");
            lua_pushnumber(state, effectIndex);
            lua_settable(state, -3);

            lua_pushstring(state, "effectType");
            lua_pushnumber(state, effectType);
            lua_settable(state, -3);
        }
    };

    struct CreatureAIEventDataOnInit
    {
    public:
        u32 creatureEntity;
        u32 scriptNameHash;

    public:
        static inline std::string NAME = "CreatureAIEventDataOnInit";
        static constexpr u32 NAME_HASH = 1765857455;
        static constexpr u16 STRUCT_ID = 37;
        static constexpr u16 NUM_PARAMETERS = 2;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "creatureEntity");
            lua_pushnumber(state, creatureEntity);
            lua_settable(state, -3);

            lua_pushstring(state, "scriptNameHash");
            lua_pushnumber(state, scriptNameHash);
            lua_settable(state, -3);
        }
    };

    struct CreatureAIEventDataOnDeinit
    {
    public:
        u32 creatureEntity;

    public:
        static inline std::string NAME = "CreatureAIEventDataOnDeinit";
        static constexpr u32 NAME_HASH = 1744025678;
        static constexpr u16 STRUCT_ID = 38;
        static constexpr u16 NUM_PARAMETERS = 1;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "creatureEntity");
            lua_pushnumber(state, creatureEntity);
            lua_settable(state, -3);
        }
    };

    struct CreatureAIEventDataOnEnterCombat
    {
    public:
        u32 creatureEntity;

    public:
        static inline std::string NAME = "CreatureAIEventDataOnEnterCombat";
        static constexpr u32 NAME_HASH = 2684227205;
        static constexpr u16 STRUCT_ID = 39;
        static constexpr u16 NUM_PARAMETERS = 1;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "creatureEntity");
            lua_pushnumber(state, creatureEntity);
            lua_settable(state, -3);
        }
    };

    struct CreatureAIEventDataOnLeaveCombat
    {
    public:
        u32 creatureEntity;

    public:
        static inline std::string NAME = "CreatureAIEventDataOnLeaveCombat";
        static constexpr u32 NAME_HASH = 3569058646;
        static constexpr u16 STRUCT_ID = 40;
        static constexpr u16 NUM_PARAMETERS = 1;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "creatureEntity");
            lua_pushnumber(state, creatureEntity);
            lua_settable(state, -3);
        }
    };

    struct CreatureAIEventDataOnUpdate
    {
    public:
        u32 creatureEntity;
        f32 deltaTime;

    public:
        static inline std::string NAME = "CreatureAIEventDataOnUpdate";
        static constexpr u32 NAME_HASH = 947260894;
        static constexpr u16 STRUCT_ID = 41;
        static constexpr u16 NUM_PARAMETERS = 2;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "creatureEntity");
            lua_pushnumber(state, creatureEntity);
            lua_settable(state, -3);

            lua_pushstring(state, "deltaTime");
            lua_pushnumber(state, deltaTime);
            lua_settable(state, -3);
        }
    };

    struct CreatureAIEventDataOnResurrect
    {
    public:
        u32 creatureEntity;
        u32 resurrectorEntity;

    public:
        static inline std::string NAME = "CreatureAIEventDataOnResurrect";
        static constexpr u32 NAME_HASH = 2536967042;
        static constexpr u16 STRUCT_ID = 42;
        static constexpr u16 NUM_PARAMETERS = 2;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "creatureEntity");
            lua_pushnumber(state, creatureEntity);
            lua_settable(state, -3);

            lua_pushstring(state, "resurrectorEntity");
            lua_pushnumber(state, resurrectorEntity);
            lua_settable(state, -3);
        }
    };

    struct CreatureAIEventDataOnDied
    {
    public:
        u32 creatureEntity;
        u32 killerEntity;

    public:
        static inline std::string NAME = "CreatureAIEventDataOnDied";
        static constexpr u32 NAME_HASH = 2792891319;
        static constexpr u16 STRUCT_ID = 43;
        static constexpr u16 NUM_PARAMETERS = 2;

        void Push(lua_State* state) const
        {
            lua_newtable(state);

            lua_pushstring(state, "creatureEntity");
            lua_pushnumber(state, creatureEntity);
            lua_settable(state, -3);

            lua_pushstring(state, "killerEntity");
            lua_pushnumber(state, killerEntity);
            lua_settable(state, -3);
        }
    };
}
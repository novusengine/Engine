type LuaServerEvent<Enum> =
{
    Name : "ServerEvent";
    Type : u8;

    Fields :
    {
        Invalid : Auto,
        Loaded : Auto,
        Updated : Auto,
        Count : Auto
    };
}
type LuaServerEventDataLoaded<LuaStruct> =
{
    Fields :
    {
        motd : string
    };
}
type LuaServerEventDataUpdated<LuaStruct> =
{
    Fields :
    {
        deltaTime : f32
    };
}

type LuaCharacterEvent<Enum> =
{
    Name : "CharacterEvent";
    Type : u8;

    Fields :
    {
        Invalid : Auto,
        OnLogin : Auto,
        OnLogout : Auto,
        OnWorldChanged : Auto,
        Count : Auto
    };
}
type LuaCharacterEventDataOnLogin<LuaStruct> =
{
    Name : "CharacterEventDataOnLogin";

    Fields :
    {
        characterEntity : u32
    };
}
type LuaCharacterEventDataOnLogout<LuaStruct> =
{
    Name : "CharacterEventDataOnLogout";

    Fields :
    {
        characterEntity : u32
    };
}
type LuaCharacterEventDataOnWorldChanged<LuaStruct> =
{
    Name : "CharacterEventDataOnWorldChanged";

    Fields :
    {
        characterEntity : u32
    };
}

type LuaTriggerEvent<Enum> =
{
    Name : "TriggerEvent";
    Type : u8;

    Fields :
    {
        Invalid : Auto,
        OnEnter : Auto,
        OnExit : Auto,
        OnStay : Auto,
        Count : Auto
    };
}
type LuaTriggerEventDataOnEnter<LuaStruct> =
{
    Name : "TriggerEventDataOnEnter";

    Fields :
    {
        triggerID : u32,
        playerID : u32
    };
}
type LuaTriggerEventDataOnExit<LuaStruct> =
{
    Name : "TriggerEventDataOnExit";

    Fields :
    {
        triggerID : u32,
        playerID : u32
    };
}
type LuaTriggerEventDataOnStay<LuaStruct> =
{
    Name : "TriggerEventDataOnStay";

    Fields :
    {
        triggerID : u32,
        playerID : u32
    };
}

type LuaSpellEvent<Enum> =
{
    Name : "SpellEvent";
    Type : u8;

    Fields :
    {
        Invalid : Auto,
        OnPrepare : Auto,
        OnHandleEffect : Auto,
        Count : Auto
    };
}
type LuaSpellEventDataOnPrepare<LuaStruct> =
{
    Name : "SpellEventDataOnPrepare";

    Fields :
    {
        casterID : u32,
        spellEntity : u32,
        spellID : u32
    };
}
type LuaSpellEventDataOnHandleEffect<LuaStruct> =
{
    Name : "SpellEventDataOnHandleEffect";

    Fields :
    {
        casterID : u32,
        spellEntity : u32,
        spellID : u32,
        effectIndex : u8,
        effectType : u8
    };
}

type LuaCreatureAIEvent<Enum> =
{
    Name : "CreatureAIEvent";
    Type : u8;

    Fields :
    {
        Invalid : Auto,
        OnInit : Auto,
        OnDeinit : Auto,
        OnEnterCombat : Auto,
        OnLeaveCombat : Auto,
        OnUpdate : Auto,
        OnResurrect : Auto,
        OnDied : Auto,
        Count : Auto
    };
}
type LuaCreatureAIEventDataOnInit<LuaStruct> =
{
    Name : "CreatureAIEventDataOnInit";

    Fields :
    {
        creatureEntity : u32,
        scriptNameHash : u32
    };
}
type LuaCreatureAIEventDataOnDeinit<LuaStruct> =
{
    Name : "CreatureAIEventDataOnDeinit";

    Fields :
    {
        creatureEntity : u32
    };
}
type LuaCreatureAIEventDataOnEnterCombat<LuaStruct> =
{
    Name : "CreatureAIEventDataOnEnterCombat";

    Fields :
    {
        creatureEntity : u32
    };
}
type LuaCreatureAIEventDataOnLeaveCombat<LuaStruct> =
{
    Name : "CreatureAIEventDataOnLeaveCombat";

    Fields :
    {
        creatureEntity : u32
    };
}
type LuaCreatureAIEventDataOnUpdate<LuaStruct> =
{
    Name : "CreatureAIEventDataOnUpdate";

    Fields :
    {
        creatureEntity : u32,
        deltaTime : f32
    };
}
type LuaCreatureAIEventDataOnResurrect<LuaStruct> =
{
    Name : "CreatureAIEventDataOnResurrect";

    Fields :
    {
        creatureEntity : u32,
        resurrectorEntity : u32
    };
}
type LuaCreatureAIEventDataOnDied<LuaStruct> =
{
    Name : "CreatureAIEventDataOnDied";

    Fields :
    {
        creatureEntity : u32,
        killerEntity : u32
    };
}
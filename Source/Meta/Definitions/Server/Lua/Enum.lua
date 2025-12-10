local M = OrderedTable()

local Type = require("Type")
local Archetype = require("Archetype")
local Component = require("Component")

M.LuaHandlerTypeEnum =
{
    archetype = Archetype.LuaEnum,
    type = Type.U16,

    fields = 
    {
        Field("Global"),
        Field("Event"),
        Field("Message"),
        Field("Unit"),
        Field("Character"),
        Field("Spell"),
        Field("Item"),
        Field("Creature"),
        Field("Count")
    }
}

M.ServerEvent =
{
    archetype = Archetype.LuaEnum,
    type = Type.U8,

    fields = 
    {
        Field("Invalid"),
        Field("Loaded"),
        Field("Updated"),
        Field("Count")
    }
}

M.CharacterEvent =
{
    archetype = Archetype.LuaEnum,
    type = Type.U8,

    fields = 
    {
        Field("Invalid"),
        Field("OnLogin"),
        Field("OnLogout"),
        Field("OnWorldChanged"),
        Field("Count")
    }
}

M.TriggerEvent =
{
    archetype = Archetype.LuaEnum,
    type = Type.U8,

    fields = 
    {
        Field("Invalid"),
        Field("OnEnter"),
        Field("OnExit"),
        Field("OnStay"),
        Field("Count")
    }
}

M.SpellEvent =
{
    archetype = Archetype.LuaEnum,
    type = Type.U8,

    fields = 
    {
        Field("Invalid"),
        Field("OnPrepare"),
        Field("OnHandleEffect"),
        Field("OnFinish"),
        Field("Count")
    }
}

M.AuraEvent =
{
    archetype = Archetype.LuaEnum,
    type = Type.U8,

    fields = 
    {
        Field("Invalid"),
        Field("OnApply"),
        Field("OnRemove"),
        Field("OnHandleEffect"),
        Field("Count")
    }
}

M.CreatureAIEvent =
{
    archetype = Archetype.LuaEnum,
    type = Type.U8,

    fields = 
    {
        Field("Invalid"),
        Field("OnInit"),
        Field("OnDeinit"),
        Field("OnEnterCombat"),
        Field("OnLeaveCombat"),
        Field("OnUpdate"),
        Field("OnResurrect"),
        Field("OnDied"),
        Field("Count")
    }
}

return M
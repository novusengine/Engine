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
        Field("Database"),
        Field("UI"),
        Field("Game"),
        Field("Unit"),
        Field("Count")
    }
}

M.GameEvent =
{
    archetype = Archetype.LuaEnum,
    type = Type.U8,

    fields =
    {
        Field("Invalid"),
        Field("Loaded"),
        Field("Updated"),
        Field("CharacterListChanged"),
        Field("MapLoading"),
        Field("ChatMessageReceived"),
        Field("LocalMoverChanged"),
        Field("Count")
    }
}

M.UnitEvent =
{
    archetype = Archetype.LuaEnum,
    type = Type.U8,

    fields =
    {
        Field("Invalid"),
        Field("Add"),
        Field("Remove"),
        Field("TargetChanged"),
        Field("PowerUpdate"),
        Field("ResistanceUpdate"),
        Field("StatUpdate"),
        Field("AuraAdd"),
        Field("AuraUpdate"),
        Field("AuraRemove"),
        Field("Count")
    }
}

M.ContainerEvent =
{
    archetype = Archetype.LuaEnum,
    type = Type.U8,

    fields =
    {
        Field("Invalid"),
        Field("Add"),
        Field("AddToSlot"),
        Field("RemoveFromSlot"),
        Field("SwapSlots"),
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

return M
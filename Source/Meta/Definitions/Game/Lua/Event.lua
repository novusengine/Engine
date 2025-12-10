local M = OrderedTable()

local Type = require("Type")
local Archetype = require("Archetype")
local Component = require("Component")

M.GameEventDataLoaded =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("motd", Type.STRING)
    }
}

M.GameEventDataUpdated =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("deltaTime", Type.F32)
    }
}

M.GameEventDataCharacterListChanged =
{
    archetype = Archetype.LuaEventData,

    fields = {}
}

M.GameEventDataMapLoading =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("mapInternalName", Type.STRING)
    }
}

M.GameEventDataChatMessageReceived =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("sender", Type.STRING),
        Field("channel", Type.STRING),
        Field("message", Type.STRING)
    }
}

M.GameEventDataLocalMoverChanged =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("moverID", Type.U32)
    }
}

M.UnitEventDataAdd =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("unitID", Type.U32)
    }
}

M.UnitEventDataRemove =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("unitID", Type.U32)
    }
}

M.UnitEventDataTargetChanged =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("unitID", Type.U32),
        Field("targetID", Type.U32)
    }
}

M.UnitEventDataPowerUpdate =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("unitID", Type.U32),
        Field("powerType", Type.U8),
        Field("base", Type.F64),
        Field("current", Type.F64),
        Field("max", Type.F64)
    }
}

M.UnitEventDataResistanceUpdate =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("unitID", Type.U32),
        Field("resistanceType", Type.U8),
        Field("base", Type.F64),
        Field("current", Type.F64),
        Field("max", Type.F64)
    }
}

M.UnitEventDataStatUpdate =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("unitID", Type.U32),
        Field("statType", Type.U8),
        Field("base", Type.F64),
        Field("current", Type.F64)
    }
}

M.UnitEventDataAuraAdd =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("unitID", Type.U32),
        Field("auraID", Type.U32),
        Field("spellID", Type.U32),
        Field("duration", Type.F32),
        Field("stacks", Type.U16)
    }
}

M.UnitEventDataAuraUpdate =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("unitID", Type.U32),
        Field("auraID", Type.U32),
        Field("duration", Type.F32),
        Field("stacks", Type.U16)
    }
}

M.UnitEventDataAuraRemove =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("unitID", Type.U32),
        Field("auraID", Type.U32)
    }
}

M.ContainerEventDataAdd =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("index", Type.U32),
        Field("numSlots", Type.U32),
        Field("itemID", Type.U32)
    }
}

M.ContainerEventDataAddToSlot =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("containerIndex", Type.U32),
        Field("slotIndex", Type.U32),
        Field("itemID", Type.U32),
        Field("count", Type.U32)
    }
}

M.ContainerEventDataRemoveFromSlot =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("containerIndex", Type.U32),
        Field("slotIndex", Type.U32)
    }
}

M.ContainerEventDataSwapSlots =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("srcContainerIndex", Type.U32),
        Field("destContainerIndex", Type.U32),
        Field("srcSlotIndex", Type.U32),
        Field("destSlotIndex", Type.U32)
    }
}

M.TriggerEventDataOnEnter =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("triggerID", Type.U32),
        Field("playerID", Type.U32)
    }
}

M.TriggerEventDataOnExit =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("triggerID", Type.U32),
        Field("playerID", Type.U32)
    }
}

M.TriggerEventDataOnStay =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("triggerID", Type.U32),
        Field("playerID", Type.U32)
    }
}

return M
local M = OrderedTable()

local Type = require("Type")
local Archetype = require("Archetype")
local Component = require("Component")

M.ServerEventDataLoaded =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("motd", Type.STRING)
    }
}

M.ServerEventDataUpdated =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("deltaTime", Type.F32)
    }
}

M.CharacterEventDataOnLogin =
{
    archetype = Archetype.LuaEventData,

    fields =
    {
        Field("characterEntity", Type.U32)
    }
}

M.CharacterEventDataOnLogout =
{
    archetype = Archetype.LuaEventData,

    fields =
    {
        Field("characterEntity", Type.U32)
    }
}

M.CharacterEventDataOnWorldChanged =
{
    archetype = Archetype.LuaEventData,

    fields =
    {
        Field("characterEntity", Type.U32)
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

M.SpellEventDataOnPrepare =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("casterID", Type.U32),
        Field("spellEntity", Type.U32),
        Field("spellID", Type.U32)
    }
}

M.SpellEventDataOnHandleEffect =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("casterID", Type.U32),
        Field("spellEntity", Type.U32),
        Field("spellID", Type.U32),
        Field("procID", Type.U32),
        Field("effectIndex", Type.U8),
        Field("effectType", Type.U8)
    }
}

M.SpellEventDataOnFinish =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("casterID", Type.U32),
        Field("spellEntity", Type.U32),
        Field("spellID", Type.U32)
    }
}

M.AuraEventDataOnApply =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("casterID", Type.U32),
        Field("targetID", Type.U32),
        Field("auraEntity", Type.U32),
        Field("spellID", Type.U32)
    }
}

M.AuraEventDataOnRemove =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("casterID", Type.U32),
        Field("targetID", Type.U32),
        Field("auraEntity", Type.U32),
        Field("spellID", Type.U32)
    }
}

M.AuraEventDataOnHandleEffect =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("casterID", Type.U32),
        Field("targetID", Type.U32),
        Field("auraEntity", Type.U32),
        Field("spellID", Type.U32),
        Field("procID", Type.U32),
        Field("effectIndex", Type.U8),
        Field("effectType", Type.U8)
    }
}

M.CreatureAIEventDataOnInit =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("creatureEntity", Type.U32),
        Field("scriptNameHash", Type.U32)
    }
}

M.CreatureAIEventDataOnDeinit =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("creatureEntity", Type.U32)
    }
}

M.CreatureAIEventDataOnEnterCombat =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("creatureEntity", Type.U32)
    }
}

M.CreatureAIEventDataOnLeaveCombat =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("creatureEntity", Type.U32)
    }
}

M.CreatureAIEventDataOnUpdate =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("creatureEntity", Type.U32),
        Field("deltaTime", Type.F32)
    }
}

M.CreatureAIEventDataOnResurrect =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("creatureEntity", Type.U32),
        Field("resurrectorEntity", Type.U32)
    }
}

M.CreatureAIEventDataOnDied =
{
    archetype = Archetype.LuaEventData,

    fields = 
    {
        Field("creatureEntity", Type.U32),
        Field("killerEntity", Type.U32)
    }
}

return M
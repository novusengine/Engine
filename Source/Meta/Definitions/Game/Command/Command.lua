local M = OrderedTable()

local Type = require("Type")
local Archetype = require("Archetype")
local Component = require("Component")

M.HelpCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "help" },
    fields = {}
}

M.PingCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "ping" },
    fields = {}
}

M.LuaCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "lua", "eval" },
    fields =
    {
        Field("code", Type.STRING)
    }
}

M.ScriptReloadCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "script reload" },
    fields = {}
}

M.DatabaseReloadCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "database reload" },
    fields = {}
}

M.CameraSaveCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "camera save" },
    fields =
    {
        Field("name", Type.STRING)
    }
}
M.CameraLoadByCodeCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "camera loadbycode" },
    fields =
    {
        Field("code", Type.STRING)
    }
}

M.MapClearCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "map clear" },
    fields = {}
}
M.MapSyncCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "map sync" },
    fields =
    {
        Field("mapID", Type.U32)
    }
}
M.MapSyncAllCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "map sync all" },
    fields = {}
}

M.CharacterAddCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "character add" },
    fields =
    {
        Field("name", Type.STRING)
    }
}
M.CharacterRemoveCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "character remove", "character rem" },
    fields =
    {
        Field("name", Type.STRING)
    }
}

M.CheatLoginCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "cheat login" },
    fields =
    {
        Field("accountName", Type.STRING)
    }
}
M.CheatFlyCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "cheat fly" },
    fields =
    {
        Field("enable", Type.BOOL)
    }
}
M.CheatDamageCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "cheat damage" },
    fields =
    {
        Field("amount", Type.U32)
    }
}
M.CheatKillCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "cheat kill" },
    fields = {}
}
M.CheatResurrectCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "cheat resurrect" },
    fields = {}
}
M.CheatCastCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "cheat cast" },
    fields =
    {
        Field("spellID", Type.U32)
    }
}
M.CheatMorphCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "cheat morph" },
    fields =
    {
        Field("displayID", Type.U32)
    }
}
M.CheatDemorphCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "cheat demorph" },
    fields = {}
}
M.CheatSetRaceCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "cheat setrace" },
    fields =
    {
        Field("race", Type.STRING)
    }
}
M.CheatSetGenderCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "cheat setgender" },
    fields =
    {
        Field("gender", Type.STRING)
    }
}
M.CheatPathGenerateCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "cheat path generate" },
    fields = {}
}

M.ItemSyncCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "item sync" },
    fields =
    {
        Field("itemID", Type.U32)
    }
}
M.ItemSyncAllCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "item sync all" },
    fields = {}
}
M.ItemAddCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "item add" },
    fields =
    {
        Field("itemID", Type.U32)
    }
}
M.ItemRemoveCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "item remove", "item rem" },
    fields =
    {
        Field("itemID", Type.U32)
    }
}

M.CreatureAddCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "creature add" },
    fields =
    {
        Field("creatureTemplateID", Type.U32)
    }
}
M.CreatureRemoveCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "creature remove", "creature rem" },
    fields = {}
}
M.CreatureInfoCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "creature info" },
    fields = {}
}
M.CreatureAddScriptCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "creature add script" },
    fields =
    {
        Field("scriptName", Type.STRING)
    }
}
M.CreatureRemoveScriptCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "creature remove script" },
    fields = {}
}

M.GotoAddCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "goto add" },
    fields =
    {
        Field("location", Type.STRING),
        Field("mapID", Type.U32),
        Field("x", Type.F32),
        Field("y", Type.F32),
        Field("z", Type.F32),
        Field("orientation", Type.F32)
    }
}
M.GotoAddHereCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "goto add here" },
    fields =
    {
        Field("location", Type.STRING)
    }
}
M.GotoRemoveCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "goto remove", "goto rem" },
    fields =
    {
        Field("location", Type.STRING)
    }
}
M.GotoMapCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "goto map" },
    fields =
    {
        Field("mapID", Type.U32)
    }
}
M.GotoLocationCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "goto location", "goto loc" },
    fields =
    {
        Field("location", Type.STRING)
    }
}
M.GotoXYZCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "goto xyz" },
    fields =
    {
        Field("x", Type.F32),
        Field("y", Type.F32),
        Field("z", Type.F32)
    }
}

M.TriggerAddCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "trigger add" },
    fields =
    {
        Field("name", Type.STRING),
        Field("flags", Type.U16),
        Field("mapID", Type.U32),
        Field("posX", Type.F32),
        Field("posY", Type.F32),
        Field("posZ", Type.F32),
        Field("extX", Type.F32),
        Field("extY", Type.F32),
        Field("extZ", Type.F32),
    }
}
M.TriggerRemoveCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "trigger remove", "trigger rem" },
    fields =
    {
        Field("triggerID", Type.U32)
    }
}

M.SpellSyncCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "spell sync" },
    fields =
    {
        Field("spellID", Type.U32)
    }
}
M.SpellSyncAllCommand =
{
    archetype = Archetype.GameCommand,

    aliases = { "spell sync all" },
    fields = {}
}

return M
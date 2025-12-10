local M = OrderedTable()

local Type = require("Type")
local Archetype = require("Archetype")
local Component = require("Component")

M.CombatLogEventEnum =
{
    archetype = Archetype.Enum,
    type = Type.U16,

    fields = 
    {
        Field("DamageDealt"),
        Field("HealingDone"),
        Field("Resurrected"),
        Field("Count")
    }
}

return M
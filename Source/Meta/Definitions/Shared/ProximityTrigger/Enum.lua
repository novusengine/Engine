local M = OrderedTable()

local Type = require("Type")
local Archetype = require("Archetype")
local Component = require("Component")

M.ProximityTriggerFlagEnum =
{
    archetype = Archetype.Enum,
    type = Type.U8,

    fields = 
    {
        Field("None"),
        Field("IsServerAuthorative", 0x1),
        Field("IsServerSideOnly", 0x2)
    }
}

return M
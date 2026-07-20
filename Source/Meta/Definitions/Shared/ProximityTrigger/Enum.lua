local Type = require("Type")
local D = require("Definition")

return D.Definitions
{
    D.Enum("ProximityTriggerFlagEnum", Type.U8,
    {
        "None",
        { "IsServerAuthorative", 0x1 },
        { "IsServerSideOnly", 0x2 }
    })
}

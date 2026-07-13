local Type = require("Type")
local D = require("Definition")

return D.Definitions
{
    D.Enum("CombatLogEventEnum", Type.U16,
    {
        "DamageDealt",
        "HealingDone",
        "Resurrected",
        "Count"
    })
}

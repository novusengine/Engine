local Type = require("Type")
local D = require("Definition")

return D.Definitions
{
    D.Enum("LocaleEnum", Type.U8,
    {
        D.Field("EnUS"),
        D.Field("DeDE"),
        D.Field("FrFR"),
        D.Field("Count")
    })
}

local Type = require("Type")
local D = require("Definition")

return D.Definitions
{
    D.NetField("ObjectNetFieldEnum", Type.U16,
    {
        D.Field("ObjectGUIDLow", 0x0),
        D.Field("ObjectGUIDHigh", 0x1),
        D.Field("Scale", 0x2)
    }),

    D.NetField("UnitNetFieldEnum", Type.U16,
    {
        D.Field("LevelRaceGenderClassPacked", 0x0),
        D.Field("DisplayID", 0x1)
    }),

    D.NetField("UnitLevelRaceGenderClassPackedInfoEnum", Type.U8,
    {
        D.Field("LevelByteOffset", 0x0),
        D.Field("LevelBitOffset", 0x0),
        D.Field("RaceBitOffset", 0x0),
        D.Field("RaceByteOffset", 0x2),
        D.Field("GenderByteOffset", 0x2),
        D.Field("ClassByteOffset", 0x2),
        D.Field("GenderBitSize", 0x2),
        D.Field("GenderBitOffset", 0x7),
        D.Field("ClassBitSize", 0x7),
        D.Field("RaceBitSize", 0x7),
        D.Field("ClassBitOffset", 0x9),
        D.Field("LevelBitSize", 0x10),
    })
}

local M = OrderedTable()

local Type = require("Type")
local Archetype = require("Archetype")
local Component = require("Component")

M.ObjectNetFieldEnum =
{
    archetype = Archetype.NetField,
    type = Type.U16,

    fields = 
    {
        Field("ObjectGUIDLow", 0x0),
        Field("ObjectGUIDHigh", 0x1),
        Field("Scale", 0x2)
    }
}

M.UnitNetFieldEnum =
{
    archetype = Archetype.NetField,
    type = Type.U16,

    fields = 
    {
        Field("LevelRaceGenderClassPacked", 0x0),
        Field("DisplayID", 0x1)
    }
}

M.UnitLevelRaceGenderClassPackedInfoEnum =
{
    archetype = Archetype.NetField,
    type = Type.U8,

    fields = 
    {
        Field("LevelByteOffset", 0x0),
        Field("LevelBitOffset", 0x0),
        Field("RaceBitOffset", 0x0),
        Field("RaceByteOffset", 0x2),
        Field("GenderByteOffset", 0x2),
        Field("ClassByteOffset", 0x2),
        Field("GenderBitSize", 0x2),
        Field("GenderBitOffset", 0x7),
        Field("ClassBitSize", 0x7),
        Field("RaceBitSize", 0x7),
        Field("ClassBitOffset", 0x9),
        Field("LevelBitSize", 0x10),
    }
}

return M
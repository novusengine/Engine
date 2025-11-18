type ObjectNetFields<Enum> =
{
    Name : "ObjectNetFields";
    Type : u16;

    Fields :
    {
        ObjectGUIDLow         : Value<0x0>,
        ObjectGUIDHigh        : Value<0x1>,
        Scale                 : Value<0x2>
    };
};

type UnitNetFields<Enum> =
{
    Name : "UnitNetFields";
    Type : u16;

    Fields :
    {
        LevelRaceGenderClassPacked  : Value<0x0>,
        DisplayID                   : Value<0x1>
    };
};

type UnitLevelRaceGenderClassPackedInfo<Enum> =
{
    Name : "UnitLevelRaceGenderClassPackedInfo";
    Type : u8;

    Fields :
    {
        LevelByteOffset   : Value<0x0>,
        LevelBitOffset    : Value<0x0>,
        RaceBitOffset     : Value<0x0>,
        RaceByteOffset    : Value<0x2>,
        GenderByteOffset  : Value<0x2>,
        ClassByteOffset   : Value<0x2>,
        GenderBitSize     : Value<0x2>,
        GenderBitOffset   : Value<0x7>,
        ClassBitSize      : Value<0x7>,
        RaceBitSize       : Value<0x7>,
        ClassBitOffset    : Value<0x9>,
        LevelBitSize      : Value<0x10>
    };
};
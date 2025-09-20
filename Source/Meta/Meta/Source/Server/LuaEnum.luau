type LuaHandlerType<Enum> =
{
    Name : "LuaHandlerType";
    Type : u16;

    Fields :
    {
        Global     : Auto,
        Event      : Auto,
        Message    : Auto,
        Unit       : Auto,
        Character  : Auto,
        Spell      : Auto,
        Item       : Auto,
        Creature   : Auto,
        Count      : Auto
    };
};
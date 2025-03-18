type LuaGameEvent<Enum> =
{
    Name : "GameEvent";
    Type : u8;

    Fields :
    {
        Invalid : Auto,
        Loaded : Auto,
        Updated : Auto,
        Count : Auto
    };
}
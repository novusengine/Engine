type LuaServerEvent<Enum> =
{
    Name : "ServerEvent";
    Type : u8;

    Fields :
    {
        Invalid : Auto,
        Loaded : Auto,
        Updated : Auto,
        Count : Auto
    };
}
type LuaServerEventDataLoaded<LuaStruct> =
{
    Fields :
    {
        motd : string
    };
}
type LuaServerEventDataUpdated<LuaStruct> =
{
    Fields :
    {
        deltaTime : f32
    };
}
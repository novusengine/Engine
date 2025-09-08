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

type LuaTriggerEvent<Enum> =
{
    Name : "TriggerEvent";
    Type : u8;

    Fields :
    {
        Invalid : Auto,
        OnEnter : Auto,
        OnExit : Auto,
        OnStay : Auto,
        Count : Auto
    };
}
type LuaTriggerEventDataOnEnter<LuaStruct> =
{
    Name : "TriggerEventDataOnEnter";

    Fields :
    {
        triggerID : u32,
        playerID : u32
    };
}
type LuaTriggerEventDataOnExit<LuaStruct> =
{
    Name : "TriggerEventDataOnExit";

    Fields :
    {
        triggerID : u32,
        playerID : u32
    };
}
type LuaTriggerEventDataOnStay<LuaStruct> =
{
    Name : "TriggerEventDataOnStay";

    Fields :
    {
        triggerID : u32,
        playerID : u32
    };
}
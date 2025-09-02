type LuaGameEvent<Enum> =
{
    Name : "GameEvent";
    Type : u8;

    Fields :
    {
        Invalid : Auto,
        Loaded : Auto,
        Updated : Auto,
        MapLoading : Auto,
        ChatMessageReceived : Auto,
        Count : Auto
    };
}
type LuaPlayerEvent<Enum> =
{
    Name : "PlayerEvent";
    Type : u8;

    Fields :
    {
        Invalid : Auto,
        Created : Auto,
        Destroyed : Auto,
        ContainerCreate : Auto,
        ContainerAddToSlot : Auto,
        ContainerRemoveFromSlot : Auto,
        ContainerSwapSlots : Auto,
        Count : Auto
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
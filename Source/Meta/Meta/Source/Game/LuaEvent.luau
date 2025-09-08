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
type LuaGameEventDataLoaded<LuaStruct> =
{
    Name : "GameEventDataLoaded";

    Fields :
    {
        motd : string
    };
}
type LuaGameEventDataUpdated<LuaStruct> =
{
    Name : "GameEventDataUpdated";

    Fields :
    {
        deltaTime : f32
    };
}
type LuaGameEventDataMapLoading<LuaStruct> =
{
    Name : "GameEventDataMapLoading";

    Fields :
    {
        mapInternalName : string
    };
}
type LuaGameEventDataChatMessageReceived<LuaStruct> =
{
    Name : "GameEventDataChatMessageReceived";

    Fields :
    {
        sender : string,
        channel : string,
        message : string
    };
}

type LuaUnitEvent<Enum> =
{
    Name : "UnitEvent";
    Type : u8;

    Fields :
    {
        Invalid : Auto,
        Add : Auto,
        Remove : Auto,
        Count : Auto
    };
}
type LuaUnitEventDataAdd<LuaStruct> =
{
    Name : "UnitEventDataAdd";

    Fields :
    {
        unitID : u32
    };
}
type LuaUnitEventDataRemove<LuaStruct> =
{
    Name : "UnitEventDataRemove";

    Fields :
    {
        unitID : u32
    };
}

type LuaContainerEvent<Enum> =
{
    Name : "ContainerEvent";
    Type : u8;

    Fields :
    {
        Invalid : Auto,
        
        Add : Auto,
        AddToSlot : Auto,
        RemoveFromSlot : Auto,
        SwapSlots : Auto,
        Count : Auto
    };
}
type LuaContainerEventDataAdd<LuaStruct> =
{
    Name : "ContainerEventDataAdd";

    Fields :
    {
        index : u32,
        numSlots : u32,
        itemID : u32
    };
}
type LuaContainerEventDataAddToSlot<LuaStruct> =
{
    Name : "ContainerEventDataAddToSlot";

    Fields :
    {
        containerIndex : u32,
        slotIndex : u32,
        itemID : u32,
        count : u32
    };
}
type LuaContainerEventDataRemoveFromSlot<LuaStruct> =
{
    Name : "ContainerEventDataRemoveFromSlot";

    Fields :
    {
        containerIndex : u32,
        slotIndex : u32
    };
}
type LuaContainerEventDataSwapSlots<LuaStruct> =
{
    Name : "ContainerEventDataSwapSlots";

    Fields :
    {
        srcContainerIndex : u32,
        destContainerIndex : u32,
        srcSlotIndex : u32,
        destSlotIndex : u32
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
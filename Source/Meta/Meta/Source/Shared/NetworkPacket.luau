type Invalid<Packet> =
{
    Name : "Invalid";

    Fields :
    {
    };
};

type Client_Connect<Packet> =
{
    Name : "Connect";

    Fields :
    {
        accountName : string,
    };
};

type Server_AuthChallenge<Packet> =
{
    Name : "ServerAuthChallenge";

    Fields :
    {
        challenge : array<u8, 36>,
    };
};

type Client_AuthChallenge<Packet> =
{
    Name : "ClientAuthChallenge";

    Fields :
    {
        challenge : array<u8, 32>,
    };
};

type Server_AuthProof<Packet> =
{
    Name : "ServerAuthProof";

    Fields :
    {
        proof : array<u8, 64>,
    };
};

type Client_AuthProof<Packet> =
{
    Name : "ClientAuthProof";

    Fields :
    {
        proof : array<u8, 32>,
    };
};

type Server_ConnectResult<Packet> =
{
    Name : "ConnectResult";

    Fields :
    {
        result : u8,
    };
};

type Client_Ping<Packet> =
{
    Name : "Ping";

    Fields :
    {
        ping : u16
    };
};

type Server_Pong<Packet> =
{
    Name : "Pong";

    Fields :
    {
    };
};

type Server_UpdateStats<Packet> =
{
    Name : "ServerUpdateStats";

    Fields :
    {
        serverTickTime : u8
    };
};

type Client_SendCheatCommand<Packet> =
{
    Name : "SendCheatCommand";

    Fields :
    {
        command : u8
    };
};

type Server_CheatCommandResult<Packet> =
{
    Name : "CheatCommandResult";

    Fields :
    {
        command : u8,
        result : u8,
        response : string
    };
};

type Server_UnitAdd<Packet> =
{
    Name : "UnitAdd";

    Fields :
    {
        guid : objectguid,
        name : string,
        unitClass : u8,
        position : vec3,
        scale : vec3,
        pitchYaw : vec2
    };
};

type Server_UnitRemove<Packet> =
{
    Name : "UnitRemove";

    Fields :
    {
        guid : objectguid
    };
};

type Server_UnitPowerUpdate<Packet> =
{
    Name : "UnitPowerUpdate";

    Fields :
    {
        guid : objectguid,
        kind : u8,
        base : f64,
        current : f64,
        max : f64
    };
};

type Server_UnitResistanceUpdate<Packet> =
{
    Name : "UnitResistanceUpdate";

    Fields :
    {
        kind : u8,
        base : f64,
        current : f64,
        max : f64
    };
};

type Server_UnitStatUpdate<Packet> =
{
    Name : "UnitStatUpdate";

    Fields :
    {
        kind : u8,
        base : f64,
        current : f64
    };
};

type Client_UnitTargetUpdate<Packet> =
{
    Name : "ClientUnitTargetUpdate";

    Fields :
    {
        targetGUID : objectguid
    };
};

type Server_UnitTargetUpdate<Packet> =
{
    Name : "ServerUnitTargetUpdate";

    Fields :
    {
        guid : objectguid,
        targetGUID : objectguid,
    };
};

type Server_UnitCastSpell<Packet> =
{
    Name : "UnitCastSpell";

    Fields :
    {
        spellID : u32,
        guid : objectguid,
        castTime : f32,
        timeToCast : f32
    };
};

type Server_UnitSetMover<Packet> =
{
    Name : "UnitSetMover";

    Fields :
    {
        guid : objectguid
    };
};

type Client_UnitMove<Packet> =
{
    Name : "ClientUnitMove";

    Fields :
    {
        movementFlags : u32,
        position : vec3,
        pitchYaw : vec2,
        verticalVelocity : f32
    };
};

type Server_UnitMove<Packet> =
{
    Name : "ServerUnitMove";

    Fields :
    {
        guid : objectguid,
        movementFlags : u32,
        position : vec3,
        pitchYaw : vec2,
        verticalVelocity : f32
    };
};

type Shared_UnitMoveStop<Packet> =
{
    Name : "UnitMoveStop";

    Fields :
    {
        guid : objectguid
    };
};

type Server_ItemAdd<Packet> =
{
    Name : "ItemAdd";

    Fields :
    {
        guid : objectguid,
        itemID : u32,
        count : u16,
        durability : u16
    };
};

type Server_ContainerAdd<Packet> =
{
    Name : "ContainerAdd";

    Fields :
    {
        index : u16,
        itemID : u32,
        guid : objectguid,
        numSlots : u16,
        numFreeSlots : u16
    };
};

type Server_ContainerAddToSlot<Packet> =
{
    Name : "ContainerAddToSlot";

    Fields :
    {
        index : u16,
        slot : u16,
        guid : objectguid
    };
};

type Server_ContainerRemoveFromSlot<Packet> =
{
    Name : "ContainerRemoveFromSlot";

    Fields :
    {
        index : u16,
        slot : u16
    };
};

type Client_ContainerSwapSlots<Packet> =
{
    Name : "ClientContainerSwapSlots";

    Fields :
    {
        srcContainer : u16,
        dstContainer : u16,
        srcSlot : u16,
        dstSlot : u16
    };
};

type Server_ContainerSwapSlots<Packet> =
{
    Name : "ServerContainerSwapSlots";

    Fields :
    {
        srcContainer : u16,
        dstContainer : u16,
        srcSlot : u16,
        dstSlot : u16
    };
};

type Client_SpellCast<Packet> =
{
    Name : "ClientSpellCast";

    Fields :
    {
        spellID : u32
    };
};

type Server_SpellCastResult<Packet> =
{
    Name : "ServerSpellCastResult";

    Fields :
    {
        result : u8
    };
};

type Server_SendCombatEvent<Packet> =
{
    Name : "SendCombatEvent";

    Fields :
    {
        eventID : u16
    };
};

type Client_SendChatMessage<Packet> =
{
    Name : "ClientSendChatMessage";

    Fields :
    {
        message : string
    };
};

type Server_SendChatMessage<Packet> =
{
    Name : "ServerSendChatMessage";

    Fields :
    {
        guid : objectguid,
        message : string
    };
};

type Server_UnitEquippedItemUpdate<Packet> =
{
    Name : "ServerUnitEquippedItemUpdate";

    Fields :
    {
        guid : objectguid,
        slot : u8,
        itemID : u32
    };
}

type Server_UnitVisualItemUpdate<Packet> =
{
    Name : "ServerUnitVisualItemUpdate";

    Fields :
    {
        guid : objectguid,
        slot : u8,
        itemID : u32
    };
}

type Server_WorldTransfer<Packet> =
{
    Name : "ServerWorldTransfer";

    Fields :
    {
    };
}

type Server_LoadMap<Packet> =
{
    Name : "ServerLoadMap";

    Fields :
    {
        mapID : u32
    };
}

type Server_UnitTeleport<Packet> =
{
    Name : "ServerUnitTeleport";

    Fields :
    {
        guid : objectguid,
        position : vec3,
        orientation : f32
    };
}

type Client_TriggerEnter<Packet> =
{
    Name : "ClientTriggerEnter";

    Fields :
    {
        triggerID : u32
    };
}

type Server_TriggerAdd<Packet> =
{
    Name : "ServerTriggerAdd";

    Fields :
    {
        triggerID : u32,
        name : string,
        flags : u8,
        mapID : u16,
        position : vec3,
        extents : vec3
    };
}

type Server_TriggerRemove<Packet> =
{
    Name : "ServerTriggerRemove";

    Fields :
    {
        triggerID : u32
    };
}

type Server_UnitAddAura<Packet> =
{
    Name : "ServerUnitAddAura";

    Fields :
    {
        guid : objectguid,
        auraInstanceID : u32,
        spellID : u32,
        duration : f32,
        stacks : u16
    };
}
type Server_UnitUpdateAura<Packet> =
{
    Name : "ServerUnitUpdateAura";

    Fields :
    {
        guid : objectguid,
        auraInstanceID : u32,
        duration : f32,
        stacks : u16
    };
}
type Server_UnitRemoveAura<Packet> =
{
    Name : "ServerUnitRemoveAura";

    Fields :
    {
        guid : objectguid,
        auraInstanceID : u32
    };
}
type Server_CharacterList<Packet> =
{
    Name : "ServerCharacterList";

    Fields :
    {
    };
};
type Client_CharacterSelect<Packet> =
{
    Name : "ClientCharacterSelect";

    Fields :
    {
        characterIndex : u8
    };
};
type Client_CharacterLogout<Packet> =
{
    Name : "ClientCharacterLogout";

    Fields :
    {
    };
};
type Server_CharacterLogout<Packet> =
{
    Name : "ServerCharacterLogout";

    Fields :
    {
    };
};
type Server_ObjectNetFieldUpdate<Packet> =
{
    Name : "ServerObjectNetFieldUpdate";

    Fields :
    {
    };
};
type Server_UnitNetFieldUpdate<Packet> =
{
    Name : "ServerUnitNetFieldUpdate";

    Fields :
    {
    };
};
type Client_PathGenerate<Packet> =
{
    Name : "ClientPathGenerate";

    Fields :
    {
        start : vec3,
        end : vec3
    };
};
type Server_PathVisualization<Packet> =
{
    Name : "ServerPathVisualization";

    Fields :
    {
    };
};
#pragma once
#include "Network/Define.h"

namespace Network
{
    enum class GameOpcode : OpcodeType
    {
        Invalid,

        Client_Connect,
        Server_Connected,

        Shared_Ping,
        Server_UpdateStats,

        Client_SendCheatCommand,
        Server_SendCheatCommandResult,

        Server_SetMover,
        Server_EntityCreate,
        Server_EntityDestroy,
        Server_EntityDisplayInfoUpdate,
        Shared_EntityMove,
        Shared_EntityMoveStop,
        Server_EntityResourcesUpdate,
        Shared_EntityTargetUpdate,
        Server_EntityCastSpell,

        Server_ItemCreate,
        Server_ContainerCreate,

        Client_ContainerSwapSlots,
        Server_ContainerAddToSlot,
        Server_ContainerRemoveFromSlot,
        Server_ContainerSwapSlots,

        Client_LocalRequestSpellCast,
        Server_SendSpellCastResult,

        Server_SendCombatEvent,

        Server_TriggerCreate,
        Server_TriggerDestroy,
        Client_TriggerEnter,
        Server_TriggerEnterAck,

        Count
    };
}
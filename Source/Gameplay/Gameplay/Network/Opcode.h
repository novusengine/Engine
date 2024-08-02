#pragma once
#include "Network/Define.h"

namespace Network
{
    enum class GameOpcode : OpcodeType
    {
        Invalid,

        Client_Connect,
        Server_Connected,

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

        Client_LocalRequestSpellCast,
        Server_SendSpellCastResult,

        Server_SendCombatEvent,

        Count
    };
}
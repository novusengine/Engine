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

        Server_PlayerCreate,
        Server_EntityCreate,
        Server_EntityDestroy,
        Server_EntityUpdate,
        Shared_EntityMove,
        Shared_EntityMoveStop,
        Server_EntityResourcesUpdate,
        Server_EntityDisplayInfoUpdate,
        Shared_EntityTargetUpdate,
        Server_EntityCastSpell,

        Client_LocalRequestSpellCast,

        Server_SendSpellCastResult,
        Server_SendCombatEvent,

        Count
    };
}
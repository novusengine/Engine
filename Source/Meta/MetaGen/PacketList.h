#pragma once

#include <Base/Types.h>

#include <array>
#include <string>
#include <utility>

namespace MetaGen
{
    enum class PacketListEnum : u16
    {
        Invalid = 0u,
        ClientConnectPacket = 1,
        ServerConnectResultPacket = 2,
        ServerAuthChallengePacket = 3,
        ClientAuthChallengePacket = 4,
        ServerAuthProofPacket = 5,
        ClientAuthProofPacket = 6,
        ClientPingPacket = 7,
        ServerPongPacket = 8,
        ServerUpdateStatsPacket = 9,
        ServerCharacterListPacket = 10,
        ClientCharacterSelectPacket = 11,
        ClientCharacterLogoutPacket = 12,
        ServerCharacterLogoutPacket = 13,
        ServerWorldTransferPacket = 14,
        ServerLoadMapPacket = 15,
        ServerUnitAddPacket = 16,
        ServerUnitRemovePacket = 17,
        ServerObjectNetFieldUpdatePacket = 18,
        ServerUnitNetFieldUpdatePacket = 19,
        ServerUnitStatUpdatePacket = 20,
        ServerUnitResistanceUpdatePacket = 21,
        ServerUnitPowerUpdatePacket = 22,
        ServerUnitEquippedItemUpdatePacket = 23,
        ServerUnitVisualItemUpdatePacket = 24,
        ServerContainerAddPacket = 25,
        ServerContainerAddToSlotPacket = 26,
        ServerContainerRemoveFromSlotPacket = 27,
        SharedContainerSwapSlotsPacket = 28,
        ServerItemAddPacket = 29,
        ServerSendCombatEventPacket = 30,
        ClientUnitTargetUpdatePacket = 31,
        ServerUnitTargetUpdatePacket = 32,
        ClientSpellCastPacket = 33,
        ServerSpellCastResultPacket = 34,
        ServerUnitCastSpellPacket = 35,
        ServerUnitAddAuraPacket = 36,
        ServerUnitUpdateAuraPacket = 37,
        ServerUnitRemoveAuraPacket = 38,
        ServerUnitSetMoverPacket = 39,
        ServerUnitMovePacket = 40,
        SharedUnitMoveStopPacket = 41,
        ServerUnitTeleportPacket = 42,
        ClientUnitMovePacket = 43,
        ClientSendChatMessagePacket = 44,
        ServerSendChatMessagePacket = 45,
        ServerTriggerAddPacket = 46,
        ServerTriggerRemovePacket = 47,
        ClientTriggerEnterPacket = 48,
        ClientPathGeneratePacket = 49,
        ServerPathVisualizationPacket = 50,
        ClientSendCheatCommandPacket = 51,
        ServerCheatCommandResultPacket = 52,
        Count = 53
    };
    struct PacketListEnumMeta
    {
        using Type = u16;

        static constexpr u16 ENUM_ID = 30;
        static constexpr std::string_view ENUM_NAME = "PacketListEnum";
        static constexpr std::array<std::pair<std::string_view, u16>, 54> ENUM_FIELD_LIST = { std::pair("Invalid", 0u), std::pair("ClientConnectPacket", 1u), std::pair("ServerConnectResultPacket", 2u), std::pair("ServerAuthChallengePacket", 3u), std::pair("ClientAuthChallengePacket", 4u), std::pair("ServerAuthProofPacket", 5u), std::pair("ClientAuthProofPacket", 6u), std::pair("ClientPingPacket", 7u), std::pair("ServerPongPacket", 8u), std::pair("ServerUpdateStatsPacket", 9u), std::pair("ServerCharacterListPacket", 10u), std::pair("ClientCharacterSelectPacket", 11u), std::pair("ClientCharacterLogoutPacket", 12u), std::pair("ServerCharacterLogoutPacket", 13u), std::pair("ServerWorldTransferPacket", 14u), std::pair("ServerLoadMapPacket", 15u), std::pair("ServerUnitAddPacket", 16u), std::pair("ServerUnitRemovePacket", 17u), std::pair("ServerObjectNetFieldUpdatePacket", 18u), std::pair("ServerUnitNetFieldUpdatePacket", 19u), std::pair("ServerUnitStatUpdatePacket", 20u), std::pair("ServerUnitResistanceUpdatePacket", 21u), std::pair("ServerUnitPowerUpdatePacket", 22u), std::pair("ServerUnitEquippedItemUpdatePacket", 23u), std::pair("ServerUnitVisualItemUpdatePacket", 24u), std::pair("ServerContainerAddPacket", 25u), std::pair("ServerContainerAddToSlotPacket", 26u), std::pair("ServerContainerRemoveFromSlotPacket", 27u), std::pair("SharedContainerSwapSlotsPacket", 28u), std::pair("ServerItemAddPacket", 29u), std::pair("ServerSendCombatEventPacket", 30u), std::pair("ClientUnitTargetUpdatePacket", 31u), std::pair("ServerUnitTargetUpdatePacket", 32u), std::pair("ClientSpellCastPacket", 33u), std::pair("ServerSpellCastResultPacket", 34u), std::pair("ServerUnitCastSpellPacket", 35u), std::pair("ServerUnitAddAuraPacket", 36u), std::pair("ServerUnitUpdateAuraPacket", 37u), std::pair("ServerUnitRemoveAuraPacket", 38u), std::pair("ServerUnitSetMoverPacket", 39u), std::pair("ServerUnitMovePacket", 40u), std::pair("SharedUnitMoveStopPacket", 41u), std::pair("ServerUnitTeleportPacket", 42u), std::pair("ClientUnitMovePacket", 43u), std::pair("ClientSendChatMessagePacket", 44u), std::pair("ServerSendChatMessagePacket", 45u), std::pair("ServerTriggerAddPacket", 46u), std::pair("ServerTriggerRemovePacket", 47u), std::pair("ClientTriggerEnterPacket", 48u), std::pair("ClientPathGeneratePacket", 49u), std::pair("ServerPathVisualizationPacket", 50u), std::pair("ClientSendCheatCommandPacket", 51u), std::pair("ServerCheatCommandResultPacket", 52u), std::pair("Count", 53u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(PacketListEnum);
}
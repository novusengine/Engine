local Type = require("Type")
local D = require("Definition")

return D.Definitions
{
    D.Packet("ClientConnectPacket",
    {
        D.Field("accountName", Type.STRING)
    }),

    D.Packet("ServerConnectResultPacket",
    {
        D.Field("result", Type.U8)
    }),

    D.Packet("ServerAuthChallengePacket",
    {
        D.Field("challenge", Type.ARRAY, { type = Type.U8, count = 36 })
    }),

    D.Packet("ClientAuthChallengePacket",
    {
        D.Field("challenge", Type.ARRAY, { type = Type.U8, count = 32 })
    }),

    D.Packet("ServerAuthProofPacket",
    {
        D.Field("proof", Type.ARRAY, { type = Type.U8, count = 64 })
    }),

    D.Packet("ClientAuthProofPacket",
    {
        D.Field("proof", Type.ARRAY, { type = Type.U8, count = 32 })
    }),

    D.Packet("ClientPingPacket",
    {
        D.Field("ping", Type.U16)
    }),

    D.Packet("ServerPongPacket",
    {}),

    D.Packet("ServerUpdateStatsPacket",
    {
        D.Field("serverTickTime", Type.U8)
    }),

    D.Packet("ServerCharacterListPacket",
    {}),

    D.Packet("ClientCharacterSelectPacket",
    {
        D.Field("characterIndex", Type.U8)
    }),

    D.Packet("ClientCharacterLogoutPacket",
    {}),

    D.Packet("ServerCharacterLogoutPacket",
    {}),

    D.Packet("ServerWorldTransferPacket",
    {}),

    D.Packet("ServerLoadMapPacket",
    {
        D.Field("mapID", Type.U32)
    }),

    D.Packet("ServerUnitAddPacket",
    {
        D.Field("guid", Type.OBJECTGUID),
        D.Field("name", Type.STRING),
        D.Field("unitClass", Type.U8),
        D.Field("position", Type.VEC3),
        D.Field("scale", Type.VEC3),
        D.Field("pitchYaw", Type.VEC2)
    }),

    D.Packet("ServerUnitRemovePacket",
    {
        D.Field("guid", Type.OBJECTGUID)
    }),

    D.Packet("ServerObjectNetFieldUpdatePacket",
    {}),

    D.Packet("ServerUnitNetFieldUpdatePacket",
    {}),

    D.Packet("ServerUnitStatUpdatePacket",
    {
        D.Field("kind", Type.U8),
        D.Field("base", Type.F64),
        D.Field("current", Type.F64)
    }),

    D.Packet("ServerUnitResistanceUpdatePacket",
    {
        D.Field("kind", Type.U8),
        D.Field("base", Type.F64),
        D.Field("current", Type.F64),
        D.Field("max", Type.F64)
    }),

    D.Packet("ServerUnitPowerUpdatePacket",
    {
        D.Field("guid", Type.OBJECTGUID),
        D.Field("kind", Type.U8),
        D.Field("base", Type.F64),
        D.Field("current", Type.F64),
        D.Field("max", Type.F64)
    }),

    D.Packet("ServerUnitEquippedItemUpdatePacket",
    {
        D.Field("guid", Type.OBJECTGUID),
        D.Field("slot", Type.U8),
        D.Field("itemID", Type.U32)
    }),

    D.Packet("ServerUnitVisualItemUpdatePacket",
    {
        D.Field("guid", Type.OBJECTGUID),
        D.Field("slot", Type.U8),
        D.Field("itemID", Type.U32)
    }),

    D.Packet("ServerContainerAddPacket",
    {
        D.Field("guid", Type.OBJECTGUID),
        D.Field("index", Type.U16),
        D.Field("itemID", Type.U32),
        D.Field("numSlots", Type.U16),
        D.Field("numFreeSlots", Type.U16)
    }),

    D.Packet("ServerContainerAddToSlotPacket",
    {
        D.Field("guid", Type.OBJECTGUID),
        D.Field("index", Type.U16),
        D.Field("slot", Type.U16)
    }),

    D.Packet("ServerContainerRemoveFromSlotPacket",
    {
        D.Field("index", Type.U16),
        D.Field("slot", Type.U16)
    }),

    D.Packet("SharedContainerSwapSlotsPacket",
    {
        D.Field("srcContainer", Type.U16),
        D.Field("dstContainer", Type.U16),
        D.Field("srcSlot", Type.U16),
        D.Field("dstSlot", Type.U16)
    }),

    D.Packet("ServerItemAddPacket",
    {
        D.Field("guid", Type.OBJECTGUID),
        D.Field("itemID", Type.U32),
        D.Field("count", Type.U16),
        D.Field("durability", Type.U16)
    }),

    D.Packet("ServerSendCombatEventPacket",
    {
        D.Field("eventID", Type.U16)
    }),

    D.Packet("ClientUnitTargetUpdatePacket",
    {
        D.Field("targetGUID", Type.OBJECTGUID)
    }),

    D.Packet("ServerUnitTargetUpdatePacket",
    {
        D.Field("guid", Type.OBJECTGUID),
        D.Field("targetGUID", Type.OBJECTGUID)
    }),

    D.Packet("ClientSpellCastPacket",
    {
        D.Field("spellID", Type.U32),
        D.Field("targetGUID", Type.OBJECTGUID),
        D.Field("targetPosition", Type.VEC3)
    }),

    D.Packet("ServerSpellCastResultPacket",
    {
        D.Field("result", Type.U8)
    }),

    D.Packet("ServerUnitCastSpellPacket",
    {
        D.Field("guid", Type.OBJECTGUID),
        D.Field("spellID", Type.U32),
        D.Field("castTime", Type.F32),
        D.Field("timeToCast", Type.F32)
    }),

    D.Packet("ServerUnitAddAuraPacket",
    {
        D.Field("guid", Type.OBJECTGUID),
        D.Field("auraInstanceID", Type.U32),
        D.Field("spellID", Type.U32),
        D.Field("duration", Type.F32),
        D.Field("stacks", Type.U16)
    }),

    D.Packet("ServerUnitUpdateAuraPacket",
    {
        D.Field("guid", Type.OBJECTGUID),
        D.Field("auraInstanceID", Type.U32),
        D.Field("duration", Type.F32),
        D.Field("stacks", Type.U16)
    }),

    D.Packet("ServerUnitRemoveAuraPacket",
    {
        D.Field("guid", Type.OBJECTGUID),
        D.Field("auraInstanceID", Type.U32)
    }),

    D.Packet("ServerUnitSetMoverPacket",
    {
        D.Field("guid", Type.OBJECTGUID)
    }),

    D.Packet("ServerUnitMovePacket",
    {
        D.Field("guid", Type.OBJECTGUID),
        D.Field("movementFlags", Type.U32),
        D.Field("position", Type.VEC3),
        D.Field("pitchYaw", Type.VEC2),
        D.Field("verticalVelocity", Type.F32)
    }),

    D.Packet("SharedUnitMoveStopPacket",
    {
        D.Field("guid", Type.OBJECTGUID)
    }),

    D.Packet("ServerUnitTeleportPacket",
    {
        D.Field("guid", Type.OBJECTGUID),
        D.Field("position", Type.VEC3),
        D.Field("orientation", Type.F32)
    }),

    D.Packet("ClientUnitMovePacket",
    {
        D.Field("movementFlags", Type.U32),
        D.Field("position", Type.VEC3),
        D.Field("pitchYaw", Type.VEC2),
        D.Field("verticalVelocity", Type.F32)
    }),

    D.Packet("ClientSendChatMessagePacket",
    {
        D.Field("message", Type.STRING)
    }),

    D.Packet("ServerSendChatMessagePacket",
    {
        D.Field("guid", Type.OBJECTGUID),
        D.Field("message", Type.STRING),
    }),

    D.Packet("ServerTriggerAddPacket",
    {
        D.Field("triggerID", Type.U32),
        D.Field("name", Type.STRING),
        D.Field("flags", Type.U8),
        D.Field("mapID", Type.U32),
        D.Field("position", Type.VEC3),
        D.Field("extents", Type.VEC3)
    }),

    D.Packet("ServerTriggerRemovePacket",
    {
        D.Field("triggerID", Type.U32)
    }),

    D.Packet("ClientTriggerEnterPacket",
    {
        D.Field("triggerID", Type.U32)
    }),

    D.Packet("ClientPathGeneratePacket",
    {
        D.Field("start", Type.VEC3),
        D.Field("end", Type.VEC3)
    }),

    D.Packet("ServerPathVisualizationPacket",
    {}),

    D.Packet("ClientSendCheatCommandPacket",
    {
        D.Field("command", Type.U8)
    }),

    D.Packet("ServerCheatCommandResultPacket",
    {
        D.Field("command", Type.U8),
        D.Field("result", Type.U8),
        D.Field("response", Type.STRING)
    }),

    D.Packet("ServerUnitFactionUpdatePacket",
    {
        D.Field("guid", Type.OBJECTGUID),
        D.Field("factionID", Type.U16),
        D.Field("playerReactionBounds", Type.U8)
    }),

    D.Packet("ServerReputationUpdatePacket",
    {
        D.Field("factionID", Type.U16),
        D.Field("value", Type.I32),
        D.Field("flags", Type.U16),
        D.Field("isPresent", Type.U8)
    }),

    D.Packet("ServerFactionPerceptionOverrideUpdatePacket",
    {
        D.Field("factionID", Type.U16),
        D.Field("activeFields", Type.U8),
        D.Field("effectiveStandingValue", Type.I32),
        D.Field("effectiveReaction", Type.U8)
    }),

    D.Packet("ClientAutoAttackStatePacket",
    {
        D.Field("enabled", Type.U8)
    }),

    D.Packet("ServerUnitAutoAttackStatePacket",
    {
        D.Field("guid", Type.OBJECTGUID),
        D.Field("enabled", Type.U8)
    }),

    D.Packet("ServerUnitAttackPacket",
    {
        D.Field("guid", Type.OBJECTGUID),
        D.Field("weaponSlot", Type.U8)
    }),

    D.Packet("ServerSpellEditorSnapshotBeginPacket",
    {}),

    D.Packet("ServerSpellEditorSnapshotChunkPacket",
    {}),

    D.Packet("ServerSpellEditorSnapshotEndPacket",
    {}),

    D.Packet("ServerSpellEditorMutationResultPacket",
    {
        D.Field("requestID", Type.U32),
        D.Field("artifact", Type.U8),
        D.Field("artifactID", Type.U32),
        D.Field("mutationType", Type.U8),
        D.Field("succeeded", Type.U8),
        D.Field("response", Type.STRING)
    })
}

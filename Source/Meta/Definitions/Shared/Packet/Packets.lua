local M = OrderedTable()

local Type = require("Type")
local Archetype = require("Archetype")
local Component = require("Component")

M.ClientConnectPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("accountName", Type.STRING)
    }
}

M.ServerConnectResultPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("result", Type.U8)
    }
}

M.ServerAuthChallengePacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("challenge", Type.ARRAY, { type = Type.U8, count = 36 })
    }
}

M.ClientAuthChallengePacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("challenge", Type.ARRAY, { type = Type.U8, count = 32 })
    }
}

M.ServerAuthProofPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("proof", Type.ARRAY, { type = Type.U8, count = 64 })
    }
}

M.ClientAuthProofPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("proof", Type.ARRAY, { type = Type.U8, count = 32 })
    }
}

M.ClientPingPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("ping", Type.U16)
    }
}

M.ServerPongPacket =
{
    archetype = Archetype.Packet,

    fields = {}
}

M.ServerUpdateStatsPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("serverTickTime", Type.U8)
    }
}

M.ServerCharacterListPacket =
{
    archetype = Archetype.Packet,

    fields = {}
}

M.ClientCharacterSelectPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("characterIndex", Type.U8)
    }
}

M.ClientCharacterLogoutPacket =
{
    archetype = Archetype.Packet,

    fields = {}
}

M.ServerCharacterLogoutPacket =
{
    archetype = Archetype.Packet,

    fields = {}
}

M.ServerWorldTransferPacket =
{
    archetype = Archetype.Packet,

    fields = {}
}

M.ServerLoadMapPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("mapID", Type.U32)
    }
}

M.ServerUnitAddPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("guid", Type.OBJECTGUID),
        Field("name", Type.STRING),
        Field("unitClass", Type.U8),
        Field("position", Type.VEC3),
        Field("scale", Type.VEC3),
        Field("pitchYaw", Type.VEC2)
    }
}

M.ServerUnitRemovePacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("guid", Type.OBJECTGUID)
    }
}

M.ServerObjectNetFieldUpdatePacket =
{
    archetype = Archetype.Packet,

    fields = {}
}

M.ServerUnitNetFieldUpdatePacket =
{
    archetype = Archetype.Packet,

    fields = {}
}

M.ServerUnitStatUpdatePacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("kind", Type.U8),
        Field("base", Type.F64),
        Field("current", Type.F64)
    }
}

M.ServerUnitResistanceUpdatePacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("kind", Type.U8),
        Field("base", Type.F64),
        Field("current", Type.F64),
        Field("max", Type.F64)
    }
}

M.ServerUnitPowerUpdatePacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("guid", Type.OBJECTGUID),
        Field("kind", Type.U8),
        Field("base", Type.F64),
        Field("current", Type.F64),
        Field("max", Type.F64)
    }
}

M.ServerUnitEquippedItemUpdatePacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("guid", Type.OBJECTGUID),
        Field("slot", Type.U8),
        Field("itemID", Type.U32)
    }
}

M.ServerUnitVisualItemUpdatePacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("guid", Type.OBJECTGUID),
        Field("slot", Type.U8),
        Field("itemID", Type.U32)
    }
}

M.ServerContainerAddPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("guid", Type.OBJECTGUID),
        Field("index", Type.U16),
        Field("itemID", Type.U32),
        Field("numSlots", Type.U16),
        Field("numFreeSlots", Type.U16)
    }
}

M.ServerContainerAddToSlotPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("guid", Type.OBJECTGUID),
        Field("index", Type.U16),
        Field("slot", Type.U16)
    }
}

M.ServerContainerRemoveFromSlotPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("index", Type.U16),
        Field("slot", Type.U16)
    }
}

M.SharedContainerSwapSlotsPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("srcContainer", Type.U16),
        Field("dstContainer", Type.U16),
        Field("srcSlot", Type.U16),
        Field("dstSlot", Type.U16)
    }
}

M.ServerItemAddPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("guid", Type.OBJECTGUID),
        Field("itemID", Type.U32),
        Field("count", Type.U16),
        Field("durability", Type.U16)
    }
}

M.ServerSendCombatEventPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("eventID", Type.U16)
    }
}

M.ClientUnitTargetUpdatePacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("targetGUID", Type.OBJECTGUID)
    }
}

M.ServerUnitTargetUpdatePacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("guid", Type.OBJECTGUID),
        Field("targetGUID", Type.OBJECTGUID)
    }
}

M.ClientSpellCastPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("spellID", Type.U32)
    }
}

M.ServerSpellCastResultPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("result", Type.U8)
    }
}

M.ServerUnitCastSpellPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("guid", Type.OBJECTGUID),
        Field("spellID", Type.U32),
        Field("castTime", Type.F32),
        Field("timeToCast", Type.F32)
    }
}

M.ServerUnitAddAuraPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("guid", Type.OBJECTGUID),
        Field("auraInstanceID", Type.U32),
        Field("spellID", Type.U32),
        Field("duration", Type.F32),
        Field("stacks", Type.U16)
    }
}

M.ServerUnitUpdateAuraPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("guid", Type.OBJECTGUID),
        Field("auraInstanceID", Type.U32),
        Field("duration", Type.F32),
        Field("stacks", Type.U16)
    }
}

M.ServerUnitRemoveAuraPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("guid", Type.OBJECTGUID),
        Field("auraInstanceID", Type.U32)
    }
}

M.ServerUnitSetMoverPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("guid", Type.OBJECTGUID)
    }
}

M.ServerUnitMovePacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("guid", Type.OBJECTGUID),
        Field("movementFlags", Type.U32),
        Field("position", Type.VEC3),
        Field("pitchYaw", Type.VEC2),
        Field("verticalVelocity", Type.F32)
    }
}

M.SharedUnitMoveStopPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("guid", Type.OBJECTGUID)
    }
}

M.ServerUnitTeleportPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("guid", Type.OBJECTGUID),
        Field("position", Type.VEC3),
        Field("orientation", Type.F32)
    }
}

M.ClientUnitMovePacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("movementFlags", Type.U32),
        Field("position", Type.VEC3),
        Field("pitchYaw", Type.VEC2),
        Field("verticalVelocity", Type.F32)
    }
}

M.ClientSendChatMessagePacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("message", Type.STRING)
    }
}

M.ServerSendChatMessagePacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("guid", Type.OBJECTGUID),
        Field("message", Type.STRING),
    }
}

M.ServerTriggerAddPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("triggerID", Type.U32),
        Field("name", Type.STRING),
        Field("flags", Type.U8),
        Field("mapID", Type.U16),
        Field("position", Type.VEC3),
        Field("extents", Type.VEC3)
    }
}

M.ServerTriggerRemovePacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("triggerID", Type.U32)
    }
}

M.ClientTriggerEnterPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("triggerID", Type.U32)
    }
}

M.ClientPathGeneratePacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("start", Type.VEC3),
        Field("end", Type.VEC3)
    }
}

M.ServerPathVisualizationPacket =
{
    archetype = Archetype.Packet,

    fields = {}
}

M.ClientSendCheatCommandPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("command", Type.U8)
    }
}

M.ServerCheatCommandResultPacket =
{
    archetype = Archetype.Packet,

    fields =
    {
        Field("command", Type.U8),
        Field("result", Type.U8),
        Field("response", Type.STRING)
    }
}

return M
local Type = require("Type")
local D = require("Definition")

return D.Definitions
{
    D.Enum("InteractionCapabilityMaskEnum", Type.U8,
    {
        D.Field("None", 0x0),
        D.Field("Gossip", 0x1),
        D.Field("Vendor", 0x2),
        D.Field("Trainer", 0x4),
        D.Field("QuestGiver", 0x8),
        D.Field("All", 0xF)
    }),

    D.Enum("InteractionSurfaceTypeEnum", Type.U8,
    {
        D.Field("Gossip"),
        D.Field("Count")
    }),

    D.Enum("InteractionRangePolicyEnum", Type.U8,
    {
        D.Field("Default"),
        D.Field("Distance"),
        D.Field("Count")
    }),

    D.Enum("InteractionCloseReasonEnum", Type.U8,
    {
        D.Field("ClientRequest"),
        D.Field("Replaced"),
        D.Field("Timeout"),
        D.Field("OutOfRange"),
        D.Field("SourceUnavailable"),
        D.Field("PlayerStateChanged"),
        D.Field("WorldTransfer"),
        D.Field("Disconnect"),
        D.Field("ContentInvalidated"),
        D.Field("ScriptReloadInvalidated"),
        D.Field("Count")
    }),

    D.Enum("InteractionResultEnum", Type.U8,
    {
        D.Field("Unavailable"),
        D.Field("InvalidSelection"),
        D.Field("ConditionsChanged"),
        D.Field("ActionFailed"),
        D.Field("Succeeded"),
        D.Field("Count")
    }),

    D.Enum("ConditionGroupOperatorEnum", Type.U8,
    {
        D.Field("All"),
        D.Field("Any"),
        D.Field("Count")
    }),

    D.Enum("ConditionComparisonEnum", Type.U8,
    {
        D.Field("None"),
        D.Field("Equal"),
        D.Field("NotEqual"),
        D.Field("Less"),
        D.Field("LessOrEqual"),
        D.Field("Greater"),
        D.Field("GreaterOrEqual"),
        D.Field("HasAllBits"),
        D.Field("HasAnyBits"),
        D.Field("Count")
    }),

    D.Enum("ConditionTypeEnum", Type.U16,
    {
        D.Field("Invalid"),
        D.Field("PlayerLevel"),
        D.Field("PlayerFaction"),
        D.Field("Count")
    }),

    D.Enum("InteractionEditorArtifactEnum", Type.U8,
    {
        D.Field("LocalizedText"),
        D.Field("LocalizedTextTranslation"),
        D.Field("ConditionDescriptor"),
        D.Field("ConditionSet"),
        D.Field("ConditionGroup"),
        D.Field("Condition"),
        D.Field("GossipActionDescriptor"),
        D.Field("GossipMenu"),
        D.Field("GossipMenuOption"),
        D.Field("CreatureTemplateDescriptor"),
        D.Field("CreatureTemplateInteraction"),
        D.Field("CreatureTemplateGossip"),
        D.Field("Count")
    })
}

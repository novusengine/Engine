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
        D.Field("Menu"),
        D.Field("Vendor"),
        D.Field("Quest"),
        D.Field("Count")
    }),

    D.Enum("InteractionProviderFlagsEnum", Type.U32,
    {
        D.Field("None", 0x0),
        D.Field("SuppressAutomaticVendor", 0x1),
        D.Field("SuppressAutomaticQuests", 0x2),
        D.Field("All", 0x3)
    }),

    D.Enum("QuestInteractionMaskEnum", Type.U8,
    {
        D.Field("None", 0x0),
        D.Field("FutureOffer", 0x1),
        D.Field("Offers", 0x2),
        D.Field("InProgress", 0x4),
        D.Field("Completable", 0x8),
        D.Field("All", 0xF)
    }),

    D.Enum("QuestAssociationRoleMaskEnum", Type.U8,
    {
        D.Field("None", 0x0),
        D.Field("Start", 0x1),
        D.Field("TurnIn", 0x2),
        D.Field("All", 0x3)
    }),

    D.Enum("QuestAssociationFlagsEnum", Type.U32,
    {
        D.Field("None", 0x0),
        D.Field("CloseOnAccept", 0x1),
        D.Field("All", 0x1)
    }),

    D.Enum("QuestFlagsEnum", Type.U32,
    {
        D.Field("None", 0x0),
        D.Field("ShowWhenUnavailable", 0x1),
        D.Field("All", 0x1)
    }),

    D.Enum("QuestObjectiveTypeEnum", Type.U8,
    {
        D.Field("Invalid"),
        D.Field("KillCreature"),
        D.Field("CollectItem"),
        D.Field("Count")
    }),

    D.Enum("QuestObjectiveFlagsEnum", Type.U32,
    {
        D.Field("None", 0x0),
        D.Field("Optional", 0x1),
        D.Field("DoNotConsume", 0x2),
        D.Field("All", 0x3)
    }),

    D.Enum("QuestRewardFlagsEnum", Type.U32,
    {
        D.Field("None", 0x0),
        D.Field("Hidden", 0x1),
        D.Field("All", 0x1)
    }),

    D.Enum("QuestRewardTypeEnum", Type.U8,
    {
        D.Field("Item"),
        D.Field("Currency"),
        D.Field("Reputation"),
        D.Field("Experience"),
        D.Field("Count")
    }),

    D.Enum("InteractionOptionKindEnum", Type.U8,
    {
        D.Field("Generic"),
        D.Field("QuestAccept"),
        D.Field("QuestComplete"),
        D.Field("ReturnToMenu"),
        D.Field("Count")
    }),

    D.Enum("CharacterQuestStateEnum", Type.U8,
    {
        D.Field("Active"),
        D.Field("ReadyToTurnIn"),
        D.Field("Count")
    }),

    D.Enum("QuestConditionStateEnum", Type.U8,
    {
        D.Field("NotStarted"),
        D.Field("Active"),
        D.Field("ReadyToTurnIn"),
        D.Field("Completed"),
        D.Field("Count")
    }),

    D.Enum("InventoryScopeEnum", Type.U8,
    {
        D.Field("CarriedBags"),
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
        D.Field("PlayerClass"),
        D.Field("PlayerRace"),
        D.Field("Reputation"),
        D.Field("QuestState"),
        D.Field("ItemCount"),
        D.Field("CurrencyAmount"),
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
        D.Field("Cost"),
        D.Field("CostCurrencyComponent"),
        D.Field("CostItemComponent"),
        D.Field("VendorList"),
        D.Field("VendorListItem"),
        D.Field("CreatureTemplateVendor"),
        D.Field("QuestTemplate"),
        D.Field("QuestPrerequisite"),
        D.Field("CreatureTemplateQuestGiver"),
        D.Field("CreatureTemplateQuest"),
        D.Field("QuestObjective"),
        D.Field("QuestObjectiveTarget"),
        D.Field("QuestRewardGroup"),
        D.Field("QuestRewardItem"),
        D.Field("QuestRewardCurrency"),
        D.Field("QuestRewardReputation"),
        D.Field("QuestRewardExperience"),
        D.Field("Count")
    })
}

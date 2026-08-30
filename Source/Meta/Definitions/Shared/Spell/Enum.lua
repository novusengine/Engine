local Type = require("Type")
local D = require("Definition")
local E = require("SpellEffectDefinition")
local SpellEffectCatalog = require("SpellEffectCatalog")

return D.Definitions
{
    D.Enum("SpellProcPhaseTypeEnum", Type.U8,
    {
        D.Field("OnSpellCast"),
        D.Field("OnSpellHandleEffect"),
        D.Field("OnSpellFinish"),
        D.Field("OnAuraApply"),
        D.Field("OnAuraHandleEffect"),
        D.Field("OnAuraRemove"),
        D.Field("Count")
    }),

    D.Enum("SpellProcPhaseMaskEnum", Type.U32,
    {
        D.Field("None", 0x0),
        D.Field("OnSpellCast", 0x1),
        D.Field("OnSpellHandleEffect", 0x2),
        D.Field("OnSpellFinish", 0x4),
        D.Field("OnAuraApply", 0x8),
        D.Field("OnAuraHandleEffect", 0x10),
        D.Field("OnAuraRemove", 0x20),
        D.Field("All", 0xFFFFFFFF)
    }),

    D.Enum("SpellProcTypeMaskEnum", Type.U32,
    {
        D.Field("None", 0x0),
        D.Field("All", 0xFFFFFFFF)
    }),

    D.Enum("SpellProcHitMaskEnum", Type.U64,
    {
        D.Field("None", 0x0),
        D.Field("Normal", 0x1),
        D.Field("Critical", 0x2),
        D.Field("Miss", 0x4),
        D.Field("All", 0xFFFFFFFF)
    }),

    D.Enum("SpellProcFlagEnum", Type.U64,
    {
        D.Field("None", 0x0),
        D.Field("UseUnitICD", 0x1),
        D.Field("All", 0xFFFFFFFF)
    }),

    E.Catalog("SpellEffectTypeEnum", Type.U8, SpellEffectCatalog),

    D.Enum("SpellEffectWeaponDamageValueEnum", Type.U8,
    {
        D.Field("Invalid"),
        D.Field("Base"),
        D.Field("Normalized"),
        D.Field("Count")
    }),

    D.Enum("SpellEffectWeaponSlotEnum", Type.U8,
    {
        D.Field("Invalid"),
        D.Field("MainHand"),
        D.Field("OffHand"),
        D.Field("Count")
    }),

    D.Enum("SpellTargetSelectorEnum", Type.U8,
    {
        D.Field("None"),
        D.Field("Caster"),
        D.Field("CasterTarget"),
        D.Field("GroundPosition"),
        D.Field("Count")
    }),

    D.Enum("SpellTargetShapeEnum", Type.U8,
    {
        D.Field("Single"),
        D.Field("Radius"),
        D.Field("Count")
    }),

    D.Enum("SpellTargetRelationEnum", Type.U8,
    {
        D.Field("Any"),
        D.Field("Self"),
        D.Field("Friendly"),
        D.Field("Attackable"),
        D.Field("Count")
    }),

    D.Enum("SpellTargetRecipientMaskEnum", Type.U8,
    {
        D.Field("None", 0x0),
        D.Field("Self", 0x1),
        D.Field("Friendly", 0x2),
        D.Field("Attackable", 0x4),
        D.Field("Other", 0x8),
        D.Field("AnyExceptSelf", 0xE),
        D.Field("Any", 0xF)
    }),

    D.Enum("SpellRangePolicyEnum", Type.U8,
    {
        D.Field("None"),
        D.Field("Melee"),
        D.Field("Distance"),
        D.Field("Count")
    }),

    D.Enum("AuraDispositionEnum", Type.U8,
    {
        D.Field("None"),
        D.Field("Beneficial"),
        D.Field("Harmful"),
        D.Field("Count")
    }),

    D.Enum("AuraDispelTypeEnum", Type.U8,
    {
        D.Field("None"),
        D.Field("Magic"),
        D.Field("Curse"),
        D.Field("Disease"),
        D.Field("Poison"),
        D.Field("Enrage"),
        D.Field("Count")
    }),

    D.Enum("AuraDispelTypeMaskEnum", Type.U32,
    {
        D.Field("None", 0x0),
        D.Field("Magic", 0x1),
        D.Field("Curse", 0x2),
        D.Field("Disease", 0x4),
        D.Field("Poison", 0x8),
        D.Field("Enrage", 0x10),
        D.Field("All", 0x1F)
    }),

    D.Enum("AuraApplicationPolicyEnum", Type.U8,
    {
        D.Field("UniquePerTarget"),
        D.Field("PerCaster"),
        D.Field("Count")
    }),

    D.Enum("AuraApplicationOutcomeEnum", Type.U8,
    {
        D.Field("Applied"),
        D.Field("Refreshed"),
        D.Field("Replaced"),
        D.Field("Immune"),
        D.Field("Resisted"),
        D.Field("Rejected"),
        D.Field("Count")
    }),

    D.Enum("AuraConstraintScopeEnum", Type.U8,
    {
        D.Field("Target"),
        D.Field("CasterTarget"),
        D.Field("CasterWorld"),
        D.Field("Count")
    }),

    D.Enum("AuraConstraintOverflowEnum", Type.U8,
    {
        D.Field("ReplaceOldest"),
        D.Field("RejectNew"),
        D.Field("Count")
    }),

    D.Enum("AuraConstraintOverrideFlagsEnum", Type.U8,
    {
        D.Field("None", 0x0),
        D.Field("Scope", 0x1),
        D.Field("MaximumApplications", 0x2),
        D.Field("OverflowBehavior", 0x4)
    }),

    D.Enum("AuraRemovalReasonEnum", Type.U8,
    {
        D.Field("Expired"),
        D.Field("Explicit"),
        D.Field("Replaced"),
        D.Field("Dispelled"),
        D.Field("TargetDeath"),
        D.Field("CasterDeath"),
        D.Field("TargetWorldExit"),
        D.Field("CasterWorldExit"),
        D.Field("Count")
    }),

    D.Enum("SpellEditorArtifactEnum", Type.U8,
    {
        D.Field("Spell"),
        D.Field("SpellEffects"),
        D.Field("SpellProcData"),
        D.Field("SpellProcLink"),
        D.Field("SpellAura"),
        D.Field("SpellAuraConstraintGroup"),
        D.Field("SpellAuraConstraint"),
        D.Field("Count")
    }),

    D.Enum("AuraLifecycleFlagsEnum", Type.U8,
    {
        D.Field("None", 0x0),
        D.Field("PersistThroughTargetDeath", 0x1),
        D.Field("RemoveOnCasterDeath", 0x2),
        D.Field("RemoveOnCasterWorldExit", 0x4)
    })
}

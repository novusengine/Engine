local Type = require("Type")
local D = require("Definition")

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
        D.Field("All", 0xFFFFFFFF)
    }),

    D.Enum("SpellProcFlagEnum", Type.U64,
    {
        D.Field("None", 0x0),
        D.Field("UseUnitICD", 0x1),
        D.Field("All", 0xFFFFFFFF)
    }),

    D.Enum("SpellEffectTypeEnum", Type.U8,
    {
        D.Field("Invalid"),
        D.Field("Dummy"),
        D.Field("WeaponDamage"),
        D.Field("AuraApply", 128),
        D.Field("AuraRemove"),
        D.Field("AuraPeriodicDamage"),
        D.Field("AuraPeriodicHeal"),
        D.Field("Count")
    }),

    D.Enum("SpellEffectWeaponDamageValueEnum", Type.U8,
    {
        D.Field("Invalid"),
        D.Field("Base"),
        D.Field("Normalized"),
        D.Field("Count")
    })
}

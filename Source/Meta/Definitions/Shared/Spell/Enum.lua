local M = OrderedTable()

local Type = require("Type")
local Archetype = require("Archetype")
local Component = require("Component")

M.SpellProcPhaseTypeEnum =
{
    archetype = Archetype.Enum,
    type = Type.U8,

    fields = 
    {
        Field("OnSpellCast"),
        Field("OnSpellHandleEffect"),
        Field("OnSpellFinish"),
        Field("OnAuraApply"),
        Field("OnAuraHandleEffect"),
        Field("OnAuraRemove"),
        Field("Count")
    }
}

M.SpellProcPhaseMaskEnum =
{
    archetype = Archetype.Enum,
    type = Type.U32,

    fields = 
    {
        Field("None", 0x0),
        Field("OnSpellCast", 0x1),
        Field("OnSpellHandleEffect", 0x2),
        Field("OnSpellFinish", 0x4),
        Field("OnAuraApply", 0x8),
        Field("OnAuraHandleEffect", 0x10),
        Field("OnAuraRemove", 0x20),
        Field("All", 0xFFFFFFFF)
    }
}

M.SpellProcTypeMaskEnum =
{
    archetype = Archetype.Enum,
    type = Type.U32,

    fields = 
    {
        Field("None", 0x0),
        Field("All", 0xFFFFFFFF)
    }
}

M.SpellProcHitMaskEnum =
{
    archetype = Archetype.Enum,
    type = Type.U64,

    fields = 
    {
        Field("None", 0x0),
        Field("Normal", 0x1),
        Field("All", 0xFFFFFFFF)
    }
}

M.SpellProcFlagEnum =
{
    archetype = Archetype.Enum,
    type = Type.U64,

    fields = 
    {
        Field("None", 0x0),
        Field("UseUnitICD", 0x1),
        Field("All", 0xFFFFFFFF)
    }
}

M.SpellEffectTypeEnum =
{
    archetype = Archetype.Enum,
    type = Type.U8,

    fields = 
    {
        Field("Invalid"),
        Field("Dummy"),
        Field("WeaponDamage"),
        Field("AuraApply", 128),
        Field("AuraRemove"),
        Field("AuraPeriodicDamage"),
        Field("AuraPeriodicHeal"),
        Field("Count")
    }
}

M.SpellEffectWeaponDamageValueEnum =
{
    archetype = Archetype.Enum,
    type = Type.U8,

    fields = 
    {
        Field("Invalid"),
        Field("Base"),
        Field("Normalized"),
        Field("Count")
    }
}

return M
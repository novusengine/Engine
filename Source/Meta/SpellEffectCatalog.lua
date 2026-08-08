local E = require("SpellEffectDefinition")

local U16_MAX = 65535
local OwnerType = E.OwnerType
local TargetMode = E.TargetMode
local TargetKind = E.TargetKind
local TargetState = E.TargetState

local DAMAGE_TYPE_OPTIONS =
{
    E.Option(1, "Base"),
    E.Option(2, "Normalized")
}

local WEAPON_SLOT_OPTIONS =
{
    E.Option(1, "MainHand"),
    E.Option(2, "OffHand")
}

local REACTION_OPTIONS =
{
    E.Option(0, "Hostile"),
    E.Option(1, "Unfriendly"),
    E.Option(2, "Neutral"),
    E.Option(3, "Friendly")
}

local AURA_DISPOSITION_OPTIONS =
{
    E.Option(1, "Beneficial"),
    E.Option(2, "Harmful")
}

local AURA_DISPEL_TYPE_MASK_OPTIONS =
{
    E.Option(0x1, "Magic"),
    E.Option(0x2, "Curse"),
    E.Option(0x4, "Disease"),
    E.Option(0x8, "Poison"),
    E.Option(0x10, "Enrage")
}

return
{
    E.Effect(0, "Invalid",
    {
        owner = OwnerType.None,
        target = E.Target(TargetMode.None, TargetKind.None, TargetState.Any)
    }),

    E.Effect(1, "Dummy",
    {
        owner = OwnerType.Spell,
        target = E.Target(TargetMode.None, TargetKind.None, TargetState.Any)
    }),

    E.Effect(2, "WeaponDamage",
    {
        owner = OwnerType.Spell,
        target = E.Target(TargetMode.Required, TargetKind.Unit, TargetState.Alive),
        parameters =
        {
            E.Enum("damageType", "SpellEffectWeaponDamageValueEnum", { default = 1, minimum = 1, maximum = 2, values = DAMAGE_TYPE_OPTIONS }),
            E.Enum("weaponSlot", "SpellEffectWeaponSlotEnum", { default = 1, minimum = 1, maximum = 2, values = WEAPON_SLOT_OPTIONS }),
            E.Integer("minimumDamage", { minimum = 0 }),
            E.Integer("maximumDamage", { minimum = 0 })
        },
        constraints =
        {
            E.LessThanOrEqual("minimumDamage", "maximumDamage")
        }
    }),

    E.Effect(3, "Damage",
    {
        owner = OwnerType.Spell,
        target = E.Target(TargetMode.Required, TargetKind.Unit, TargetState.Alive),
        parameters =
        {
            E.Integer("minimumDamage", { minimum = 0 }),
            E.Integer("maximumDamage", { minimum = 0 })
        },
        constraints =
        {
            E.LessThanOrEqual("minimumDamage", "maximumDamage")
        }
    }),

    E.Effect(4, "Heal",
    {
        owner = OwnerType.Spell,
        target = E.Target(TargetMode.Required, TargetKind.Unit, TargetState.Alive),
        parameters =
        {
            E.Integer("minimumHealing", { minimum = 0 }),
            E.Integer("maximumHealing", { minimum = 0 })
        },
        constraints =
        {
            E.LessThanOrEqual("minimumHealing", "maximumHealing")
        }
    }),

    E.Effect(5, "DamagePercent",
    {
        owner = OwnerType.Spell,
        target = E.Target(TargetMode.Required, TargetKind.Unit, TargetState.Alive),
        parameters =
        {
            E.Percentage("percentage")
        }
    }),

    E.Effect(6, "HealPercent",
    {
        owner = OwnerType.Spell,
        target = E.Target(TargetMode.Required, TargetKind.Unit, TargetState.Alive),
        parameters =
        {
            E.Percentage("percentage")
        }
    }),

    E.Effect(7, "Dispel",
    {
        owner = OwnerType.Spell,
        target = E.Target(TargetMode.Required, TargetKind.Unit, TargetState.Alive),
        parameters =
        {
            E.Flags("dispelTypeMask", "AuraDispelTypeMaskEnum", { default = 0x1, minimum = 1, maximum = 0x1F, values = AURA_DISPEL_TYPE_MASK_OPTIONS }),
            E.Enum("auraDisposition", "AuraDispositionEnum", { default = 2, minimum = 1, maximum = 2, values = AURA_DISPOSITION_OPTIONS }),
            E.Integer("maximumAuras", { default = 1, minimum = 1, maximum = 16 })
        }
    }),

    E.Effect(128, "ApplyAura",
    {
        owner = OwnerType.Spell,
        target = E.Target(TargetMode.Required, TargetKind.Unit, TargetState.Any),
        parameters =
        {
            E.SpellID("auraSpellID", { default = 1, minimum = 1 }),
            E.Integer("applicationStacks", { minimum = 0, maximum = U16_MAX })
        }
    }),

    E.Effect(129, "AuraRemove",
    {
        owner = OwnerType.Spell,
        target = E.Target(TargetMode.Required, TargetKind.Unit, TargetState.Any),
        parameters =
        {
            E.SpellID("auraSpellID", { default = -1, minimum = -1 }),
            E.Integer("stacks", { minimum = 0, maximum = U16_MAX })
        }
    }),

    E.Effect(130, "AuraPeriodicDamage",
    {
        owner = OwnerType.Aura,
        periodic = true,
        target = E.Target(TargetMode.Required, TargetKind.Unit, TargetState.Alive),
        parameters =
        {
            E.IntervalMilliseconds("interval", { default = 1000, minimum = 1 }),
            E.Integer("minimumDamage", { minimum = 0 }),
            E.Integer("maximumDamage", { minimum = 0 })
        },
        constraints =
        {
            E.LessThanOrEqual("minimumDamage", "maximumDamage")
        }
    }),

    E.Effect(131, "AuraPeriodicHeal",
    {
        owner = OwnerType.Aura,
        periodic = true,
        target = E.Target(TargetMode.Required, TargetKind.Unit, TargetState.Alive),
        parameters =
        {
            E.IntervalMilliseconds("interval", { default = 1000, minimum = 1 }),
            E.Integer("minimumHealing", { minimum = 0 }),
            E.Integer("maximumHealing", { minimum = 0 })
        },
        constraints =
        {
            E.LessThanOrEqual("minimumHealing", "maximumHealing")
        }
    }),

    E.Effect(132, "AuraFactionOverride",
    {
        owner = OwnerType.Aura,
        target = E.Target(TargetMode.Required, TargetKind.Unit, TargetState.Any),
        parameters =
        {
            E.FactionID("factionID", { minimum = 0, maximum = U16_MAX })
        }
    }),

    E.Effect(133, "AuraFactionStandingOverride",
    {
        owner = OwnerType.Aura,
        target = E.Target(TargetMode.Required, TargetKind.Unit, TargetState.Any),
        parameters =
        {
            E.FactionID("factionID", { default = 1, minimum = 1, maximum = U16_MAX }),
            E.Standing("standing")
        }
    }),

    E.Effect(134, "AuraFactionReactionOverride",
    {
        owner = OwnerType.Aura,
        target = E.Target(TargetMode.Required, TargetKind.Unit, TargetState.Any),
        parameters =
        {
            E.FactionID("factionID", { default = 1, minimum = 1, maximum = U16_MAX }),
            E.Enum("reaction", "Gameplay::Faction::Reaction", { minimum = 0, maximum = 3, values = REACTION_OPTIONS })
        }
    }),

    E.Effect(135, "AuraFactionPlayerReactionMinOverride",
    {
        owner = OwnerType.Aura,
        target = E.Target(TargetMode.Required, TargetKind.Unit, TargetState.Any),
        parameters =
        {
            E.Enum("minimumReaction", "Gameplay::Faction::Reaction", { minimum = 0, maximum = 3, values = REACTION_OPTIONS })
        }
    }),

    E.Effect(136, "AuraFactionPlayerReactionMaxOverride",
    {
        owner = OwnerType.Aura,
        target = E.Target(TargetMode.Required, TargetKind.Unit, TargetState.Any),
        parameters =
        {
            E.Enum("maximumReaction", "Gameplay::Faction::Reaction", { default = 3, minimum = 0, maximum = 3, values = REACTION_OPTIONS })
        }
    }),

    E.Effect(137, "AuraFactionUnitReactionOverride",
    {
        owner = OwnerType.Aura,
        target = E.Target(TargetMode.Required, TargetKind.Unit, TargetState.Any),
        parameters =
        {
            E.Enum("reaction", "Gameplay::Faction::Reaction", { minimum = 0, maximum = 3, values = REACTION_OPTIONS })
        }
    }),

    E.Effect(138, "AuraPeriodicDamagePercent",
    {
        owner = OwnerType.Aura,
        periodic = true,
        target = E.Target(TargetMode.Required, TargetKind.Unit, TargetState.Alive),
        parameters =
        {
            E.IntervalMilliseconds("interval", { default = 1000, minimum = 1 }),
            E.Percentage("percentage")
        }
    }),

    E.Effect(139, "AuraPeriodicHealPercent",
    {
        owner = OwnerType.Aura,
        periodic = true,
        target = E.Target(TargetMode.Required, TargetKind.Unit, TargetState.Alive),
        parameters =
        {
            E.IntervalMilliseconds("interval", { default = 1000, minimum = 1 }),
            E.Percentage("percentage")
        }
    }),

    E.Effect(140, "Count",
    {
        owner = OwnerType.None,
        target = E.Target(TargetMode.None, TargetKind.None, TargetState.Any)
    })
}

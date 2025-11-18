type SpellProcPhaseType<Enum> =
{
    Name : "SpellProcPhaseType";
    Type : u8;

    Fields :
    {
        OnSpellCast         : Auto,
        OnSpellHandleEffect : Auto,
        OnSpellFinish       : Auto,
        OnAuraApply         : Auto,
        OnAuraHandleEffect  : Auto,
        OnAuraRemove        : Auto,
        Count               : Auto
    };
};

type SpellProcPhaseMask<Enum> =
{
    Name : "SpellProcPhaseMask";
    Type : u32;

    Fields :
    {
        None                : Value<0x0>,
        OnSpellCast         : Value<0x1>,
        OnSpellHandleEffect : Value<0x2>,
        OnSpellFinish       : Value<0x4>,
        OnAuraApply         : Value<0x8>,
        OnAuraHandleEffect  : Value<0x10>,
        OnAuraRemove        : Value<0x20>,
        All                 : Value<0xFFFFFFFF>
    };
};

type SpellProcTypeMask<Enum> =
{
    Name : "SpellProcTypeMask";
    Type : u64;

    Fields :
    {
        None                : Value<0x0>,
        All                 : Value<0xFFFFFFFFFFFFFFFF>
    };
};

type SpellProcHitMask<Enum> =
{
    Name : "SpellProcHitMask";
    Type : u64;

    Fields :
    {
        None                : Value<0x0>,
        Normal              : Value<0x1>,
        All                 : Value<0xFFFFFFFFFFFFFFFF>
    };
};

type SpellProcFlag<Enum> =
{
    Name : "SpellProcFlag";
    Type : u64;

    Fields :
    {
        None                : Value<0x0>,
        UseUnitICD          : Value<0x1>,
        All                 : Value<0xFFFFFFFFFFFFFFFF>
    };
};

type SpellEffectType<Enum> =
{
    Name : "SpellEffectType";
    Type : u8;

    Fields :
    {
        Invalid : Auto,
        Dummy : Auto,
        WeaponDamage : Auto,
        AuraApply : Value<128>,
        AuraRemove : Auto,
        AuraPeriodicDamage : Auto,
        AuraPeriodicHeal : Auto,
        Count : Auto
    };
};

type SpellEffectWeaponDamageValue<Enum> =
{
    Name : "SpellEffectWeaponDamageValue";
    Type : u8;

    Fields :
    {
        Invalid : Auto,
        Base : Auto,
        Normalized : Auto,
        Count : Auto
    };
};
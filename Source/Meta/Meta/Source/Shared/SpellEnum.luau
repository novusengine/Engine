type SpellEffectType<Enum> =
{
    Name : "SpellEffectType";
    Type : u8;

    Fields :
    {
        Invalid : Auto,
        WeaponDamage : Auto,
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
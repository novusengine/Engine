type CombatLogEvents<Enum> =
{
    Name : "CombatLogEvents";
    Type : u16;

    Fields :
    {
        DamageDealt : Auto,
        HealingDone : Auto,
        Resurrected : Auto,
        Count : Auto
    };
};
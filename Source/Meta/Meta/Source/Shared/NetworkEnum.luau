type CheatCommand<Enum> =
{
    Name : "CheatCommand";
    Type : u8;

    Fields :
    {
        None                    : Auto,
        Damage                  : Auto,
        Heal                    : Auto,
        Kill                    : Auto,
        Resurrect               : Auto,
        UnitMorph               : Auto,
        UnitDemorph             : Auto,
        Teleport                : Auto,
        CharacterAdd            : Auto,
        CharacterRemove         : Auto,
        UnitSetRace             : Auto,
        UnitSetGender           : Auto,
        UnitSetClass            : Auto,
        UnitSetLevel            : Auto,
        ItemSetTemplate         : Auto,
        ItemSetStatTemplate     : Auto,
        ItemSetArmorTemplate    : Auto,
        ItemSetWeaponTemplate   : Auto,
        ItemSetShieldTemplate   : Auto,
        ItemAdd                 : Auto,
        ItemRemove              : Auto,
        CreatureAdd             : Auto,
        CreatureRemove          : Auto,
        CreatureInfo            : Auto,
        MapAdd                  : Auto,
        GotoAdd                 : Auto,
        GotoAddHere             : Auto,
        GotoRemove              : Auto,
        GotoMap                 : Auto,
        GotoLocation            : Auto,
        GotoXYZ                 : Auto,
        TriggerAdd              : Auto,
        TriggerRemove           : Auto
    };
};
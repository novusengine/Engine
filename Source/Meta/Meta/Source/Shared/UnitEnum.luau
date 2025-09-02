type PowerType<Enum> =
{
    Name : "PowerType";
    Type : i8;

    Fields :
    {
        Health : Value<-2>,
        Mana : Value<0>,
        Rage : Auto,
        Focus : Auto,
        Energy : Auto,
        Happiness : Auto,
        Count : Auto
    };
};

type StatType<Enum> =
{
    Name : "StatType";
    Type : u8;

    Fields :
    {
        Strength : Auto,
        Agility : Auto,
        Stamina : Auto,
        Intellect : Auto,
        Spirit : Auto,
        Count : Auto
    };
};

type ResistanceType<Enum> =
{
    Name : "ResistanceType";
    Type : u8;

    Fields :
    {
        Normal : Auto,
        Holy : Auto,
        Fire : Auto,
        Nature : Auto,
        Frost : Auto,
        Shadow : Auto,
        Arcane : Auto,
        Count : Auto
    };
};

type ItemEquipSlot<Enum> =
{
    Name : "ItemEquipSlot";
    Type : u8;

    Fields :
    {
        EquipmentStart : Value<0>,
        Helm : Value<0>,
        Necklace : Auto,
        Shoulders : Auto,
        Cloak : Auto,
        Chest : Auto,
        Shirt : Auto,
        Tabard : Auto,
        Bracers : Auto,
        Gloves : Auto,
        Belt : Auto,
        Pants : Auto,
        Boots : Auto,
        Ring2 : Auto,
        Ring1 : Auto,
        Trinket1 : Auto,
        Trinket2 : Auto,
        MainHand : Auto,
        OffHand : Auto,
        Ranged : Auto,
        EquipmentEnd : Value<18>,
        MainBag : Auto,
        BagStart : Value<19>,
        Bag1 : Auto,
        Bag2 : Auto,
        Bag3 : Auto,
        Bag4 : Auto,
        BagEnd : Value<23>,
        Count : Auto
    };
};
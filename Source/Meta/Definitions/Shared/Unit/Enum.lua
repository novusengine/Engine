local Type = require("Type")
local D = require("Definition")

return D.Definitions
{
    D.Enum("PowerTypeEnum", Type.U8,
    {
        D.Field("Invalid"),
        D.Field("Health"),
        D.Field("Mana"),
        D.Field("Rage"),
        D.Field("Focus"),
        D.Field("Energy"),
        D.Field("Happiness"),
        D.Field("Count")
    }),

    D.Enum("StatTypeEnum", Type.U8,
    {
        D.Field("Invalid"),
        D.Field("Health"),
        D.Field("Stamina"),
        D.Field("Strength"),
        D.Field("Agility"),
        D.Field("Intellect"),
        D.Field("Spirit"),
        D.Field("Armor"),
        D.Field("AttackPower"),
        D.Field("SpellPower"),
        D.Field("Count")
    }),

    D.Enum("ResistanceTypeEnum", Type.U8,
    {
        D.Field("Invalid"),
        D.Field("Holy"),
        D.Field("Fire"),
        D.Field("Nature"),
        D.Field("Frost"),
        D.Field("Shadow"),
        D.Field("Arcane"),
        D.Field("Count")
    }),

    D.Enum("ItemEquipSlotEnum", Type.U8,
    {
        D.Field("EquipmentStart", 0),
        D.Field("Helm", 0),
        D.Field("Necklace"),
        D.Field("Shoulders"),
        D.Field("Cloak"),
        D.Field("Chest"),
        D.Field("Shirt"),
        D.Field("Tabard"),
        D.Field("Bracers"),
        D.Field("Gloves"),
        D.Field("Belt"),
        D.Field("Pants"),
        D.Field("Boots"),
        D.Field("Ring1"),
        D.Field("Ring2"),
        D.Field("Trinket1"),
        D.Field("Trinket2"),
        D.Field("MainHand"),
        D.Field("OffHand"),
        D.Field("Ranged"),
        D.Field("EquipmentEnd", 18),
        D.Field("MainBag"),
        D.Field("BagStart", 19),
        D.Field("Bag1"),
        D.Field("Bag2"),
        D.Field("Bag3"),
        D.Field("Bag4"),
        D.Field("BagEnd", 23),
        D.Field("Count")
    }),

    D.Enum("UnitClassEnum", Type.U8,
    {
        D.Field("Invalid"),
        D.Field("Warrior"),
        D.Field("Paladin"),
        D.Field("Hunter"),
        D.Field("Rogue"),
        D.Field("Priest"),
        D.Field("Shaman"),
        D.Field("Mage"),
        D.Field("Warlock"),
        D.Field("Druid"),
        D.Field("Count")
    })
}

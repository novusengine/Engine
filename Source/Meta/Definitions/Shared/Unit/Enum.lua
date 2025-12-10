local M = OrderedTable()

local Type = require("Type")
local Archetype = require("Archetype")
local Component = require("Component")

M.PowerTypeEnum =
{
    archetype = Archetype.Enum,
    type = Type.U8,

    fields = 
    {
        Field("Invalid"),
        Field("Health"),
        Field("Mana"),
        Field("Rage"),
        Field("Focus"),
        Field("Energy"),
        Field("Happiness"),
        Field("Count")
    }
}

M.StatTypeEnum =
{
    archetype = Archetype.Enum,
    type = Type.U8,

    fields = 
    {
        Field("Invalid"),
        Field("Health"),
        Field("Stamina"),
        Field("Strength"),
        Field("Agility"),
        Field("Intellect"),
        Field("Spirit"),
        Field("Armor"),
        Field("AttackPower"),
        Field("SpellPower"),
        Field("Count")
    }
}

M.ResistanceTypeEnum =
{
    archetype = Archetype.Enum,
    type = Type.U8,

    fields = 
    {
        Field("Invalid"),
        Field("Holy"),
        Field("Fire"),
        Field("Nature"),
        Field("Frost"),
        Field("Shadow"),
        Field("Arcane"),
        Field("Count")
    }
}

M.ItemEquipSlotEnum =
{
    archetype = Archetype.Enum,
    type = Type.U8,

    fields = 
    {
        Field("EquipmentStart", 0),
        Field("Helm", 0),
        Field("Necklace"),
        Field("Shoulders"),
        Field("Cloak"),
        Field("Chest"),
        Field("Shirt"),
        Field("Tabard"),
        Field("Bracers"),
        Field("Gloves"),
        Field("Belt"),
        Field("Pants"),
        Field("Boots"),
        Field("Ring1"),
        Field("Ring2"),
        Field("Trinket1"),
        Field("Trinket2"),
        Field("MainHand"),
        Field("OffHand"),
        Field("Ranged"),
        Field("EquipmentEnd", 18),
        Field("MainBag"),
        Field("BagStart", 19),
        Field("Bag1"),
        Field("Bag2"),
        Field("Bag3"),
        Field("Bag4"),
        Field("BagEnd", 23),
        Field("Count")
    }
}

M.UnitClassEnum =
{
    archetype = Archetype.Enum,
    type = Type.U8,

    fields = 
    {
        Field("Invalid"),
        Field("Warrior"),
        Field("Paladin"),
        Field("Hunter"),
        Field("Rogue"),
        Field("Priest"),
        Field("Shaman"),
        Field("Mage"),
        Field("Warlock"),
        Field("Druid"),
        Field("Count")
    }
}

return M
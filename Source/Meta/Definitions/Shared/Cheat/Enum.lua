local M = OrderedTable()

local Type = require("Type")
local Archetype = require("Archetype")
local Component = require("Component")

M.CheatCommandEnum =
{
    archetype = Archetype.Enum,
    type = Type.U8,

    fields = 
    {
        Field("None"),
        Field("Damage"),
        Field("Heal"),
        Field("Kill"),
        Field("Resurrect"),
        Field("UnitMorph"),
        Field("UnitDemorph"),
        Field("Teleport"),
        Field("CharacterAdd"),
        Field("CharacterRemove"),
        Field("UnitSetRace"),
        Field("UnitSetGender"),
        Field("UnitSetClass"),
        Field("UnitSetLevel"),
        Field("ItemSetTemplate"),
        Field("ItemSetStatTemplate"),
        Field("ItemSetArmorTemplate"),
        Field("ItemSetWeaponTemplate"),
        Field("ItemSetShieldTemplate"),
        Field("ItemAdd"),
        Field("ItemRemove"),
        Field("CreatureAdd"),
        Field("CreatureRemove"),
        Field("CreatureInfo"),
        Field("MapAdd"),
        Field("GotoAdd"),
        Field("GotoAddHere"),
        Field("GotoRemove"),
        Field("GotoMap"),
        Field("GotoLocation"),
        Field("GotoXYZ"),
        Field("TriggerAdd"),
        Field("TriggerRemove"),
        Field("SpellSet"),
        Field("SpellEffectSet"),
        Field("SpellProcDataSet"),
        Field("SpellProcLinkSet"),
        Field("CreatureAddScript"),
        Field("CreatureRemoveScript")
    }
}

return M
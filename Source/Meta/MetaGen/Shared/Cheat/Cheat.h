#pragma once

#include <Array>
#include <Base/Types.h>

namespace MetaGen::Shared::Cheat
{
    enum class CheatCommandEnum : u8
    {
        None = 0,
        Damage = 1,
        Heal = 2,
        Kill = 3,
        Resurrect = 4,
        UnitMorph = 5,
        UnitDemorph = 6,
        Teleport = 7,
        CharacterAdd = 8,
        CharacterRemove = 9,
        UnitSetRace = 10,
        UnitSetGender = 11,
        UnitSetClass = 12,
        UnitSetLevel = 13,
        ItemSetTemplate = 14,
        ItemSetStatTemplate = 15,
        ItemSetArmorTemplate = 16,
        ItemSetWeaponTemplate = 17,
        ItemSetShieldTemplate = 18,
        ItemAdd = 19,
        ItemRemove = 20,
        CreatureAdd = 21,
        CreatureRemove = 22,
        CreatureInfo = 23,
        MapAdd = 24,
        GotoAdd = 25,
        GotoAddHere = 26,
        GotoRemove = 27,
        GotoMap = 28,
        GotoLocation = 29,
        GotoXYZ = 30,
        TriggerAdd = 31,
        TriggerRemove = 32,
        SpellSet = 33,
        SpellEffectSet = 34,
        SpellProcDataSet = 35,
        SpellProcLinkSet = 36,
        CreatureAddScript = 37,
        CreatureRemoveScript = 38
    };
    struct CheatCommandEnumMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 12;
        static constexpr std::string_view ENUM_NAME = "CheatCommandEnum";
        static constexpr std::array<std::pair<std::string_view, u8>, 39> ENUM_FIELD_LIST = { std::pair("None", 0u), std::pair("Damage", 1u), std::pair("Heal", 2u), std::pair("Kill", 3u), std::pair("Resurrect", 4u), std::pair("UnitMorph", 5u), std::pair("UnitDemorph", 6u), std::pair("Teleport", 7u), std::pair("CharacterAdd", 8u), std::pair("CharacterRemove", 9u), std::pair("UnitSetRace", 10u), std::pair("UnitSetGender", 11u), std::pair("UnitSetClass", 12u), std::pair("UnitSetLevel", 13u), std::pair("ItemSetTemplate", 14u), std::pair("ItemSetStatTemplate", 15u), std::pair("ItemSetArmorTemplate", 16u), std::pair("ItemSetWeaponTemplate", 17u), std::pair("ItemSetShieldTemplate", 18u), std::pair("ItemAdd", 19u), std::pair("ItemRemove", 20u), std::pair("CreatureAdd", 21u), std::pair("CreatureRemove", 22u), std::pair("CreatureInfo", 23u), std::pair("MapAdd", 24u), std::pair("GotoAdd", 25u), std::pair("GotoAddHere", 26u), std::pair("GotoRemove", 27u), std::pair("GotoMap", 28u), std::pair("GotoLocation", 29u), std::pair("GotoXYZ", 30u), std::pair("TriggerAdd", 31u), std::pair("TriggerRemove", 32u), std::pair("SpellSet", 33u), std::pair("SpellEffectSet", 34u), std::pair("SpellProcDataSet", 35u), std::pair("SpellProcLinkSet", 36u), std::pair("CreatureAddScript", 37u), std::pair("CreatureRemoveScript", 38u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(CheatCommandEnum);
}
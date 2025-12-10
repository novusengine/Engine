#pragma once

#include <Array>
#include <Base/Types.h>

namespace MetaGen::Shared::Unit
{
    enum class PowerTypeEnum : u8
    {
        Invalid = 0,
        Health = 1,
        Mana = 2,
        Rage = 3,
        Focus = 4,
        Energy = 5,
        Happiness = 6,
        Count = 7
    };
    struct PowerTypeEnumMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 25;
        static constexpr std::string_view ENUM_NAME = "PowerTypeEnum";
        static constexpr std::array<std::pair<std::string_view, u8>, 8> ENUM_FIELD_LIST = { std::pair("Invalid", 0u), std::pair("Health", 1u), std::pair("Mana", 2u), std::pair("Rage", 3u), std::pair("Focus", 4u), std::pair("Energy", 5u), std::pair("Happiness", 6u), std::pair("Count", 7u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(PowerTypeEnum);

    enum class StatTypeEnum : u8
    {
        Invalid = 0,
        Health = 1,
        Stamina = 2,
        Strength = 3,
        Agility = 4,
        Intellect = 5,
        Spirit = 6,
        Armor = 7,
        AttackPower = 8,
        SpellPower = 9,
        Count = 10
    };
    struct StatTypeEnumMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 26;
        static constexpr std::string_view ENUM_NAME = "StatTypeEnum";
        static constexpr std::array<std::pair<std::string_view, u8>, 11> ENUM_FIELD_LIST = { std::pair("Invalid", 0u), std::pair("Health", 1u), std::pair("Stamina", 2u), std::pair("Strength", 3u), std::pair("Agility", 4u), std::pair("Intellect", 5u), std::pair("Spirit", 6u), std::pair("Armor", 7u), std::pair("AttackPower", 8u), std::pair("SpellPower", 9u), std::pair("Count", 10u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(StatTypeEnum);

    enum class ResistanceTypeEnum : u8
    {
        Invalid = 0,
        Holy = 1,
        Fire = 2,
        Nature = 3,
        Frost = 4,
        Shadow = 5,
        Arcane = 6,
        Count = 7
    };
    struct ResistanceTypeEnumMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 27;
        static constexpr std::string_view ENUM_NAME = "ResistanceTypeEnum";
        static constexpr std::array<std::pair<std::string_view, u8>, 8> ENUM_FIELD_LIST = { std::pair("Invalid", 0u), std::pair("Holy", 1u), std::pair("Fire", 2u), std::pair("Nature", 3u), std::pair("Frost", 4u), std::pair("Shadow", 5u), std::pair("Arcane", 6u), std::pair("Count", 7u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(ResistanceTypeEnum);

    enum class ItemEquipSlotEnum : u8
    {
        EquipmentStart = 0,
        Helm = 0,
        Necklace = 1,
        Shoulders = 2,
        Cloak = 3,
        Chest = 4,
        Shirt = 5,
        Tabard = 6,
        Bracers = 7,
        Gloves = 8,
        Belt = 9,
        Pants = 10,
        Boots = 11,
        Ring1 = 12,
        Ring2 = 13,
        Trinket1 = 14,
        Trinket2 = 15,
        MainHand = 16,
        OffHand = 17,
        Ranged = 18,
        EquipmentEnd = 18,
        MainBag = 19,
        BagStart = 19,
        Bag1 = 20,
        Bag2 = 21,
        Bag3 = 22,
        Bag4 = 23,
        BagEnd = 23,
        Count = 24
    };
    struct ItemEquipSlotEnumMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 28;
        static constexpr std::string_view ENUM_NAME = "ItemEquipSlotEnum";
        static constexpr std::array<std::pair<std::string_view, u8>, 29> ENUM_FIELD_LIST = { std::pair("EquipmentStart", 0u), std::pair("Helm", 0u), std::pair("Necklace", 1u), std::pair("Shoulders", 2u), std::pair("Cloak", 3u), std::pair("Chest", 4u), std::pair("Shirt", 5u), std::pair("Tabard", 6u), std::pair("Bracers", 7u), std::pair("Gloves", 8u), std::pair("Belt", 9u), std::pair("Pants", 10u), std::pair("Boots", 11u), std::pair("Ring1", 12u), std::pair("Ring2", 13u), std::pair("Trinket1", 14u), std::pair("Trinket2", 15u), std::pair("MainHand", 16u), std::pair("OffHand", 17u), std::pair("Ranged", 18u), std::pair("EquipmentEnd", 18u), std::pair("MainBag", 19u), std::pair("BagStart", 19u), std::pair("Bag1", 20u), std::pair("Bag2", 21u), std::pair("Bag3", 22u), std::pair("Bag4", 23u), std::pair("BagEnd", 23u), std::pair("Count", 24u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(ItemEquipSlotEnum);

    enum class UnitClassEnum : u8
    {
        Invalid = 0,
        Warrior = 1,
        Paladin = 2,
        Hunter = 3,
        Rogue = 4,
        Priest = 5,
        Shaman = 6,
        Mage = 7,
        Warlock = 8,
        Druid = 9,
        Count = 10
    };
    struct UnitClassEnumMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 29;
        static constexpr std::string_view ENUM_NAME = "UnitClassEnum";
        static constexpr std::array<std::pair<std::string_view, u8>, 11> ENUM_FIELD_LIST = { std::pair("Invalid", 0u), std::pair("Warrior", 1u), std::pair("Paladin", 2u), std::pair("Hunter", 3u), std::pair("Rogue", 4u), std::pair("Priest", 5u), std::pair("Shaman", 6u), std::pair("Mage", 7u), std::pair("Warlock", 8u), std::pair("Druid", 9u), std::pair("Count", 10u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(UnitClassEnum);
}
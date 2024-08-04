#pragma once
#include "Base/Types.h"

namespace GameDefine
{
    enum class UnitRace : u8
    {
        None        = 0,
        Human       = 1,
        Orc         = 2,
        Dwarf       = 3,
        NightElf    = 4,
        Undead      = 5,
        Tauren      = 6,
        Gnome       = 7,
        Troll       = 8
    };

    enum class Gender : u8
    {
        None    = 0,
        Male    = 1,
        Female  = 2,
        Other   = 3
    };

    enum class UnitClass : u8
    {
        None    = 0,
        Warrior = 1,
        Paladin = 2,
        Hunter  = 3,
        Rogue   = 4,
        Priest  = 5,
        Shaman  = 7,
        Mage    = 8,
        Warlock = 9,
        Druid   = 11
    };

    enum class CurrencyType : u16
    {
        None = 0,
        Gold = 1,
    };
}
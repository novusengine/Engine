#pragma once
#include "Base/Types.h"

#include <memory>

class Bytebuffer;

namespace GameDefine
{
    enum class UnitRace : u8
    {
        None        = 0,
        Human       = 1,
        Start       = 1,
        Orc         = 2,
        Dwarf       = 3,
        NightElf    = 4,
        Undead      = 5,
        Tauren      = 6,
        Gnome       = 7,
        Troll       = 8,

        // Keep this updated
        Count       = Troll
    };

    enum class UnitGender : u8
    {
        None    = 0,
        Male    = 1,
        Start   = 1,
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

    namespace Database
    {
        struct ItemTemplate
        {
        public:
            u32 id;
            u32 displayID;
            u8 bind;
            u8 rarity;
            u8 category;
            u8 type;
            u16 virtualLevel;
            u16 requiredLevel;
            u32 durability;
            u32 iconID;

            std::string name;
            std::string description;

            u32 armor;
            u32 statTemplateID;
            u32 armorTemplateID;
            u32 weaponTemplateID;
            u32 shieldTemplateID;
            
        public:
            static bool Read(std::shared_ptr<Bytebuffer>& buffer, ItemTemplate& result);
            static bool Write(std::shared_ptr<Bytebuffer>& buffer, const ItemTemplate& data);
        };

        struct ItemStatTemplate
        {
        public:
            u32 id;
            u8 statTypes[8];
            i32 statValues[8];

        public:
            static bool Read(std::shared_ptr<Bytebuffer>& buffer, ItemStatTemplate& result);
            static bool Write(std::shared_ptr<Bytebuffer>& buffer, const ItemStatTemplate& data);
        };

        struct ItemArmorTemplate
        {
        public:
            u32 id;
            u8 equipType;
            u32 bonusArmor;

        public:
            static bool Read(std::shared_ptr<Bytebuffer>& buffer, ItemArmorTemplate& result);
            static bool Write(std::shared_ptr<Bytebuffer>& buffer, const ItemArmorTemplate& data);
        };

        struct ItemWeaponTemplate
        {
        public:
            u32 id;
            u8 weaponStyle;
            u32 minDamage;
            u32 maxDamage;
            f32 speed;

        public:
            static bool Read(std::shared_ptr<Bytebuffer>& buffer, ItemWeaponTemplate& result);
            static bool Write(std::shared_ptr<Bytebuffer>& buffer, const ItemWeaponTemplate& data);
        };

        struct ItemShieldTemplate
        {
        public:
            u32 id;
            u32 bonusArmor;
            u32 block;

        public:
            static bool Read(std::shared_ptr<Bytebuffer>& buffer, ItemShieldTemplate& result);
            static bool Write(std::shared_ptr<Bytebuffer>& buffer, const ItemShieldTemplate& data);
        };

        struct CreatureTemplate
        {
        public:
            u32 id;
            u32 flags;
            
            std::string name;
            std::string subname;

            u32 displayID;
            f32 scale;

            u16 minLevel;
            u16 maxLevel;
            f32 armorMod;
            f32 healthMod;
            f32 resourceMod;
            f32 damageMod;

            std::string scriptName;

        public:
            static bool Read(std::shared_ptr<Bytebuffer>& buffer, CreatureTemplate& result);
            static bool Write(std::shared_ptr<Bytebuffer>& buffer, const CreatureTemplate& data);
        };

        struct Map
        {
        public:
            u32 id;
            u32 flags;

            std::string name;

            u16 type;
            u16 maxPlayers;

        public:
            static bool Read(std::shared_ptr<Bytebuffer>& buffer, Map& result);
            static bool Write(std::shared_ptr<Bytebuffer>& buffer, const Map& data);
        };

        struct MapLocation
        {
        public:
            u32 id;
            std::string name;

            u32 mapID;
            f32 positionX;
            f32 positionY;
            f32 positionZ;
            f32 orientation;

        public:
            static bool Read(std::shared_ptr<Bytebuffer>& buffer, MapLocation& result);
            static bool Write(std::shared_ptr<Bytebuffer>& buffer, const MapLocation& data);
        };

        struct Spell
        {
        public:
            u32 id;

            std::string name;
            std::string description;
            std::string auraDescription;
            u32 iconID;

            f32 castTime;
            f32 cooldown;

        public:
            static bool Read(std::shared_ptr<Bytebuffer>& buffer, Spell& result);
            static bool Write(std::shared_ptr<Bytebuffer>& buffer, const Spell& data);
        };

        struct SpellEffect
        {
        public:
            u32 id;
            u32 spellID;

            u8 effectPriority;
            u8 effectType;

            i32 effectValue1;
            i32 effectValue2;
            i32 effectValue3;

            i32 effectMiscValue1;
            i32 effectMiscValue2;
            i32 effectMiscValue3;

        public:
            static bool Read(std::shared_ptr<Bytebuffer>& buffer, SpellEffect& result);
            static bool Write(std::shared_ptr<Bytebuffer>& buffer, const SpellEffect& data);
        };
    }
}
#pragma once
#include "Base/Types.h"

#include <memory>

class Bytebuffer;

namespace GameDefine
{
    struct ObjectGuid
    {
    public:
        enum class Type : u8
        {
            None,
            Player,
            Creature,
            GameObject,
            Item,
            Count
        };

        ObjectGuid() : _data(0) { }
        ObjectGuid(Type type, u64 counter)
        {
            _data = (static_cast<u64>(type) << 59) | (counter & COUNTER_MASK);
        }
        static ObjectGuid FromU64(u64 rawValue);
        static ObjectGuid CreatePlayer(u64 counter);
        static ObjectGuid CreateCreature(u64 counter);
        static ObjectGuid CreateGameObject(u64 counter);
        static ObjectGuid CreateItem(u64 counter);

        Type GetType() const { return static_cast<Type>(_data >> 59); }
        u64 GetCounter() const { return _data & COUNTER_MASK; }
        u64 GetData() const { return _data; }
        bool IsValid() const { return GetType() != Type::None; }
        bool IsSame(const ObjectGuid& other) const { return GetType() == other.GetType(); }

        /**
        * Calculates the minimum number of bytes needed to represent a counter value.
        * The function determines this by finding the position of the highest set bit,
        * effectively finding the smallest number of bytes that can contain the value.
        *
        * @return Number of bytes (0-7) needed to represent the counter value:
        *         - Returns 0 if counter is 0
        *         - Returns 1-7 based on minimum bytes needed
        */
        u8 GetCounterBytesUsed() const;

    public:
        static const ObjectGuid Empty;
        static constexpr u64 TYPE_MASK = 0xF800000000000000;
        static constexpr u64 COUNTER_MASK = 0x07FFFFFFFFFFFFFF;

        std::strong_ordering operator<=>(const ObjectGuid& other) const
        {
            return _data <=> other._data;
        }

        // Then, the other comparison operators can be very simply defined:
        bool operator==(const ObjectGuid& other) const { return (*this <=> other) == std::strong_ordering::equal; }
        bool operator!=(const ObjectGuid& other) const { return !(*this == other); }
        bool operator<(const ObjectGuid& other) const { return (*this <=> other) == std::strong_ordering::less; }
        bool operator>(const ObjectGuid& other) const { return (*this <=> other) == std::strong_ordering::greater; }
        bool operator<=(const ObjectGuid& other) const { return (*this <=> other) != std::strong_ordering::greater; }
        bool operator>=(const ObjectGuid& other) const { return (*this <=> other) != std::strong_ordering::less; }

        std::string ToString() const;
        static std::string TypeToString(Type type);

        bool Deserialize(Bytebuffer* buffer);
        bool Serialize(Bytebuffer* buffer) const;

    private:
        ObjectGuid(u64 data);

    private:
        u64 _data;

        static_assert(TYPE_MASK == 0xF800000000000000, "TYPE_MASK is incorrect");
        static_assert(COUNTER_MASK == 0x07FFFFFFFFFFFFFF, "COUNTER_MASK is incorrect");
    };

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
    }
}

namespace std // You can specialize std::hash in the std namespace
{
    template <>
    struct hash<GameDefine::ObjectGuid>
    {
        size_t operator()(const GameDefine::ObjectGuid& og) const
        {
            return std::hash<u64>()(og.GetData());
        }
    };
}
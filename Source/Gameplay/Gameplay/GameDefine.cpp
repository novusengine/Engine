#include "GameDefine.h"

#include <Base/Memory/Bytebuffer.h>
#include <Base/Util/DebugHandler.h>

#include <string>
#include <memory>

namespace GameDefine
{
    const ObjectGuid ObjectGuid::Empty;

    ObjectGuid::ObjectGuid(u64 data) : _data(data)
    {
        Type extractedType = GetType();
        if (extractedType < Type::None || extractedType >= Type::Count)
        {
#ifdef NC_DEBUG
            NC_ASSERT(false, "ObjectGuid created with out-of-range Type value!");
#endif
            _data = 0;
        }
    }

    ObjectGuid ObjectGuid::FromU64(u64 rawValue)
    {
        return ObjectGuid(rawValue);
    }

    ObjectGuid ObjectGuid::CreatePlayer(u64 counter)
    {
        return ObjectGuid(Type::Player, counter);
    }

    ObjectGuid ObjectGuid::CreateCreature(u64 counter)
    {
        return ObjectGuid(Type::Creature, counter);
    }

    ObjectGuid ObjectGuid::CreateGameObject(u64 counter)
    {
        return ObjectGuid(Type::GameObject, counter);
    }

    ObjectGuid ObjectGuid::CreateItem(u64 counter)
    {
        return ObjectGuid(Type::Item, counter);
    }

    u8 ObjectGuid::GetCounterBytesUsed() const
    {
        u64 counter = GetCounter();
        if (counter == 0) return 0;

#if defined(_MSC_VER)
        // MSVC: Use built-in bit scan reverse intrinsic
        unsigned long index;
        _BitScanReverse64(&index, counter);

        // Convert bit position to bytes (add 8 since index is 0-based)
        return static_cast<u8>((index + 8) >> 3);
#elif defined(__GNUC__) || defined(__clang__)
        // GCC/Clang: Use built-in leading zero count intrinsic
        // 64 - leading zeros = position of highest set bit
        // Add 7 and divide by 8 to get ceiling of bytes needed
        return static_cast<u8>((64 - __builtin_clzll(counter) + 7) >> 3);
#else
        // Fallback: Use binary search to find highest set bit
        // This approach progressively checks larger ranges and
        // accumulates the byte count as it narrows down the value

        u8 bytes = 1;

        // Check if value exceeds 32 bits (4 bytes)
        if (counter > 0xFFFFFFFF) { counter >>= 32; bytes += 4; }

        // Check if remaining value exceeds 16 bits (2 bytes)
        if (counter > 0xFFFF) { counter >>= 16; bytes += 2; }

        // Check if remaining value exceeds 8 bits (1 byte)
        if (counter > 0xFF) { counter >>= 8;  bytes += 1; }

        return bytes;
#endif
    }

    std::string ObjectGuid::ToString() const
    {
        std::string objectAsStr = "ObjectGuid (" + TypeToString(GetType()) + ", " + std::to_string(GetCounter()) + ")";
        return objectAsStr;
    }

    std::string ObjectGuid::TypeToString(Type type)
    {
        switch (type)
        {
            case Type::None:       return "None";
            case Type::Player:     return "Player";
            case Type::Creature:   return "Creature";
            case Type::GameObject: return "GameObject";
            case Type::Item:       return "Item";
            default:               return "Unknown"; // Handle unexpected type values
        }
    }

    bool ObjectGuid::Deserialize(Bytebuffer* buffer)
    {
        u8 packedData = 0;
        u64 counter = 0;

        bool didFail = false;
        didFail |= !buffer->GetU8(packedData);

        u8 numBytes = packedData & 0x7;
        didFail |= !buffer->GetBytes(&counter, numBytes);

        bool succeeded = !didFail;

        // Branchless Trick to ensure ObjectGuid is empty if either read fails
        // If the first read fails, the resulting ObjectGuid will be empty
        // If the first read succeeds, but the second one fails, the resulting ObjectGuid will also be empty
        packedData *= succeeded;

        ObjectGuid::Type type = static_cast<ObjectGuid::Type>(packedData >> 3);
        *this = ObjectGuid(type, counter);

        return succeeded;
    }

    bool ObjectGuid::Serialize(Bytebuffer* buffer) const
    {
        u8 numBytes = GetCounterBytesUsed() & 0x7;
        u8 type = static_cast<u8>(GetType());
        u64 counter = GetCounter();

        u8 packedData = (type << 3) | numBytes;

        bool didFail = false;
        didFail |= !buffer->PutU8(packedData);
        didFail |= !buffer->PutBytes(&counter, numBytes);

        bool succeeded = !didFail;
        return succeeded;
    }

    namespace Database
    {
        bool ItemTemplate::Read(std::shared_ptr<Bytebuffer>& buffer, ItemTemplate& result)
        {
            bool didFail = false;

            didFail |= !buffer->GetU32(result.id);
            didFail |= !buffer->GetU32(result.displayID);
            didFail |= !buffer->GetU8(result.bind);
            didFail |= !buffer->GetU8(result.rarity);
            didFail |= !buffer->GetU8(result.category);
            didFail |= !buffer->GetU8(result.type);
            didFail |= !buffer->GetU16(result.virtualLevel);
            didFail |= !buffer->GetU16(result.requiredLevel);
            didFail |= !buffer->GetU32(result.durability);
            didFail |= !buffer->GetU32(result.iconID);

            didFail |= !buffer->GetString(result.name);
            didFail |= !buffer->GetString(result.description);

            didFail |= !buffer->GetU32(result.armor);
            didFail |= !buffer->GetU32(result.statTemplateID);
            didFail |= !buffer->GetU32(result.armorTemplateID);
            didFail |= !buffer->GetU32(result.weaponTemplateID);
            didFail |= !buffer->GetU32(result.shieldTemplateID);

            bool succeeded = !didFail;
            return succeeded;
        }
        bool ItemTemplate::Write(std::shared_ptr<Bytebuffer>& buffer, const ItemTemplate& data)
        {
            bool didFail = false;

            didFail |= !buffer->PutU32(data.id);
            didFail |= !buffer->PutU32(data.displayID);
            didFail |= !buffer->PutU8(data.bind);
            didFail |= !buffer->PutU8(data.rarity);
            didFail |= !buffer->PutU8(data.category);
            didFail |= !buffer->PutU8(data.type);
            didFail |= !buffer->PutU16(data.virtualLevel);
            didFail |= !buffer->PutU16(data.requiredLevel);
            didFail |= !buffer->PutU32(data.durability);
            didFail |= !buffer->PutU32(data.iconID);

            didFail |= !buffer->PutString(data.name);
            didFail |= !buffer->PutString(data.description);

            didFail |= !buffer->PutU32(data.armor);
            didFail |= !buffer->PutU32(data.statTemplateID);
            didFail |= !buffer->PutU32(data.armorTemplateID);
            didFail |= !buffer->PutU32(data.weaponTemplateID);
            didFail |= !buffer->PutU32(data.shieldTemplateID);

            bool succeeded = !didFail;
            return succeeded;
        }

        bool ItemStatTemplate::Read(std::shared_ptr<Bytebuffer>& buffer, ItemStatTemplate& result)
        {
            bool succeeded = buffer->Get(result);
            return succeeded;
        }
        bool ItemStatTemplate::Write(std::shared_ptr<Bytebuffer>& buffer, const ItemStatTemplate& data)
        {
            bool succeeded = buffer->Put(data);
            return succeeded;
        }

        bool ItemArmorTemplate::Read(std::shared_ptr<Bytebuffer>& buffer, ItemArmorTemplate& result)
        {
            bool succeeded = buffer->Get(result);
            return succeeded;
        }
        bool ItemArmorTemplate::Write(std::shared_ptr<Bytebuffer>& buffer, const ItemArmorTemplate& data)
        {
            bool succeeded = buffer->Put(data);
            return succeeded;
        }

        bool ItemWeaponTemplate::Read(std::shared_ptr<Bytebuffer>& buffer, ItemWeaponTemplate& result)
        {
            bool succeeded = buffer->Get(result);
            return succeeded;
        }
        bool ItemWeaponTemplate::Write(std::shared_ptr<Bytebuffer>& buffer, const ItemWeaponTemplate& data)
        {
            bool succeeded = buffer->Put(data);
            return succeeded;
        }

        bool ItemShieldTemplate::Read(std::shared_ptr<Bytebuffer>& buffer, ItemShieldTemplate& result)
        {
            bool succeeded = buffer->Get(result);
            return succeeded;
        }
        bool ItemShieldTemplate::Write(std::shared_ptr<Bytebuffer>& buffer, const ItemShieldTemplate& data)
        {
            bool succeeded = buffer->Put(data);
            return succeeded;
        }
    }
}
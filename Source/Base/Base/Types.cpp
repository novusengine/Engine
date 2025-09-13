#include "Types.h"

#include "Memory/Bytebuffer.h"
#include "Util/DebugHandler.h"

const ObjectGUID ObjectGUID::Empty;

ObjectGUID::ObjectGUID(u64 data) : _data(data)
{
    Type extractedType = GetType();
    if (extractedType < Type::None || extractedType >= Type::Count)
    {
#ifdef NC_DEBUG
        NC_ASSERT(false, "ObjectGUID created with out-of-range Type value!");
#endif
        _data = 0;
    }
}

ObjectGUID ObjectGUID::FromU64(u64 rawValue)
{
    return ObjectGUID(rawValue);
}

ObjectGUID ObjectGUID::CreatePlayer(u64 counter)
{
    return ObjectGUID(Type::Player, counter);
}

ObjectGUID ObjectGUID::CreateCreature(u64 counter)
{
    return ObjectGUID(Type::Creature, counter);
}

ObjectGUID ObjectGUID::CreateGameObject(u64 counter)
{
    return ObjectGUID(Type::GameObject, counter);
}

ObjectGUID ObjectGUID::CreateItem(u64 counter)
{
    return ObjectGUID(Type::Item, counter);
}

u8 ObjectGUID::GetCounterBytesUsed() const
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

std::string ObjectGUID::ToString() const
{
    std::string objectAsStr = "ObjectGUID (" + TypeToString(GetType()) + ", " + std::to_string(GetCounter()) + ")";
    return objectAsStr;
}

std::string ObjectGUID::TypeToString(Type type)
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

bool ObjectGUID::Deserialize(Bytebuffer* buffer)
{
    u8 packedData = 0;
    u64 counter = 0;

    bool didFail = false;
    didFail |= !buffer->GetU8(packedData);

    u8 numBytes = packedData & 0x7;
    didFail |= !buffer->GetBytes(&counter, numBytes);

    bool succeeded = !didFail;

    // Branchless Trick to ensure ObjectGUID is empty if either read fails
    // If the first read fails, the resulting ObjectGUID will be empty
    // If the first read succeeds, but the second one fails, the resulting ObjectGUID will also be empty
    packedData *= succeeded;

    ObjectGUID::Type type = static_cast<ObjectGUID::Type>(packedData >> 3);
    *this = ObjectGUID(type, counter);

    return succeeded;
}

bool ObjectGUID::Serialize(Bytebuffer* buffer) const
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
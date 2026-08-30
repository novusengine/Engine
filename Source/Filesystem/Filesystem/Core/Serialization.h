#pragma once

#include <Base/Memory/Bytebuffer.h>
#include <Base/Types.h>

namespace PACT::Serialization
{
    inline bool WriteU32(Bytebuffer* buffer, u32 value)
    {
        const u8 bytes[4] =
        {
            static_cast<u8>(value),
            static_cast<u8>(value >> 8),
            static_cast<u8>(value >> 16),
            static_cast<u8>(value >> 24)
        };
        return buffer->PutBytes(bytes, sizeof(bytes));
    }

    inline bool WriteU64(Bytebuffer* buffer, u64 value)
    {
        const u8 bytes[8] =
        {
            static_cast<u8>(value),
            static_cast<u8>(value >> 8),
            static_cast<u8>(value >> 16),
            static_cast<u8>(value >> 24),
            static_cast<u8>(value >> 32),
            static_cast<u8>(value >> 40),
            static_cast<u8>(value >> 48),
            static_cast<u8>(value >> 56)
        };
        return buffer->PutBytes(bytes, sizeof(bytes));
    }

    inline bool ReadU32(Bytebuffer* buffer, u32& value)
    {
        u8 bytes[4];
        if (!buffer->GetBytes(bytes, sizeof(bytes)))
            return false;

        value = static_cast<u32>(bytes[0]) |
            (static_cast<u32>(bytes[1]) << 8) |
            (static_cast<u32>(bytes[2]) << 16) |
            (static_cast<u32>(bytes[3]) << 24);
        return true;
    }

    inline bool ReadU64(Bytebuffer* buffer, u64& value)
    {
        u8 bytes[8];
        if (!buffer->GetBytes(bytes, sizeof(bytes)))
            return false;

        value = static_cast<u64>(bytes[0]) |
            (static_cast<u64>(bytes[1]) << 8) |
            (static_cast<u64>(bytes[2]) << 16) |
            (static_cast<u64>(bytes[3]) << 24) |
            (static_cast<u64>(bytes[4]) << 32) |
            (static_cast<u64>(bytes[5]) << 40) |
            (static_cast<u64>(bytes[6]) << 48) |
            (static_cast<u64>(bytes[7]) << 56);
        return true;
    }
}

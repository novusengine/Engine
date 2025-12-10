#pragma once
#include "Gameplay/ECS/Components/Events.h"

#include <Base/Types.h>
#include <Base/Memory/Bytebuffer.h>
#include <Base/Util/DebugHandler.h>

#include <entt/entt.hpp>
#include <robinhood/robinhood.h>

#include <bit>

namespace Network
{
    template <typename T>
    concept HasMetaEnumTraits = requires
    {
        typename EnumTraits<T>::Meta;
    };

    template <typename TEnum> requires std::is_enum_v<TEnum>&& std::is_same_v<std::underlying_type_t<TEnum>, u16>&& HasMetaEnumTraits<TEnum>
    using NetFieldListenerCallback = std::function<void(entt::entity, ObjectGUID, TEnum)>;

    template <typename TEnum> requires std::is_enum_v<TEnum> && std::is_same_v<std::underlying_type_t<TEnum>, u16> && HasMetaEnumTraits<TEnum>
    struct NetFields
    {
        using Meta = typename EnumTraits<TEnum>::Meta;
        static constexpr u16 NUM_FIELDS = static_cast<u16>(Meta::ENUM_FIELD_LIST.size());

    public:
        static_assert(NUM_FIELDS > 0, "NUM_FIELDS must be greater than zero.");
        
        template <typename T> requires std::is_trivially_copyable_v<std::decay_t<T>>
        void SetField(TEnum startField, T&& value, u8 fieldByteOffset = 0, u8 fieldBitOffset = 0, u8 bitCount = sizeof(std::decay_t<T>) * 8)
        {
            using DecayedT = std::decay_t<T>;
            constexpr u16 typeSizeInBytes = sizeof(T);
            constexpr u16 fieldSize = sizeof(u32);
            constexpr u16 fieldBits = fieldSize * 8;

            // Disallow byte/bit offsets for full-field or multi-field writes
            if constexpr (sizeof(DecayedT) >= fieldSize)
            {
                NC_ASSERT(fieldByteOffset == 0, "Byte offset must be zero for full-field or multi-field writes.");
                NC_ASSERT(fieldBitOffset == 0, "Bit offset must be zero for full-field or multi-field writes.");
                NC_ASSERT(bitCount == sizeof(DecayedT) * 8, "Bit count override not allowed for full-field writes.");
            }
            else
            {
                const u16 bitsAvailable = fieldBits - (fieldByteOffset * 8);
                NC_ASSERT(fieldBitOffset + bitCount <= bitsAvailable, "Byte + bit offset combination exceeds single 4-byte field capacity.");
            }

            u16 startFieldIndex = static_cast<u16>(startField);
            constexpr u16 numFieldsTouched = (sizeof(DecayedT) + fieldSize - 1) / fieldSize;
            NC_ASSERT(startFieldIndex + numFieldsTouched <= NUM_FIELDS, "Attempting to Set NetField out of bounds");

            std::byte* base = reinterpret_cast<std::byte*>(fields);
            std::byte* dest = base + (startFieldIndex * fieldSize) + fieldByteOffset;

            if constexpr (sizeof(DecayedT) >= fieldSize)
            {
                std::memcpy(dest, &value, sizeof(DecayedT));
            }
            else if (bitCount == sizeof(DecayedT) * 8 && fieldBitOffset == 0)
            {
                // Normal partial-byte write
                std::memcpy(dest, &value, sizeof(DecayedT));
            }
            else
            {
                // Sub-byte bitfield write
                u32* fieldPtr = reinterpret_cast<u32*>(dest - fieldByteOffset);
                const u8 totalBitOffset = (fieldByteOffset * 8u) + fieldBitOffset;

                const uint64_t bitMask64 = ((1ull << bitCount) - 1ull) << totalBitOffset;
                const u32 mask = static_cast<u32>(bitMask64);

                u32 raw = 0;
                std::memcpy(&raw, &value, sizeof(DecayedT));

                const u32 valueMask = (bitCount == 32) ? 0xFFFFFFFFu : ((1u << bitCount) - 1u);
                raw &= valueMask;

                const u32 newBits = (raw << totalBitOffset) & mask;

                const u32 fieldValue = *fieldPtr;
                *fieldPtr = (fieldValue & ~mask) | newBits;
            }

            SetFieldsDirty(startField, numFieldsTouched);
        }

        template <typename T>
        T GetField(TEnum startField, u8 fieldByteOffset = 0, u8 fieldBitOffset = 0, u8 bitCount = sizeof(T) * 8) const
        {
            constexpr u16 fieldSize = sizeof(u32);
            constexpr u16 typeSize = sizeof(T);
            constexpr u16 fieldBits = fieldSize * 8;

            const u16 startFieldIndex = static_cast<u16>(startField);
            NC_ASSERT(startFieldIndex < NUM_FIELDS, "Start field index out of bounds.");

            const std::byte* base = reinterpret_cast<const std::byte*>(fields);
            const std::byte* src = base + (startFieldIndex * fieldSize) + fieldByteOffset;

            // Same safety checks as in SetField
            if constexpr (typeSize >= fieldSize)
            {
                // Multi-field or full-field read
                NC_ASSERT(fieldByteOffset == 0 && fieldBitOffset == 0, "Byte/Bit offsets only allowed for partial reads.");
                NC_ASSERT(startFieldIndex + ((typeSize + fieldSize - 1) / fieldSize) <= NUM_FIELDS, "Read exceeds field range.");

                T result;
                std::memcpy(&result, src, sizeof(T));
                return result;
            }
            else
            {
                // Partial read within a single field
                const u16 bitsAvailable = fieldBits - (fieldByteOffset * 8);
                NC_ASSERT(fieldBitOffset + bitCount <= bitsAvailable, "Byte + bit offset combination exceeds single 4-byte field capacity.");

                const u32* fieldPtr = reinterpret_cast<const u32*>(src - fieldByteOffset);
                const u32 fieldValue = *fieldPtr;
                const u8 totalBitOffset = (fieldByteOffset * 8u) + fieldBitOffset;

                const u64 mask64 = ((bitCount == 32) ? 0xFFFFFFFFull : ((1ull << bitCount) - 1ull)) << totalBitOffset;
                const u32 mask = static_cast<u32>(mask64);
                const u32 extractedBits = (fieldValue & mask) >> totalBitOffset;

                if constexpr (std::is_floating_point_v<T>)
                {
                    static_assert(sizeof(T) <= 4, "Float/double reads must not exceed one field width.");
                    u32 tmp = extractedBits;
                    return std::bit_cast<T>(tmp);
                }
                else
                {
                    return static_cast<T>(extractedBits);
                }
            }
        }

        template <typename T>
        T* GetFieldPtr(TEnum startField)
        {
            u16 startFieldIndex = static_cast<u16>(startField);
            NC_ASSERT(startFieldIndex < NUM_FIELDS, "Start field index out of bounds.");

            T* ptr = reinterpret_cast<T*>(&fields[startFieldIndex]);
            return ptr;
        }

        bool SerializeSetFields(Bytebuffer* buffer)
        {
            constexpr u16 NumMaskQwords = (FIELD_MASK_SIZE + 7) / 8;

            const u64* mask64 = reinterpret_cast<const u64*>(setFieldMask);
            const u16 lastQwordIndex = NumMaskQwords - 1;

            // Find first and last 64-bit mask with any set bit
            u16 firstQword = NumMaskQwords;
            u16 lastQword = 0;
            for (u16 i = 0; i < NumMaskQwords; ++i)
            {
                if (!mask64[i])
                    continue;

                if (firstQword == NumMaskQwords)
                    firstQword = i;

                lastQword = i;
            }

            if (firstQword == NumMaskQwords)
            {
                return false;
            }

            const u16 startByte = firstQword * 8;
            const u16 endByte = (lastQword + 1) * 8;
            const u16 maskCount = (endByte > FIELD_MASK_SIZE ? FIELD_MASK_SIZE : endByte) - startByte;

            // Header: start byte | number of mask bytes written
            buffer->PutU8(static_cast<u8>(startByte));
            buffer->PutU8(static_cast<u8>(maskCount));

            // Write only the relevant set mask bytes
            buffer->PutBytes(&setFieldMask[startByte], maskCount);

            // Process 64-bit chunks for set bits
            const u16 startBlock = startByte * 8;
            const u16 endBlock = endByte * 8;
            const u16 limit = (endBlock > NUM_FIELDS) ? NUM_FIELDS : endBlock;

            for (u16 q = firstQword; q <= lastQword; ++q)
            {
                u64 mask = mask64[q];
                if (!mask)
                    continue;

                const u16 baseBlock = q * 64;

                // Iterate through all set bits
                while (mask)
                {
                    u16 bitIndex = static_cast<u16>(std::countr_zero(mask));
                    mask &= (mask - 1);

                    const u16 fieldIndex = baseBlock + bitIndex;
                    if (fieldIndex >= limit)
                        break;

                    buffer->PutU32(fields[fieldIndex]);
                }
            }

            return true;
        }

        bool SerializeDirtyFields(Bytebuffer* buffer)
        {
            if (!isDirty)
                return false;

            constexpr u16 NumMaskQwords = (FIELD_MASK_SIZE + 7) / 8;

            const u64* mask64 = reinterpret_cast<const u64*>(dirtyFieldMask);
            const u16 lastQwordIndex = NumMaskQwords - 1;

            // Find first and last 64-bit mask with any dirty bit
            u16 firstQword = NumMaskQwords;
            u16 lastQword = 0;
            for (u16 i = 0; i < NumMaskQwords; ++i)
            {
                if (!mask64[i])
                    continue;

                if (firstQword == NumMaskQwords)
                    firstQword = i;

                lastQword = i;
            }

            if (firstQword == NumMaskQwords)
            {
                ClearDirtyMask();
                return false;
            }

            const u16 startByte = firstQword * 8;
            const u16 endByte = (lastQword + 1) * 8;
            const u16 maskCount = (endByte > FIELD_MASK_SIZE ? FIELD_MASK_SIZE : endByte) - startByte;

            // Header: start byte | number of mask bytes written
            buffer->PutU8(static_cast<u8>(startByte));
            buffer->PutU8(static_cast<u8>(maskCount));

            // Write only the relevant dirty mask bytes
            buffer->PutBytes(&dirtyFieldMask[startByte], maskCount);

            // Process 64-bit chunks for dirty bits
            const u16 startBlock = startByte * 8;
            const u16 endBlock = endByte * 8;
            const u16 limit = (endBlock > NUM_FIELDS) ? NUM_FIELDS : endBlock;

            for (u16 q = firstQword; q <= lastQword; ++q)
            {
                u64 mask = mask64[q];
                if (!mask)
                    continue;

                const u16 baseBlock = q * 64;

                // Iterate through all set bits
                while (mask)
                {
                    u16 bitIndex = static_cast<u16>(std::countr_zero(mask));
                    mask &= (mask - 1);

                    const u16 fieldIndex = baseBlock + bitIndex;
                    if (fieldIndex >= limit)
                        break;

                    buffer->PutU32(fields[fieldIndex]);
                }
            }

            ClearDirtyMask();
            return true;
        }

        void SetFieldsDirty(TEnum startField, u16 numFields = 1)
        {
            u16 startFieldIndex = static_cast<u16>(startField);
            if (startFieldIndex + numFields > NUM_FIELDS)
                return;

            const u16 startByte = startFieldIndex / 8;
            const u16 endByte = (startFieldIndex + numFields - 1) / 8;

            const u8 startBit = startFieldIndex % 8;
            const u8 endBit = (startFieldIndex + numFields - 1) % 8;

            if (startByte == endByte)
            {
                const u8 mask = ((0xFFu >> (7 - endBit)) & (0xFFu << startBit));
                setFieldMask[startByte] |= mask;
                dirtyFieldMask[startByte] |= mask;
            }
            else
            {
                setFieldMask[startByte] |= (0xFFu << startBit);
                dirtyFieldMask[startByte] |= (0xFFu << startBit);

                // Bytes Inbetween can be set to all on
                if (endByte > startByte + 1)
                {
                    memset(&setFieldMask[startByte + 1], 0xFF, endByte - startByte - 1);
                    memset(&dirtyFieldMask[startByte + 1], 0xFF, endByte - startByte - 1);
                }

                setFieldMask[endByte] |= (0xFFu >> (7 - endBit));
                dirtyFieldMask[endByte] |= (0xFFu >> (7 - endBit));
            }

            isDirty = true;
        }

        bool IsDirty() const
        {
            return isDirty;
        }
        void ClearDirtyMask()
        {
            memset(dirtyFieldMask, 0, sizeof(dirtyFieldMask));
            isDirty = false;
        }

    public:
        static constexpr u16 FIELD_MASK_SIZE = (NUM_FIELDS + 7) / 8;

        // Fields are stored as a 4 byte integer array, however dirtymask is stored as a byte array where one bit refers to 1 field
        u32 fields[NUM_FIELDS];
        u8 setFieldMask[FIELD_MASK_SIZE];
        u8 dirtyFieldMask[FIELD_MASK_SIZE];
        bool isDirty = false;
    };

    template <typename TEnum> requires std::is_enum_v<TEnum>&& std::is_same_v<std::underlying_type_t<TEnum>, u16>&& HasMetaEnumTraits<TEnum>
    struct NetFieldListener
    {
        using Callback = NetFieldListenerCallback<TEnum>;

    public:
        void RegisterFieldListener(TEnum field, Callback callback)
        {
            _fieldListeners[field] = callback;
        }
        void UnregisterFieldListener(TEnum field)
        {
            _fieldListeners.erase(field);
        }
        void ClearAllFieldListeners()
        {
            _fieldListeners.clear();
        }

        void NotifyFieldChanged(entt::entity entity, ObjectGUID objectGUID, TEnum field)
        {
            auto it = _fieldListeners.find(field);
            if (it == _fieldListeners.end())
                return;

            it->second(entity, objectGUID, field);
        }

    private:
        robin_hood::unordered_map<TEnum, Callback> _fieldListeners;
    };
}

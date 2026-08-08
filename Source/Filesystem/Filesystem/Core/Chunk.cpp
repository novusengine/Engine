#include "Chunk.h"

#include <libsodium/core/crypto_hash_sha256.h>

#include <algorithm>
#include <array>
#include <limits>

namespace PACT
{
    namespace
    {
        const std::array<u32, 256>& GetGearTable()
        {
            static const std::array<u32, 256> gearTable = []
            {
                std::array<u32, 256> table;
                u32 seed = 1;
                for (u32& value : table)
                {
                    seed = seed * 1103515245u + 12345u;
                    value = seed;
                }
                return table;
            }();
            return gearTable;
        }

        u32 MakeMask(u32 bits)
        {
            if (bits >= 32)
                return std::numeric_limits<u32>::max();

            return (1u << bits) - 1u;
        }
    }

    std::string PactDigestToHex(const PactDigest& digest)
    {
        static constexpr char HEX_DIGITS[] = "0123456789abcdef";

        std::string result;
        result.resize(digest.size() * 2);
        for (size_t i = 0; i < digest.size(); i++)
        {
            result[i * 2] = HEX_DIGITS[digest[i] >> 4];
            result[i * 2 + 1] = HEX_DIGITS[digest[i] & 0xFu];
        }

        return result;
    }

    bool PactChunker::GetFastCDCChunkSize(const u8* data, size_t length, u32 minSize, u32 avgSize, u32 maxSize, u32& outChunkSize)
    {
        outChunkSize = 0;
        if (length == 0 || data == nullptr || minSize == 0 || minSize > avgSize || avgSize > maxSize)
            return false;

        const size_t availableLength = std::min<size_t>(length, maxSize);
        if (availableLength > std::numeric_limits<u32>::max())
            return false;

        u32 averageBits = 0;
        u64 averagePowerOfTwo = 1;
        while (averagePowerOfTwo < avgSize)
        {
            averagePowerOfTwo <<= 1;
            averageBits++;
        }

        const u32 strictMask = MakeMask(std::min(averageBits + 1, 32u));
        const u32 relaxedMask = MakeMask(averageBits > 1 ? averageBits - 1 : 1);
        const std::array<u32, 256>& gearTable = GetGearTable();

        size_t chunkLength = availableLength;
        if (availableLength > minSize)
        {
            const size_t normalizationPoint = std::min<size_t>(availableLength, avgSize);
            u32 hash = 0;

            size_t position = minSize;
            for (; position < availableLength; position++)
            {
                hash = (hash << 1) + gearTable[data[position]];
                const u32 mask = position < normalizationPoint ? strictMask : relaxedMask;
                if ((hash & mask) == 0)
                {
                    chunkLength = position + 1;
                    break;
                }
            }
        }

        outChunkSize = static_cast<u32>(chunkLength);
        return true;
    }

    bool PactChunker::SplitFastCDC(const u8* data, size_t length, u32 minSize, u32 avgSize, u32 maxSize, std::vector<PactChunkInfo>& chunks, u32& outChunkCount)
    {
        outChunkCount = 0;
        if (length == 0)
            return true;

        size_t offset = 0;
        while (offset < length)
        {
            u32 chunkLength = 0;
            if (!GetFastCDCChunkSize(data + offset, length - offset, minSize, avgSize, maxSize, chunkLength))
                return false;

            PactChunkInfo& chunk = chunks.emplace_back();
            chunk.size = chunkLength;
            chunk.storedSize = chunk.size;
            chunk.flags = 0;
            crypto_hash_sha256(chunk.digest.data(), data + offset, static_cast<unsigned long long>(chunkLength));

            offset += chunkLength;
            outChunkCount++;
        }

        return true;
    }
}

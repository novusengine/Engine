#pragma once

#include <Base/Types.h>

#include <array>
#include <string>
#include <vector>

namespace PACT
{
    using PactDigest = std::array<u8, 32>;

    std::string PactDigestToHex(const PactDigest& digest);

    struct PactChunkInfo
    {
    public:
        PactDigest digest = {}; // SHA-256 of stored chunk bytes; this is the OCI blob digest.
        u32 size = 0;           // Plaintext size.
        u32 storedSize = 0;     // Stored size after compression and encryption.
        u32 flags = 0;          // Reserved; PACT v2 requires zero.
    };

    class PactChunker
    {
    public:
        // PACT FastCDC v1 uses the deterministic Gear table and boundary rules
        // implemented here. Publishers must use this implementation for cdcAlgo 0.
        static bool GetFastCDCChunkSize(const u8* data, size_t length, u32 minSize, u32 avgSize, u32 maxSize, u32& outChunkSize);
        static bool SplitFastCDC(const u8* data, size_t length, u32 minSize, u32 avgSize, u32 maxSize, std::vector<PactChunkInfo>& chunks, u32& outChunkCount);
    };
}

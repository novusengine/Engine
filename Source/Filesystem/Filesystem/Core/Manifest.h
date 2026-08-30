#pragma once
#include "Filesystem/Core/Chunk.h"
#include "Filesystem/Core/File.h"
#include "Filesystem/Core/Origin.h"

#include <Base/Types.h>
#include <Base/Container/StringTable.h>

#include <robinhood/robinhood.h>

#include <filesystem>
#include <limits>
#include <random>

class Bytebuffer;

namespace PACT
{
    using PactManifestHandle = u64;
    static constexpr PactManifestHandle MANIFEST_INVALID_ID = std::numeric_limits<PactManifestHandle>().max();

    // Runtime-only IDs for loose overlays. Persisted publishers assign their
    // manifest IDs without using this generator.
    class PactManifestHandleGenerator
    {
    public:
        PactManifestHandleGenerator() : _rng(_rd()) {}

        PactManifestHandle Generate()
        {
            return _dist(_rng);
        }

    private:
        std::random_device _rd;
        std::mt19937_64 _rng;
        std::uniform_int_distribution<PactManifestHandle> _dist;
    };

    enum class PactSourceType : u8
    {
        Pack,
        Loose
    };

    struct ManifestHeader
    {
    public:
        struct Flags
        {
            u32 isOverlay : 1;
            u32 storageMask : 3; // Dev, Server, Client
            u32 : 28;
        };

    public:
        char magic[4] = { 'P', 'A', 'M', 'F' };
        uvec3 version = {};

        PactManifestHandle manifestID = 0;
        Flags flags = {};
        u32 priority = 0;
        PactSourceType sourceType = PactSourceType::Pack;
    };

    struct ManifestEntry
    {
    public:
        struct Flags
        {
            u32 compressedType : 3;
            u32 isCompressed : 1;
            u32 isEncrypted : 1;
            u32 isPinned : 1;
            u32 isStreamed : 1;

            u32 : 25;
        };

    public:
        PactFileID fileID = 0;
        Flags flags = {};

        u32 pathIndex = 0;
        u64 pathHash = 0;
        u64 dataOffset = 0;
        u32 dataSize = 0;
        u32 chunkIndex = 0;         // First chunk of this file in the chunk table.
        u32 chunkCount = 0;         // Number of consecutive chunks.
        PactDigest contentDigest{}; // SHA-256 of the whole-file plaintext.
    };

    enum class PactManifestValidateResult
    {
        Success,
        InvalidMagic,
        MismatchVersion,
        InvalidHeader,
        InvalidEntries
    };

    struct PactManifest
    {
    public:
        static constexpr size_t SERIALIZED_HEADER_SIZE = 44;
        static constexpr size_t SERIALIZED_CHUNK_SIZE = 44;
        static constexpr size_t SERIALIZED_ENTRY_SIZE = 76;

        ManifestHeader header;
        Novus::Container::StringTableUnsafe stringTable;
        std::vector<PactChunkInfo> chunks;
        std::vector<ManifestEntry> entries;
        PactManifestOrigin origin = PactManifestOrigin::Remote;
        std::filesystem::path path;
        std::filesystem::path dataPath;

    public:
        size_t GetSerializedSize() const;
        bool Serialize(Bytebuffer* buffer) const;
        bool Deserialize(Bytebuffer* buffer);
        PactManifestValidateResult Validate() const;

    public:
        PactManifest() = default;
        PactManifest(PactManifest&& other) noexcept = default;
        PactManifest& operator=(PactManifest&& other) noexcept = default;

        PactManifest(const PactManifest&) = delete;
        PactManifest& operator=(const PactManifest&) = delete;
    };

    struct PactManifestTable
    {
    public:
        robin_hood::unordered_map<PactManifestHandle, PactManifest> handleToManifest;
        robin_hood::unordered_map<std::filesystem::path, PactManifestHandle> overlayPathToHandle;

        PactManifestHandleGenerator handleGenerator;
    };
}

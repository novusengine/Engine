#pragma once
#include "Filesystem/Core/File.h"

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

    class PactManifestHandleGenerator
    {
    public:
        PactManifestHandleGenerator() : _rng(_rd()) { }

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
        uvec3 version;

        u64 manifestID;
        Flags flags;
        u32 priority;
        PactSourceType sourceType;

        u32 entryCount;
        u32 reserved;
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
        PactFileID fileID;
        Flags flags;

        u32 pathIndex;
        u64 pathHash;
        u64 dataOffset;
        u32 dataSize;
        u32 dataCompressedSize;
    };

    enum class PactManifestValidateResult
    {
        Success,
        InvalidMagic,
        MismatchVersion
    };

    struct PactManifest
    {
    public:
        ManifestHeader header;
        Novus::Container::StringTableUnsafe stringTable;
        std::vector<ManifestEntry> entries;
        std::filesystem::path dataPath;

    public:
        bool Serialize(Bytebuffer* buffer) const;
        bool Deserialize(Bytebuffer* buffer);
        PactManifestValidateResult Validate() const;

    public:
        PactManifest& operator=(PactManifest&& other) noexcept;
    };

    struct PactManifestTable
    {
    public:
        robin_hood::unordered_map<PactManifestHandle, PactManifest> handleToManifest;
        robin_hood::unordered_map<std::filesystem::path, PactManifestHandle> overlayPathToHandle;

        PactManifestHandleGenerator handleGenerator;
    };
}
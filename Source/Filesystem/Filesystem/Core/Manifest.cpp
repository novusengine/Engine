#include "Manifest.h"
#include "Filesystem/Config.h"
#include "Filesystem/Core/Serialization.h"

#include <Base/Memory/Bytebuffer.h>

#include <libsodium/core/crypto_hash_sha256.h>
#include <xxhash/xxhash64.h>

#include <algorithm>
#include <limits>
#include <unordered_set>

namespace PACT
{
    namespace
    {
        u32 PackManifestFlags(const ManifestHeader::Flags& flags)
        {
            return flags.isOverlay | (flags.storageMask << 1);
        }

        u32 PackEntryFlags(const ManifestEntry::Flags& flags)
        {
            return flags.compressedType |
                (flags.isCompressed << 3) |
                (flags.isEncrypted << 4) |
                (flags.isPinned << 5) |
                (flags.isStreamed << 6);
        }

        void UnpackManifestFlags(u32 value, ManifestHeader::Flags& flags)
        {
            flags = {};
            flags.isOverlay = value & 0x1u;
            flags.storageMask = (value >> 1) & 0x7u;
        }

        void UnpackEntryFlags(u32 value, ManifestEntry::Flags& flags)
        {
            flags = {};
            flags.compressedType = value & 0x7u;
            flags.isCompressed = (value >> 3) & 0x1u;
            flags.isEncrypted = (value >> 4) & 0x1u;
            flags.isPinned = (value >> 5) & 0x1u;
            flags.isStreamed = (value >> 6) & 0x1u;
        }
    }

    size_t PactManifest::GetSerializedSize() const
    {
        return SERIALIZED_HEADER_SIZE +
            chunks.size() * SERIALIZED_CHUNK_SIZE +
            entries.size() * SERIALIZED_ENTRY_SIZE +
            sizeof(u32) + stringTable.GetNumBytes();
    }

    bool PactManifest::Serialize(Bytebuffer* buffer) const
    {
        if (Validate() != PactManifestValidateResult::Success || chunks.size() > std::numeric_limits<u32>::max() ||
            entries.size() > std::numeric_limits<u32>::max() || stringTable.GetNumBytes() > std::numeric_limits<u32>::max())
        {
            return false;
        }

        bool failed = false;
        failed |= !buffer->PutBytes(header.magic, sizeof(header.magic));
        failed |= !Serialization::WriteU32(buffer, header.version.x);
        failed |= !Serialization::WriteU32(buffer, header.version.y);
        failed |= !Serialization::WriteU32(buffer, header.version.z);
        failed |= !Serialization::WriteU64(buffer, header.manifestID);
        failed |= !Serialization::WriteU32(buffer, PackManifestFlags(header.flags));
        failed |= !Serialization::WriteU32(buffer, header.priority);
        failed |= !Serialization::WriteU32(buffer, static_cast<u32>(header.sourceType));
        failed |= !Serialization::WriteU32(buffer, static_cast<u32>(chunks.size()));
        failed |= !Serialization::WriteU32(buffer, static_cast<u32>(entries.size()));

        for (const PactChunkInfo& chunk : chunks)
        {
            failed |= !buffer->PutBytes(chunk.digest.data(), chunk.digest.size());
            failed |= !Serialization::WriteU32(buffer, chunk.size);
            failed |= !Serialization::WriteU32(buffer, chunk.storedSize);
            failed |= !Serialization::WriteU32(buffer, chunk.flags);
        }

        for (const ManifestEntry& entry : entries)
        {
            failed |= !Serialization::WriteU64(buffer, entry.fileID);
            failed |= !Serialization::WriteU32(buffer, PackEntryFlags(entry.flags));
            failed |= !Serialization::WriteU32(buffer, entry.pathIndex);
            failed |= !Serialization::WriteU64(buffer, entry.pathHash);
            failed |= !Serialization::WriteU64(buffer, entry.dataOffset);
            failed |= !Serialization::WriteU32(buffer, entry.dataSize);
            failed |= !Serialization::WriteU32(buffer, entry.chunkIndex);
            failed |= !Serialization::WriteU32(buffer, entry.chunkCount);
            failed |= !buffer->PutBytes(entry.contentDigest.data(), entry.contentDigest.size());
        }

        failed |= !buffer->Serialize(stringTable);

        return !failed;
    }

    bool PactManifest::Deserialize(Bytebuffer* buffer)
    {
        chunks.clear();
        entries.clear();
        stringTable.Clear();

        u32 manifestFlags = 0;
        u32 sourceType = 0;
        u32 chunkCount = 0;
        u32 entryCount = 0;
        bool failed = false;
        failed |= !buffer->GetBytes(header.magic, sizeof(header.magic));
        failed |= !Serialization::ReadU32(buffer, header.version.x);
        failed |= !Serialization::ReadU32(buffer, header.version.y);
        failed |= !Serialization::ReadU32(buffer, header.version.z);
        failed |= !Serialization::ReadU64(buffer, header.manifestID);
        failed |= !Serialization::ReadU32(buffer, manifestFlags);
        failed |= !Serialization::ReadU32(buffer, header.priority);
        failed |= !Serialization::ReadU32(buffer, sourceType);
        failed |= !Serialization::ReadU32(buffer, chunkCount);
        failed |= !Serialization::ReadU32(buffer, entryCount);
        if (failed || (manifestFlags & ~0xFu) != 0 || sourceType > static_cast<u32>(PactSourceType::Loose))
            return false;

        UnpackManifestFlags(manifestFlags, header.flags);
        header.sourceType = static_cast<PactSourceType>(sourceType);

        size_t remainingBytes = buffer->GetActiveSize();
        if (chunkCount > remainingBytes / SERIALIZED_CHUNK_SIZE)
            return false;

        remainingBytes -= static_cast<size_t>(chunkCount) * SERIALIZED_CHUNK_SIZE;
        if (entryCount > remainingBytes / SERIALIZED_ENTRY_SIZE)
            return false;

        chunks.resize(chunkCount);
        for (PactChunkInfo& chunk : chunks)
        {
            failed |= !buffer->GetBytes(chunk.digest.data(), chunk.digest.size());
            failed |= !Serialization::ReadU32(buffer, chunk.size);
            failed |= !Serialization::ReadU32(buffer, chunk.storedSize);
            failed |= !Serialization::ReadU32(buffer, chunk.flags);
        }

        entries.resize(entryCount);
        for (ManifestEntry& entry : entries)
        {
            u32 entryFlags = 0;
            failed |= !Serialization::ReadU64(buffer, entry.fileID);
            failed |= !Serialization::ReadU32(buffer, entryFlags);
            failed |= !Serialization::ReadU32(buffer, entry.pathIndex);
            failed |= !Serialization::ReadU64(buffer, entry.pathHash);
            failed |= !Serialization::ReadU64(buffer, entry.dataOffset);
            failed |= !Serialization::ReadU32(buffer, entry.dataSize);
            failed |= !Serialization::ReadU32(buffer, entry.chunkIndex);
            failed |= !Serialization::ReadU32(buffer, entry.chunkCount);
            failed |= !buffer->GetBytes(entry.contentDigest.data(), entry.contentDigest.size());
            if ((entryFlags & ~0x7Fu) != 0)
                return false;

            UnpackEntryFlags(entryFlags, entry.flags);
        }

        if (failed || !buffer->Deserialize(stringTable))
            return false;

        return buffer->GetActiveSize() == 0;
    }

    PactManifestValidateResult PactManifest::Validate() const
    {
        const bool invalidMagic = header.magic[0] != 'P' || header.magic[1] != 'A' || header.magic[2] != 'M' || header.magic[3] != 'F';
        if (invalidMagic)
            return PactManifestValidateResult::InvalidMagic;

        if (header.version != Config::MANIFEST_VERSION)
            return PactManifestValidateResult::MismatchVersion;

        if (header.manifestID == MANIFEST_INVALID_ID || header.flags.storageMask > 0x7u ||
            (header.sourceType != PactSourceType::Pack && header.sourceType != PactSourceType::Loose))
        {
            return PactManifestValidateResult::InvalidHeader;
        }

        if (header.sourceType == PactSourceType::Loose && !chunks.empty())
            return PactManifestValidateResult::InvalidHeader;

        PactDigest emptyContentDigest;
        const u8 emptyContent = 0;
        crypto_hash_sha256(emptyContentDigest.data(), &emptyContent, 0);

        std::unordered_set<u64> pathHashes;
        std::unordered_set<PactFileID> fileIDs;
        std::vector<bool> usedChunks(chunks.size(), false);
        for (const ManifestEntry& entry : entries)
        {
            if (entry.pathIndex >= stringTable.GetNumStrings())
                return PactManifestValidateResult::InvalidEntries;

            const std::string& path = stringTable.GetString(entry.pathIndex);
            const std::filesystem::path virtualPath(path);
            if (path.empty() || !virtualPath.has_filename() || virtualPath == "." || virtualPath.is_absolute() || virtualPath.has_root_name() ||
                virtualPath.lexically_normal().generic_string() != path ||
                XXHash64::hash(path.c_str(), path.size(), 0) != entry.pathHash)
            {
                return PactManifestValidateResult::InvalidEntries;
            }

            if (!pathHashes.insert(entry.pathHash).second || (entry.fileID != 0 && !fileIDs.insert(entry.fileID).second))
                return PactManifestValidateResult::InvalidEntries;

            if (entry.chunkIndex > chunks.size() || entry.chunkCount > chunks.size() - entry.chunkIndex)
                return PactManifestValidateResult::InvalidEntries;

            if (entry.flags.compressedType != 0 || entry.flags.isCompressed || entry.flags.isEncrypted)
                return PactManifestValidateResult::InvalidEntries;

            if (header.sourceType == PactSourceType::Loose)
            {
                if (entry.chunkIndex != 0 || entry.chunkCount != 0 || entry.dataOffset != 0)
                    return PactManifestValidateResult::InvalidEntries;
                continue;
            }

            if (entry.dataSize == 0)
            {
                if (entry.chunkIndex != 0 || entry.chunkCount != 0 || entry.contentDigest != emptyContentDigest)
                    return PactManifestValidateResult::InvalidEntries;
                continue;
            }

            if (entry.chunkCount == 0)
                return PactManifestValidateResult::InvalidEntries;

            if (!std::any_of(entry.contentDigest.begin(), entry.contentDigest.end(), [](u8 value) { return value != 0; }))
                return PactManifestValidateResult::InvalidEntries;

            u64 plaintextSize = 0;
            for (u32 i = 0; i < entry.chunkCount; i++)
            {
                const size_t chunkIndex = entry.chunkIndex + i;
                const PactChunkInfo& chunk = chunks[chunkIndex];
                const bool hasDigest = std::any_of(chunk.digest.begin(), chunk.digest.end(), [](u8 value) { return value != 0; });
                if (usedChunks[chunkIndex] || !hasDigest || chunk.size == 0 || chunk.storedSize != chunk.size || chunk.flags != 0)
                    return PactManifestValidateResult::InvalidEntries;

                usedChunks[chunkIndex] = true;
                plaintextSize += chunk.size;
            }

            if (plaintextSize != entry.dataSize)
                return PactManifestValidateResult::InvalidEntries;

        }

        if (header.sourceType == PactSourceType::Pack && std::find(usedChunks.begin(), usedChunks.end(), false) != usedChunks.end())
            return PactManifestValidateResult::InvalidEntries;

        return PactManifestValidateResult::Success;
    }
}

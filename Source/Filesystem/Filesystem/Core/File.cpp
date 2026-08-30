#include "File.h"
#include "Filesystem/Config.h"
#include "Filesystem/PactStorage.h"

#include <Base/Memory/Bytebuffer.h>

#include <libsodium/core/crypto_hash_sha256.h>

#include <filesystem>
#include <fstream>
#include <thread>

namespace fs = std::filesystem;

namespace PACT
{
    bool PactFileKey::operator==(const PactFileKey& other) const
    {
        return type == other.type && value == other.value && generation == other.generation;
    }

    bool PactResidentFile::operator==(const PactResidentFile& other) const
    {
        return data == other.data;
    }

    void PactResidentFile::AddHandle()
    {
        handleCount.fetch_add(1, std::memory_order_relaxed);
    }
    void PactResidentFile::ReleaseHandle()
    {
        const u32 prev = handleCount.fetch_sub(1, std::memory_order_relaxed);
        assert(prev > 0 && "PactResidentFile attempted to ReleaseHandle when no handle was claimed");

        if (prev == 1 && pinCount.load(std::memory_order_relaxed) == 0)
        {
            owner->RequestEviction(*this);
        }
    }

    bool PactResidentFile::IsReady() const
    {
        return loadState.load(std::memory_order_acquire) == PactLoadState::Loaded;
    }

    bool PactResidentFile::IsPinned() const
    {
        return pinCount.load(std::memory_order_relaxed) > 0;
    }
    void PactResidentFile::AddPin()
    {
        pinCount.fetch_add(1, std::memory_order_relaxed);
    }
    void PactResidentFile::RemovePin()
    {
        const u32 prev = pinCount.fetch_sub(1, std::memory_order_relaxed);
        assert(prev > 0 && "PactResidentFile attempted to RemovePin when no Pin was claimed");

        if (prev == 1 && handleCount.load(std::memory_order_relaxed) == 0)
        {
            owner->RequestEviction(*this);
        }
    }

    const void* PactResidentFile::GetData() const
    {
        return data;
    }
    const size_t PactResidentFile::GetSize() const
    {
        return size;
    }

    PactReadResult PactResidentFile::LoadData(const PactFileRuntimeRecord& record)
    {
        const ManifestEntry& entry = *record.entry;
        const PactManifest& manifest = *record.source->manifest;

        if (manifest.header.sourceType == PactSourceType::Pack)
        {
            std::ifstream file(manifest.dataPath, std::ios::binary);
            if (!file)
            {
                return PactReadResult::FileAccessFailed;
            }

            u8* buffer = new u8[entry.dataSize];

            file.seekg(entry.dataOffset);
            file.read(reinterpret_cast<char*>(buffer), entry.dataSize);

            if (!file)
            {
                delete[] buffer;
                return PactReadResult::FileReadFailed;
            }

            PactDigest contentDigest;
            const u8 emptyContent = 0;
            const u8* content = entry.dataSize > 0 ? buffer : &emptyContent;
            crypto_hash_sha256(contentDigest.data(), content, entry.dataSize);
            if (contentDigest != entry.contentDigest)
            {
                delete[] buffer;
                return PactReadResult::FileReadFailed;
            }

            size_t chunkOffset = 0;
            for (u32 i = 0; i < entry.chunkCount; i++)
            {
                const PactChunkInfo& chunk = manifest.chunks[entry.chunkIndex + i];
                PactDigest chunkDigest;
                crypto_hash_sha256(chunkDigest.data(), buffer + chunkOffset, chunk.storedSize);
                if (chunkDigest != chunk.digest)
                {
                    delete[] buffer;
                    return PactReadResult::FileReadFailed;
                }

                chunkOffset += chunk.storedSize;
            }

            data = buffer;
            size = entry.dataSize;
        }
        else
        {
            const std::string& relPath = manifest.stringTable.GetString(entry.pathIndex);
            fs::path filePath = manifest.dataPath / relPath;

            std::ifstream file(filePath, std::ios::binary);
            if (!file)
            {
                return PactReadResult::FileAccessFailed;
            }

            file.seekg(0, std::ios::end);
            size_t fileSize = static_cast<size_t>(file.tellg());
            file.seekg(0, std::ios::beg);

            u8* buffer = new u8[fileSize];
            file.read(reinterpret_cast<char*>(buffer), fileSize);

            if (!file)
            {
                delete[] buffer;
                return PactReadResult::FileReadFailed;
            }

            data = buffer;
            size = fileSize;
        }

        return PactReadResult::Success;
    }

    PactReadResult PactResidentFile::EnsureLoadedSync(const PactFileRuntimeRecord& record)
    {
        PactLoadState expected = PactLoadState::Unloaded;
        if (loadState.compare_exchange_strong(expected, PactLoadState::Loading, std::memory_order_acq_rel))
        {
            PactReadResult result = LoadData(record);
            loadState.store(result == PactReadResult::Success ? PactLoadState::Loaded : PactLoadState::Failed, std::memory_order_release);
            return result;
        }

        while (loadState.load(std::memory_order_acquire) == PactLoadState::Loading)
        {
            std::this_thread::yield();
        }

        return loadState.load() == PactLoadState::Loaded ? PactReadResult::Success : PactReadResult::Failed;
    }

    PactFileHandle::PactFileHandle() : _owner(nullptr), _hasPin(false) { }
    PactFileHandle::PactFileHandle(PactResidentFile* owner) : _owner(owner), _hasPin(false)
    {
        assert(owner == nullptr || owner->handleCount.load(std::memory_order_acquire) > 0);
    }
    PactFileHandle::~PactFileHandle()
    {
        Reset();
    }

    PactFileHandle::PactFileHandle(PactFileHandle&& other) noexcept
        : _owner(std::exchange(other._owner, nullptr))
        , _hasPin(std::exchange(other._hasPin, false))
    {
    }
    PactFileHandle& PactFileHandle::operator=(PactFileHandle&& other) noexcept
    {
        if (this == &other)
            return *this;

        Reset();

        _owner = std::exchange(other._owner, nullptr);
        _hasPin = std::exchange(other._hasPin, false);

        return *this;
    }

    bool PactFileHandle::operator==(const PactFileHandle& other) const
    {
        return _owner == other._owner;
    }

    void PactFileHandle::Reset()
    {
        if (_owner == nullptr)
            return;

        if (_hasPin)
        {
            assert(false && "PactFileHandle is being reset without releasing its pin");

            // Auto Unpin in release
            _owner->RemovePin();
        }

        _owner->ReleaseHandle();
        _owner = nullptr;
        _hasPin = false;
    }

    bool PactFileHandle::IsReady() const
    {
        return _owner->IsReady();
    }

    bool PactFileHandle::IsPinned() const
    {
        return _owner->IsPinned();
    }
    void PactFileHandle::Pin()
    {
        if (_hasPin)
        {
            assert(false && "PactFileHandle attempted to Pin when a pin was already claimed");
            return;
        }

        _hasPin = true;
        _owner->AddPin();
    }
    void PactFileHandle::Unpin()
    {
        if (!_hasPin)
        {
            assert(false && "PactFileHandle attempted to Unpin when no pin was claimed");
            return;
        }

        _hasPin = false;
        _owner->RemovePin();
    }

    PactResidentFile* PactFileHandle::GetOwner()
    {
        return _owner;
    }

    const void* PactFileHandle::GetData() const
    {
        return _owner->GetData();
    }
    const size_t PactFileHandle::GetSize() const
    {
        return _owner->GetSize();
    }
}

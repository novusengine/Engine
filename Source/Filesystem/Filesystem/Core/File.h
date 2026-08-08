#pragma once

#include <Base/Types.h>

#include <xxhash/xxhash64.h>

#include <atomic>
#include <mutex>

namespace PACT
{
    struct ManifestEntry;
    struct PactMount;
    class PactStorage;

    using PactFileID = u64;
    using PactGenerationID = u64;

    enum class PactFileOpenOption
    {
        None,
        Pinned
    };

    enum class PactReadResult : u8
    {
        Success,
        FileNotFound,
        FileAccessFailed,
        FileReadFailed,
        Failed,
        GenerationMismatch,
        Pending
    };

    struct PactFileKey
    {
    public:
        enum class Type : u8
        {
            Packed,
            Loose
        };
    public:
        bool operator==(const PactFileKey& other) const;

    public:
        Type type;
        u64 value;
        PactGenerationID generation;
    };

    enum class PactLoadState
    {
        Unloaded,
        Loading,
        Loaded,
        Stale,
        Failed
    };

    struct PactFileRuntimeRecord
    {
    public:
        const ManifestEntry* entry;
        const PactMount* source;
    };

    struct PactResidentFile
    {
    public:
        bool operator==(const PactResidentFile& other) const;

    public:
        PactFileKey key;
        void* data;
        size_t size;

        std::atomic<u32> handleCount;
        std::atomic<u32> pinCount;
        std::atomic<PactLoadState> loadState;

        PactStorage* owner;

    public:
        void AddHandle();
        void ReleaseHandle();

        bool IsReady() const;

        bool IsPinned() const;
        void AddPin();
        void RemovePin();

        const void* GetData() const;
        const size_t GetSize() const;

        PactReadResult LoadData(const PactFileRuntimeRecord& record);
        PactReadResult EnsureLoadedSync(const PactFileRuntimeRecord& record);
    };

    struct PactFileHandle
    {
    public:
        PactFileHandle();
        PactFileHandle(PactResidentFile* owner);
        ~PactFileHandle();

        PactFileHandle(const PactFileHandle&) = delete;
        PactFileHandle& operator=(const PactFileHandle&) = delete;

        PactFileHandle(PactFileHandle&& other) noexcept;
        PactFileHandle& operator=(PactFileHandle&& other) noexcept;

        bool operator==(const PactFileHandle& other) const;

    public:
        bool IsReady() const;

        bool IsPinned() const;
        void Pin();
        void Unpin();

        PactResidentFile* GetOwner();
        const void* GetData() const;
        const size_t GetSize() const;

    private:
        void Reset();

    private:
        PactResidentFile* _owner;
        bool _hasPin;
    };

    struct PactIORequest
    {
    public:
        PactResidentFile* file;
        PactFileRuntimeRecord record;
    };
}

template<>
struct std::hash<PACT::PactFileKey>
{
    size_t operator()(const PACT::PactFileKey& x) const
    {
        XXHash64 hash(0);
        hash.add(&x.type, sizeof(x.type));
        hash.add(&x.value, sizeof(x.value));
        hash.add(&x.generation, sizeof(x.generation));

        return hash.hash();
    }
};

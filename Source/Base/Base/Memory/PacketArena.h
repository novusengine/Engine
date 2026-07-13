#pragma once
#include "Bytebuffer.h"

#include <atomic>
#include <memory>

class PacketArenaState;
struct PacketAllocation;

// Shared reservation limit for a group of packet arenas. The budget is charged
// by arena blocks, so queued PacketRefs remain accounted for until they release
// their backing block.
class PacketArenaBudget
{
public:
    explicit PacketArenaBudget(size_t maxReservedBytes);

    [[nodiscard]] bool TryReserve(size_t bytes);
    void Release(size_t bytes);

    [[nodiscard]] size_t GetReservedBytes() const;
    [[nodiscard]] size_t GetMaxReservedBytes() const;

private:
    size_t _maxReservedBytes = 0;
    std::atomic<size_t> _reservedBytes = 0;
};

// A PacketRef keeps serialized packet storage valid until the final queued send releases it.
class PacketRef
{
public:
    PacketRef() = default;
    PacketRef(const PacketRef& other) noexcept;
    PacketRef(PacketRef&& other) noexcept;
    PacketRef& operator=(const PacketRef& other) noexcept;
    PacketRef& operator=(PacketRef&& other) noexcept;
    ~PacketRef();

    [[nodiscard]] bool IsValid() const { return _allocation != nullptr; }
    [[nodiscard]] const u8* GetDataPointer() const;
    [[nodiscard]] size_t GetWrittenData() const;
    [[nodiscard]] size_t GetCapacity() const;
    [[nodiscard]] Bytebuffer CreateReadView() const;

private:
    explicit PacketRef(PacketAllocation* allocation) : _allocation(allocation) { }

    void Reset() noexcept;

private:
    friend class PacketWriter;

    PacketAllocation* _allocation = nullptr;
};

// PacketWriter owns an arena slot while it is mutable. Seal transfers it to PacketRef.
class PacketWriter
{
public:
    PacketWriter() = default;
    PacketWriter(const PacketWriter&) = delete;
    PacketWriter& operator=(const PacketWriter&) = delete;
    PacketWriter(PacketWriter&& other) noexcept;
    PacketWriter& operator=(PacketWriter&& other) noexcept;
    ~PacketWriter();

    [[nodiscard]] bool IsValid() const { return _allocation != nullptr; }
    Bytebuffer& GetBuffer();
    PacketRef Seal();

private:
    explicit PacketWriter(PacketAllocation* allocation);
    void Reset() noexcept;

private:
    friend class PacketArena;

    PacketAllocation* _allocation = nullptr;
    Bytebuffer _buffer;
};

// Single-producer packet arena. The owning producer thread acquires slots and
// other threads return them through a lock-free remote-free list.
class PacketArena
{
public:
    static constexpr size_t DEFAULT_MAX_RESERVED_BYTES = 128ull * 1024ull * 1024ull;
    static constexpr size_t DEFAULT_BLOCK_SIZE = 2ull * 1024ull * 1024ull;
    static constexpr size_t DEFAULT_WARM_BLOCKS_PER_SIZE_CLASS = 1;

    explicit PacketArena(size_t maxReservedBytes = DEFAULT_MAX_RESERVED_BYTES, size_t blockSize = DEFAULT_BLOCK_SIZE);
    PacketArena(std::shared_ptr<PacketArenaBudget> budget, size_t maxReservedBytes = DEFAULT_MAX_RESERVED_BYTES, size_t blockSize = DEFAULT_BLOCK_SIZE);

    PacketWriter Acquire(size_t minimumCapacity);
    void Drain();
    size_t Trim(size_t warmBlocksPerSizeClass = DEFAULT_WARM_BLOCKS_PER_SIZE_CLASS);

    [[nodiscard]] size_t GetReservedBytes() const;
    [[nodiscard]] size_t GetMaxReservedBytes() const;
    [[nodiscard]] size_t GetInUseBytes() const;
    [[nodiscard]] size_t GetInUseAllocationCount() const;
    [[nodiscard]] size_t GetAllocationFailureCount() const;
    [[nodiscard]] size_t GetSharedReservedBytes() const;
    [[nodiscard]] size_t GetSharedMaxReservedBytes() const;

private:
    std::shared_ptr<PacketArenaState> _state;
};

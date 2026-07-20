#include "PacketArena.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <new>
#include <thread>
#include <vector>

namespace
{
    constexpr std::array<size_t, 14> PacketSizeClasses =
    {
        128,
        256,
        512,
        1024,
        2048,
        4096,
        8192,
        16384,
        32768,
        65536,
        131072,
        262144,
        524288,
        1048576
    };

    constexpr size_t PacketSizeClassCount = PacketSizeClasses.size();

    size_t AlignUp(size_t value, size_t alignment)
    {
        return (value + alignment - 1) & ~(alignment - 1);
    }
}

class PacketBlock;

struct PacketAllocation
{
    std::atomic<u32> references = 0;
    std::shared_ptr<PacketArenaState> arena;
    PacketBlock* block = nullptr;
    PacketAllocation* next = nullptr;
    u8* data = nullptr;
    size_t capacity = 0;
    size_t writtenData = 0;
    size_t sizeClass = 0;
};

class PacketBlock
{
public:
    PacketBlock(size_t sizeClass, size_t capacity, size_t slotSize, size_t slotCount)
        : _sizeClass(sizeClass), _slotSize(slotSize), _slotCount(slotCount), _freeSlotCount(slotCount)
    {
        _memory = static_cast<u8*>(::operator new(_slotSize * _slotCount, std::align_val_t(alignof(std::max_align_t))));

        for (size_t i = 0; i < _slotCount; i++)
        {
            u8* slot = _memory + i * _slotSize;
            PacketAllocation* allocation = new (slot) PacketAllocation();
            allocation->block = this;
            allocation->data = slot + AlignUp(sizeof(PacketAllocation), alignof(std::max_align_t));
            allocation->capacity = capacity;
            allocation->sizeClass = _sizeClass;
        }
    }

    ~PacketBlock()
    {
        for (size_t i = 0; i < _slotCount; i++)
        {
            PacketAllocation* allocation = reinterpret_cast<PacketAllocation*>(_memory + i * _slotSize);
            allocation->~PacketAllocation();
        }

        ::operator delete(_memory, std::align_val_t(alignof(std::max_align_t)));
    }

    PacketAllocation* GetAllocation(size_t index)
    {
        return reinterpret_cast<PacketAllocation*>(_memory + index * _slotSize);
    }

    [[nodiscard]] size_t GetReservedBytes() const
    {
        return _slotSize * _slotCount;
    }

    [[nodiscard]] size_t GetSlotCount() const
    {
        return _slotCount;
    }

    [[nodiscard]] size_t GetSizeClass() const
    {
        return _sizeClass;
    }

    void MarkAcquired()
    {
        _freeSlotCount.fetch_sub(1, std::memory_order_relaxed);
    }

    void MarkReleased()
    {
        _freeSlotCount.fetch_add(1, std::memory_order_release);
    }

    void MarkRemoteFreePending()
    {
        _pendingRemoteFrees.fetch_add(1, std::memory_order_relaxed);
    }

    void MarkRemoteFreeDrained()
    {
        _pendingRemoteFrees.fetch_sub(1, std::memory_order_relaxed);
    }

    [[nodiscard]] bool IsFullyFree() const
    {
        return _freeSlotCount.load(std::memory_order_acquire) == _slotCount &&
            _pendingRemoteFrees.load(std::memory_order_acquire) == 0;
    }

private:
    size_t _sizeClass = 0;
    size_t _slotSize = 0;
    size_t _slotCount = 0;
    u8* _memory = nullptr;
    std::atomic<size_t> _freeSlotCount = 0;
    std::atomic<size_t> _pendingRemoteFrees = 0;
};

class PacketArenaState : public std::enable_shared_from_this<PacketArenaState>
{
public:
    PacketArenaState(std::shared_ptr<PacketArenaBudget> budget, size_t maxReservedBytes, size_t blockSize)
        : _budget(std::move(budget)), _maxReservedBytes(maxReservedBytes), _blockSize(blockSize)
    {
        if (_budget == nullptr)
            _budget = std::make_shared<PacketArenaBudget>(maxReservedBytes);
    }

    ~PacketArenaState()
    {
        _budget->Release(_reservedBytes.load(std::memory_order_relaxed));
    }

    PacketAllocation* Acquire(size_t minimumCapacity)
    {
        AssertOwnerThread();

        const size_t sizeClass = FindSizeClass(minimumCapacity);
        if (sizeClass == PacketSizeClassCount)
        {
            _allocationFailureCount.fetch_add(1, std::memory_order_relaxed);
            return nullptr;
        }

        std::vector<PacketAllocation*>& freeList = _freeLists[sizeClass];
        if (freeList.empty())
        {
            Drain(sizeClass);
            if (freeList.empty() && !AllocateBlock(sizeClass))
            {
                _allocationFailureCount.fetch_add(1, std::memory_order_relaxed);
                return nullptr;
            }
        }

        PacketAllocation* allocation = freeList.back();
        freeList.pop_back();
        allocation->block->MarkAcquired();
        allocation->references.store(1, std::memory_order_relaxed);
        allocation->writtenData = 0;
        allocation->arena = shared_from_this();
        _inUseBytes.fetch_add(allocation->capacity, std::memory_order_relaxed);
        _inUseAllocationCount.fetch_add(1, std::memory_order_relaxed);
        return allocation;
    }

    void Release(PacketAllocation* allocation)
    {
        _inUseBytes.fetch_sub(allocation->capacity, std::memory_order_relaxed);
        _inUseAllocationCount.fetch_sub(1, std::memory_order_relaxed);

        const size_t sizeClass = allocation->sizeClass;
        allocation->block->MarkRemoteFreePending();
        PacketAllocation* head = _remoteFreeLists[sizeClass].load(std::memory_order_relaxed);
        do
        {
            allocation->next = head;
        }
        while (!_remoteFreeLists[sizeClass].compare_exchange_weak(head, allocation, std::memory_order_release, std::memory_order_relaxed));

        allocation->block->MarkReleased();
    }

    void Drain()
    {
        AssertOwnerThread();

        for (size_t sizeClass = 0; sizeClass < PacketSizeClassCount; sizeClass++)
            Drain(sizeClass);
    }

    void Drain(size_t sizeClass)
    {
        AssertOwnerThread();

        PacketAllocation* allocation = _remoteFreeLists[sizeClass].exchange(nullptr, std::memory_order_acquire);
        while (allocation != nullptr)
        {
            PacketAllocation* next = allocation->next;
            allocation->next = nullptr;
            allocation->block->MarkRemoteFreeDrained();
            _freeLists[sizeClass].push_back(allocation);
            allocation = next;
        }
    }

    size_t Trim(size_t warmBlocksPerSizeClass)
    {
        AssertOwnerThread();
        Drain();

        size_t trimmedBytes = 0;
        for (size_t sizeClass = 0; sizeClass < PacketSizeClassCount; sizeClass++)
        {
            size_t blockCount = 0;
            for (const std::unique_ptr<PacketBlock>& block : _blocks)
            {
                if (block->GetSizeClass() == sizeClass)
                    blockCount++;
            }

            for (auto it = _blocks.begin(); it != _blocks.end() && blockCount > warmBlocksPerSizeClass;)
            {
                PacketBlock* block = it->get();
                if (block->GetSizeClass() != sizeClass || !block->IsFullyFree())
                {
                    ++it;
                    continue;
                }

                std::vector<PacketAllocation*>& freeList = _freeLists[sizeClass];
                std::erase_if(freeList, [block](PacketAllocation* allocation)
                {
                    return allocation->block == block;
                });

                const size_t reservedBytes = block->GetReservedBytes();
                it = _blocks.erase(it);
                _reservedBytes.fetch_sub(reservedBytes, std::memory_order_relaxed);
                _budget->Release(reservedBytes);
                trimmedBytes += reservedBytes;
                blockCount--;
            }
        }

        return trimmedBytes;
    }

    [[nodiscard]] size_t GetReservedBytes() const { return _reservedBytes.load(std::memory_order_relaxed); }
    [[nodiscard]] size_t GetMaxReservedBytes() const { return _maxReservedBytes; }
    [[nodiscard]] size_t GetInUseBytes() const { return _inUseBytes.load(std::memory_order_relaxed); }
    [[nodiscard]] size_t GetInUseAllocationCount() const { return _inUseAllocationCount.load(std::memory_order_relaxed); }
    [[nodiscard]] size_t GetAllocationFailureCount() const { return _allocationFailureCount.load(std::memory_order_relaxed); }
    [[nodiscard]] size_t GetSharedReservedBytes() const { return _budget->GetReservedBytes(); }
    [[nodiscard]] size_t GetSharedMaxReservedBytes() const { return _budget->GetMaxReservedBytes(); }

private:
    size_t FindSizeClass(size_t minimumCapacity) const
    {
        for (size_t i = 0; i < PacketSizeClassCount; i++)
        {
            if (minimumCapacity <= PacketSizeClasses[i])
                return i;
        }

        return PacketSizeClassCount;
    }

    bool AllocateBlock(size_t sizeClass)
    {
        const size_t capacity = PacketSizeClasses[sizeClass];
        const size_t payloadOffset = AlignUp(sizeof(PacketAllocation), alignof(std::max_align_t));
        const size_t slotSize = AlignUp(payloadOffset + capacity, alignof(std::max_align_t));
        const size_t slotCount = std::max<size_t>(1, _blockSize / slotSize);
        const size_t reservedBytes = slotSize * slotCount;
        const size_t currentReservedBytes = _reservedBytes.load(std::memory_order_relaxed);

        if (reservedBytes > _maxReservedBytes - currentReservedBytes)
            return false;

        std::vector<PacketAllocation*>& freeList = _freeLists[sizeClass];
        freeList.reserve(freeList.size() + slotCount);
        _blocks.reserve(_blocks.size() + 1);

        if (!_budget->TryReserve(reservedBytes))
            return false;

        std::unique_ptr<PacketBlock> block;
        try
        {
            block = std::make_unique<PacketBlock>(sizeClass, capacity, slotSize, slotCount);
        }
        catch (...)
        {
            _budget->Release(reservedBytes);
            throw;
        }

        for (size_t i = 0; i < block->GetSlotCount(); i++)
        {
            freeList.push_back(block->GetAllocation(i));
        }

        _reservedBytes.store(currentReservedBytes + block->GetReservedBytes(), std::memory_order_relaxed);
        _blocks.push_back(std::move(block));
        return true;
    }

    void AssertOwnerThread()
    {
        const std::thread::id currentThreadID = std::this_thread::get_id();
        if (!_hasOwnerThread)
        {
            _ownerThreadID = currentThreadID;
            _hasOwnerThread = true;
            return;
        }

        assert(_ownerThreadID == currentThreadID);
    }

private:
    size_t _maxReservedBytes = 0;
    size_t _blockSize = 0;
    std::shared_ptr<PacketArenaBudget> _budget;
    std::atomic<size_t> _reservedBytes = 0;
    std::atomic<size_t> _inUseBytes = 0;
    std::atomic<size_t> _inUseAllocationCount = 0;
    std::atomic<size_t> _allocationFailureCount = 0;
    std::array<std::vector<PacketAllocation*>, PacketSizeClassCount> _freeLists;
    std::array<std::atomic<PacketAllocation*>, PacketSizeClassCount> _remoteFreeLists = { };
    std::vector<std::unique_ptr<PacketBlock>> _blocks;
    std::thread::id _ownerThreadID;
    bool _hasOwnerThread = false;
};

PacketArenaBudget::PacketArenaBudget(size_t maxReservedBytes)
    : _maxReservedBytes(maxReservedBytes)
{
}

bool PacketArenaBudget::TryReserve(size_t bytes)
{
    size_t reservedBytes = _reservedBytes.load(std::memory_order_relaxed);
    while (true)
    {
        if (bytes > _maxReservedBytes - reservedBytes)
            return false;

        if (_reservedBytes.compare_exchange_weak(reservedBytes, reservedBytes + bytes, std::memory_order_acq_rel, std::memory_order_relaxed))
            return true;
    }
}

void PacketArenaBudget::Release(size_t bytes)
{
    _reservedBytes.fetch_sub(bytes, std::memory_order_acq_rel);
}

size_t PacketArenaBudget::GetReservedBytes() const
{
    return _reservedBytes.load(std::memory_order_relaxed);
}

size_t PacketArenaBudget::GetMaxReservedBytes() const
{
    return _maxReservedBytes;
}

PacketRef::PacketRef(const PacketRef& other) noexcept : _allocation(other._allocation)
{
    if (_allocation != nullptr)
        _allocation->references.fetch_add(1, std::memory_order_relaxed);
}

PacketRef::PacketRef(PacketRef&& other) noexcept : _allocation(other._allocation)
{
    other._allocation = nullptr;
}

PacketRef& PacketRef::operator=(const PacketRef& other) noexcept
{
    if (this == &other)
        return *this;

    Reset();
    _allocation = other._allocation;
    if (_allocation != nullptr)
        _allocation->references.fetch_add(1, std::memory_order_relaxed);

    return *this;
}

PacketRef& PacketRef::operator=(PacketRef&& other) noexcept
{
    if (this == &other)
        return *this;

    Reset();
    _allocation = other._allocation;
    other._allocation = nullptr;
    return *this;
}

PacketRef::~PacketRef()
{
    Reset();
}

const u8* PacketRef::GetDataPointer() const
{
    return _allocation != nullptr ? _allocation->data : nullptr;
}

size_t PacketRef::GetWrittenData() const
{
    return _allocation != nullptr ? _allocation->writtenData : 0;
}

size_t PacketRef::GetCapacity() const
{
    return _allocation != nullptr ? _allocation->capacity : 0;
}

Bytebuffer PacketRef::CreateReadView() const
{
    return Bytebuffer::CreateReadOnlyView(GetDataPointer(), GetWrittenData());
}

void PacketRef::Reset() noexcept
{
    if (_allocation == nullptr)
        return;

    PacketAllocation* allocation = _allocation;
    _allocation = nullptr;
    if (allocation->references.fetch_sub(1, std::memory_order_acq_rel) != 1)
        return;

    std::shared_ptr<PacketArenaState> arena = std::move(allocation->arena);
    allocation->writtenData = 0;
    arena->Release(allocation);
}

PacketWriter::PacketWriter(PacketAllocation* allocation)
    : _allocation(allocation), _buffer(allocation != nullptr ? allocation->data : nullptr, allocation != nullptr ? allocation->capacity : 0)
{
}

PacketWriter::PacketWriter(PacketWriter&& other) noexcept
    : _allocation(other._allocation), _buffer(std::move(other._buffer))
{
    other._allocation = nullptr;
}

PacketWriter& PacketWriter::operator=(PacketWriter&& other) noexcept
{
    if (this == &other)
        return *this;

    Reset();
    _allocation = other._allocation;
    _buffer = std::move(other._buffer);
    other._allocation = nullptr;
    return *this;
}

PacketWriter::~PacketWriter()
{
    Reset();
}

Bytebuffer& PacketWriter::GetBuffer()
{
    assert(_allocation != nullptr);
    return _buffer;
}

PacketRef PacketWriter::Seal()
{
    if (_allocation == nullptr)
        return { };

    _allocation->writtenData = _buffer.writtenData;
    std::atomic_thread_fence(std::memory_order_release);

    PacketRef result(_allocation);
    _allocation = nullptr;
    _buffer = Bytebuffer();
    return result;
}

void PacketWriter::Reset() noexcept
{
    if (_allocation == nullptr)
        return;

    PacketRef packet(_allocation);
    _allocation = nullptr;
    _buffer = Bytebuffer();
}

PacketArena::PacketArena(size_t maxReservedBytes, size_t blockSize)
    : PacketArena(std::make_shared<PacketArenaBudget>(maxReservedBytes), maxReservedBytes, blockSize)
{
}

PacketArena::PacketArena(std::shared_ptr<PacketArenaBudget> budget, size_t maxReservedBytes, size_t blockSize)
    : _state(std::make_shared<PacketArenaState>(std::move(budget), maxReservedBytes, blockSize))
{
}

PacketWriter PacketArena::Acquire(size_t minimumCapacity)
{
    return PacketWriter(_state->Acquire(minimumCapacity));
}

void PacketArena::Drain()
{
    _state->Drain();
}

size_t PacketArena::Trim(size_t warmBlocksPerSizeClass)
{
    return _state->Trim(warmBlocksPerSizeClass);
}

size_t PacketArena::GetReservedBytes() const
{
    return _state->GetReservedBytes();
}

size_t PacketArena::GetMaxReservedBytes() const
{
    return _state->GetMaxReservedBytes();
}

size_t PacketArena::GetInUseBytes() const
{
    return _state->GetInUseBytes();
}

size_t PacketArena::GetInUseAllocationCount() const
{
    return _state->GetInUseAllocationCount();
}

size_t PacketArena::GetAllocationFailureCount() const
{
    return _state->GetAllocationFailureCount();
}

size_t PacketArena::GetSharedReservedBytes() const
{
    return _state->GetSharedReservedBytes();
}

size_t PacketArena::GetSharedMaxReservedBytes() const
{
    return _state->GetSharedMaxReservedBytes();
}

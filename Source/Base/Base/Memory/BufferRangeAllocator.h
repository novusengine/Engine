#pragma once
#include "Base/Types.h"

#include <vector>

struct BufferRangeFrame
{
    size_t offset = 0;
    size_t size = 0;
    bool wasHole = false;
};

enum class BufferRangeSortStrategy : u8
{
    SizeAscending,
    SizeDescending,
    OffsetAscending,
    OffsetDescending,
    None
};

enum class BufferRangeAllocationStrategy : u8
{
    ReuseFirst, // Try to reuse freed ranges before overallocated capacity
    CapacityFirst, // Try to use overallocated capacity before reusing freed ranges
    BestFit, // Look through all the frames and find the best fit, regardless how we sort, is gonna cost more
    PerfectFit, // Try to find a frame that fits perfectly, otherwise CapacityFirst
};

template<BufferRangeSortStrategy SORT_STRATEGY = BufferRangeSortStrategy::SizeAscending, BufferRangeAllocationStrategy ALLOCATION_STRATEGY = BufferRangeAllocationStrategy::ReuseFirst>
class BufferRangeAllocator
{
public:
    BufferRangeAllocator() { }

    void Init(size_t bufferOffset, size_t bufferSize)
    {
        _freeFrames.reserve(64);
        _freeFrames.clear();

        _allocatedBytes = bufferOffset;
        _currentSize = bufferSize;
    }
    void Reset()
    {
        _freeFrames.clear();

        _allocatedBytes = 0;
    }

    bool Allocate(size_t size, BufferRangeFrame& frame)
    {
        // If the allocator has no data, we can't allocate anything
        if (size == 0)
            return false;

        frame.size = size;

        if constexpr (ALLOCATION_STRATEGY == BufferRangeAllocationStrategy::ReuseFirst)
        {
            // First we try to find a free frame that fits the size
            for (size_t i = 0; i < _freeFrames.size(); i++)
            {
                BufferRangeFrame& freeFrame = _freeFrames[i];

                if (size <= freeFrame.size)
                {
                    frame.offset = freeFrame.offset;
                    frame.wasHole = true;

                    freeFrame.offset += size;
                    freeFrame.size -= size;

                    if (freeFrame.size == 0)
                        _freeFrames.erase(_freeFrames.begin() + i);

                    return true;
                }
            }
        }
        else if constexpr (ALLOCATION_STRATEGY == BufferRangeAllocationStrategy::BestFit)
        {
            // Loop through all free frames and find the best fit
            size_t bestFitIndex = SIZE_MAX;
            size_t bestFitSize = SIZE_MAX;
            for (size_t i = 0; i < _freeFrames.size(); i++)
            {
                BufferRangeFrame& freeFrame = _freeFrames[i];

                if (size <= freeFrame.size && freeFrame.size < bestFitSize)
                {
                    bestFitIndex = i;
                    bestFitSize = freeFrame.size;
                }
            }

            if (bestFitIndex != SIZE_MAX)
            {
                BufferRangeFrame& freeFrame = _freeFrames[bestFitIndex];

                frame.offset = freeFrame.offset;
                frame.wasHole = true;

                freeFrame.offset += size;
                freeFrame.size -= size;

                if (freeFrame.size == 0)
                    _freeFrames.erase(_freeFrames.begin() + bestFitIndex);

                return true;
            }
        }
        else if constexpr (ALLOCATION_STRATEGY == BufferRangeAllocationStrategy::PerfectFit)
        {
            // First we try to find a free frame that perfectly fits the size
            for (size_t i = 0; i < _freeFrames.size(); i++)
            {
                BufferRangeFrame& freeFrame = _freeFrames[i];

                if (size == freeFrame.size)
                {
                    frame.offset = freeFrame.offset;
                    frame.wasHole = true;

                    _freeFrames.erase(_freeFrames.begin() + i);

                    return true;
                }
            }
        }

        // Then we take from the end of the buffer if there is enough space
        if (size <= _currentSize - _allocatedBytes)
        {
            frame.offset = _allocatedBytes;
            frame.wasHole = false;

            _allocatedBytes += size;
            return true;
        }
        
        if constexpr (ALLOCATION_STRATEGY == BufferRangeAllocationStrategy::CapacityFirst)
        {
            // If we checked capacity and there is not enough space, we try to find a free frame that fits the size
            for (size_t i = 0; i < _freeFrames.size(); i++)
            {
                BufferRangeFrame& freeFrame = _freeFrames[i];

                if (size <= freeFrame.size)
                {
                    frame.offset = freeFrame.offset;
                    frame.wasHole = true;

                    freeFrame.offset += size;
                    freeFrame.size -= size;

                    if (freeFrame.size == 0)
                        _freeFrames.erase(_freeFrames.begin() + i);

                    return true;
                }
            }
        }

        // Otherwise we return false
        return false;
    }
    bool Allocate(size_t size, size_t alignment, BufferRangeFrame& frame)
    {
        if (size == 0)
            return false;

        frame.size = size;

        if constexpr (ALLOCATION_STRATEGY == BufferRangeAllocationStrategy::ReuseFirst)
        {
            // First we try to find a free frame that fits the size
            for (size_t i = 0; i < _freeFrames.size(); i++)
            {
                BufferRangeFrame& freeFrame = _freeFrames[i];

                size_t alignmentOffset = (alignment - (freeFrame.offset % alignment)) % alignment;
                size_t alignedSize = size + alignmentOffset;

                if (alignedSize <= freeFrame.size)
                {
                    frame.offset = freeFrame.offset + alignmentOffset;
                    frame.size = alignedSize;
                    frame.wasHole = true;

                    freeFrame.offset += alignedSize;
                    freeFrame.size -= alignedSize;

                    if (freeFrame.size == 0)
                        _freeFrames.erase(_freeFrames.begin() + i);

                    return true;
                }
            }
        }
        else if constexpr (ALLOCATION_STRATEGY == BufferRangeAllocationStrategy::BestFit)
        {
            // Loop through all free frames and find the best fit
            size_t bestFitIndex = SIZE_MAX;
            size_t bestFitSize = SIZE_MAX;
            for (size_t i = 0; i < _freeFrames.size(); i++)
            {
                BufferRangeFrame& freeFrame = _freeFrames[i];

                size_t alignmentOffset = (alignment - (freeFrame.offset % alignment)) % alignment;
                size_t alignedSize = size + alignmentOffset;

                if (alignedSize <= freeFrame.size && freeFrame.size < bestFitSize)
                {
                    bestFitIndex = i;
                    bestFitSize = alignedSize;
                }
            }

            if (bestFitIndex != SIZE_MAX)
            {
                BufferRangeFrame& freeFrame = _freeFrames[bestFitIndex];

                size_t alignmentOffset = (alignment - (freeFrame.offset % alignment)) % alignment;
                size_t alignedSize = size + alignmentOffset;

                frame.offset = freeFrame.offset + alignmentOffset;
                frame.size = alignedSize;
                frame.wasHole = true;

                freeFrame.offset += alignedSize;
                freeFrame.size -= alignedSize;

                if (freeFrame.size == 0)
                    _freeFrames.erase(_freeFrames.begin() + bestFitIndex);

                return true;
            }
        }
        else if constexpr (ALLOCATION_STRATEGY == BufferRangeAllocationStrategy::PerfectFit)
        {
            // First we try to find a free frame that perfectly fits the size
            for (size_t i = 0; i < _freeFrames.size(); i++)
            {
                BufferRangeFrame& freeFrame = _freeFrames[i];

                size_t alignmentOffset = (alignment - (freeFrame.offset % alignment)) % alignment;
                size_t alignedSize = size + alignmentOffset;

                if (alignedSize == freeFrame.size)
                {
                    frame.offset = freeFrame.offset + alignmentOffset;
                    frame.size = alignedSize;
                    frame.wasHole = true;

                    _freeFrames.erase(_freeFrames.begin() + i);

                    return true;
                }
            }
        }

        // First we try to find a free frame that fits the size
        for (size_t i = 0; i < _freeFrames.size(); i++)
        {
            BufferRangeFrame& freeFrame = _freeFrames[i];

            size_t alignmentOffset = (alignment - (freeFrame.offset % alignment)) % alignment;
            size_t alignedSize = size + alignmentOffset;

            if (alignedSize <= freeFrame.size)
            {
                frame.offset = freeFrame.offset;
                frame.wasHole = true;

                frame.offset = freeFrame.offset + alignmentOffset;
                frame.size = size;

                freeFrame.offset += alignedSize;
                freeFrame.size -= alignedSize;

                if (freeFrame.size == 0)
                    _freeFrames.erase(_freeFrames.begin() + i);

                return true;
            }
        }

        // Then we take from the end of the buffer if there is enough space
        size_t alignmentOffset = (alignment - (_allocatedBytes % alignment)) % alignment;
        size_t alignedSize = size + alignmentOffset;
        if (alignedSize <= _currentSize - _allocatedBytes)
        {
            frame.offset = _allocatedBytes + alignmentOffset;
            frame.wasHole = false;

            _allocatedBytes += alignedSize;
            return true;
        }

        if constexpr (ALLOCATION_STRATEGY == BufferRangeAllocationStrategy::CapacityFirst)
        {
            // If we checked capacity and there is not enough space, we try to find a free frame that fits the size
            for (size_t i = 0; i < _freeFrames.size(); i++)
            {
                BufferRangeFrame& freeFrame = _freeFrames[i];

                size_t alignmentOffset = (alignment - (freeFrame.offset % alignment)) % alignment;
                size_t alignedSize = size + alignmentOffset;

                if (alignedSize <= freeFrame.size)
                {
                    frame.offset = freeFrame.offset + alignmentOffset;
                    frame.size = alignedSize;
                    frame.wasHole = true;

                    freeFrame.offset += alignedSize;
                    freeFrame.size -= alignedSize;

                    if (freeFrame.size == 0)
                        _freeFrames.erase(_freeFrames.begin() + i);

                    return true;
                }
            }
        }

        // Otherwise we return false
        return false;
    }
    bool Free(const BufferRangeFrame& frame)
    {
        BufferRangeFrame& newFreeFrame = _freeFrames.emplace_back();
        newFreeFrame = frame;

        TryMergeFrames();

        return true;
    }
    bool Grow(size_t newSize)
    {
        if (newSize < _currentSize)
            return false;

        _currentSize = newSize;
        return true;
    }

    inline size_t Size() const { return _currentSize; }
    inline size_t AllocatedBytes() const { return _allocatedBytes; }

    void TryMergeFrames()
    {
        if (_freeFrames.size() > 1)
        {
            // Check if we can merge 'frame' with any free frame
            for (i32 i = static_cast<i32>(_freeFrames.size()) - 2; i >= 0; i--)
            {
                BufferRangeFrame& curFrame = _freeFrames[i]; // The currently iterated frame
                BufferRangeFrame& nextFrame = _freeFrames[i + 1]; // Current+1, the one we're trying to remove

                size_t curFrameEnd = curFrame.offset + curFrame.size;
                if (nextFrame.offset == curFrameEnd)
                {
                    curFrame.size += nextFrame.size;
                    _freeFrames.erase(_freeFrames.begin() + i + 1);
                }
            }

            // Sort the free frames depending on strategy
            if constexpr (SORT_STRATEGY == BufferRangeSortStrategy::SizeAscending)
            {
                std::sort(_freeFrames.begin(), _freeFrames.end(), [](const BufferRangeFrame& a, const BufferRangeFrame& b) { return a.size < b.size; });
            }
            else if constexpr (SORT_STRATEGY == BufferRangeSortStrategy::SizeDescending)
            {
                std::sort(_freeFrames.begin(), _freeFrames.end(), [](const BufferRangeFrame& a, const BufferRangeFrame& b) { return a.size > b.size; });
            }
            else if constexpr (SORT_STRATEGY == BufferRangeSortStrategy::OffsetAscending)
            {
                std::sort(_freeFrames.begin(), _freeFrames.end(), [](const BufferRangeFrame& a, const BufferRangeFrame& b) { return a.offset < b.offset; });
            }
            else if constexpr (SORT_STRATEGY == BufferRangeSortStrategy::OffsetDescending)
            {
                std::sort(_freeFrames.begin(), _freeFrames.end(), [](const BufferRangeFrame& a, const BufferRangeFrame& b) { return a.offset > b.offset; });
            }
        }
    }
    std::vector<BufferRangeFrame>& GetFreeFrames() { return _freeFrames; }

private:
    size_t _currentSize = 0;
    size_t _allocatedBytes = 0;
    std::vector<BufferRangeFrame> _freeFrames;
};
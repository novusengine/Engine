#pragma once
#include "Base/Types.h"

#include <vector>

struct BufferRangeFrame
{
    size_t offset = 0;
    size_t size = 0;
};

class BufferRangeAllocator
{
public:
    BufferRangeAllocator() { }

    void Init(size_t bufferOffset, size_t bufferSize, size_t freeSize = std::numeric_limits<size_t>::max());
    void Reset();

    bool Allocate(size_t size, BufferRangeFrame& frame);
    bool Allocate(size_t size, size_t alignment, BufferRangeFrame& frame);
    bool Free(const BufferRangeFrame& frame);
    bool Grow(size_t newSize);

    inline size_t Offset() const { return _currentOffset; }
    inline size_t Size() const { return _currentSize; }
    inline size_t AllocatedBytes() const { return _allocatedBytes; }

    void TryMergeFrames();
    std::vector<BufferRangeFrame>& GetFreeFrames() { return _freeFrames; }

private:
    size_t _currentOffset = 0;
    size_t _currentSize = 0;
    size_t _allocatedBytes = 0;
    std::vector<BufferRangeFrame> _freeFrames;
};
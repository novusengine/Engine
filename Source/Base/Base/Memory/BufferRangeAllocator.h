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

    void Init(size_t bufferOffset, size_t bufferSize);
    void Reset();

    bool Allocate(size_t size, BufferRangeFrame& frame);
    bool Allocate(size_t size, size_t alignment, BufferRangeFrame& frame);
    bool Free(const BufferRangeFrame& frame);
    bool Grow(size_t newSize);

    size_t Offset() { return _currentOffset; }
    size_t Size() { return _currentSize; }
    size_t AllocatedBytes() { return _allocatedBytes; }
private:
    void TryMergeFrames();
    
private:
    size_t _currentOffset = 0;
    size_t _currentSize = 0;
    size_t _allocatedBytes = 0;
    std::vector<BufferRangeFrame> _freeFrames;
};
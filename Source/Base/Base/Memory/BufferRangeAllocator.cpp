#include "BufferRangeAllocator.h"
#include "Base/Util/DebugHandler.h"

void BufferRangeAllocator::Init(size_t bufferOffset, size_t bufferSize, size_t freeSize)
{
    _freeFrames.reserve(64);
    _freeFrames.clear();

    if (freeSize == std::numeric_limits<size_t>::max())
    {
        freeSize = bufferSize;
    }

    BufferRangeFrame& frame = _freeFrames.emplace_back();
    frame.offset = bufferOffset;
    frame.size = freeSize;

    _currentOffset = bufferOffset;
    _currentSize = bufferSize;
    _allocatedBytes = bufferOffset;
}

void BufferRangeAllocator::Reset()
{
    _freeFrames.clear();

    BufferRangeFrame& frame = _freeFrames.emplace_back();
    frame.offset = _currentOffset;
    frame.size = _currentSize;

    _allocatedBytes = 0;
}

bool BufferRangeAllocator::Allocate(size_t size, BufferRangeFrame& frame)
{
    if (size == 0)
        return false;

    size_t index = std::numeric_limits<size_t>().max();
    bool wasLastFrame = false;

    for (size_t i = 0; i < _freeFrames.size(); i++)
    {
        BufferRangeFrame& freeFrame = _freeFrames[i];

        if (size <= freeFrame.size)
        {
            index = i;
            wasLastFrame = i == _freeFrames.size() - 1;
            break;
        }
    }

    if (index != std::numeric_limits<size_t>().max())
    {
        BufferRangeFrame& freeFrame = _freeFrames[index];

        frame.offset = freeFrame.offset;
        frame.size = size;
        frame.wasHole = !wasLastFrame;

        freeFrame.offset += size;
        freeFrame.size -= size;

        if (freeFrame.size == 0)
            _freeFrames.erase(_freeFrames.begin() + index);

        if (wasLastFrame)
        {
            _allocatedBytes += size;
        }

        return true;
    }

    return false;
}

bool BufferRangeAllocator::Allocate(size_t size, size_t alignment, BufferRangeFrame& frame)
{
    if (size == 0)
        return false;

    size_t index = std::numeric_limits<size_t>().max();
    bool wasLastFrame = false;

    for (size_t i = 0; i < _freeFrames.size(); i++)
    {
        BufferRangeFrame& freeFrame = _freeFrames[i];

        size_t alignmentOffset = (alignment - (freeFrame.offset % alignment)) % alignment;
        size_t alignedSize = size + alignmentOffset;

        if (alignedSize <= freeFrame.size)
        {
            index = i;
            wasLastFrame = i == _freeFrames.size() - 1;
            break;
        }
    }

    if (index != std::numeric_limits<size_t>().max())
    {
        BufferRangeFrame& freeFrame = _freeFrames[index];

        size_t alignmentOffset = (alignment - (freeFrame.offset % alignment)) % alignment;
        size_t alignedSize = size + alignmentOffset;

        frame.offset = freeFrame.offset + alignmentOffset;
        frame.size = size;

        freeFrame.offset += alignedSize;
        freeFrame.size -= alignedSize;

        if (freeFrame.size == 0)
            _freeFrames.erase(_freeFrames.begin() + index);

        if (frame.offset % alignment != 0)
        {
            //NC_LOG_CRITICAL("Did not allocate aligned");
        }

        if (wasLastFrame)
        {
            _allocatedBytes += size;
        }

        return true;
    }

    return false;
}

bool BufferRangeAllocator::Free(const BufferRangeFrame& frame)
{
    BufferRangeFrame& newFreeFrame = _freeFrames.emplace_back();
    newFreeFrame = frame;

    TryMergeFrames();

    //_allocatedBytes -= frame.size; // This looks very sus to me so I disabled it for now... If we're using this to for example loop over data it would be bad

    return true;
}

bool BufferRangeAllocator::Grow(size_t newSize)
{
    size_t oldSize = _currentSize;
    if (newSize < oldSize)
        return false;

    size_t deltaSize = newSize - oldSize;

    BufferRangeFrame& frame = _freeFrames.emplace_back();
    frame.size = deltaSize;
    frame.offset = oldSize;

    _currentSize = newSize;

    TryMergeFrames();

    return true;
}

void BufferRangeAllocator::TryMergeFrames()
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

    // Sort the free frames by offset
    std::sort(_freeFrames.begin(), _freeFrames.end(), [](const BufferRangeFrame& a, const BufferRangeFrame& b) { return a.offset < b.offset; });
}

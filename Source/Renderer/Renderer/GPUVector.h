#pragma once
#include "Renderer.h"
#include "RenderSettings.h"
#include "CommandList.h"
#include "Descriptors/BufferDesc.h"

#include <Base/Platform.h>
#include <Base/Memory/BufferRangeAllocator.h>
#include <Base/Util/DebugHandler.h>

#include <shared_mutex>

namespace Renderer
{
    // This is a combined Vector<T> with a backing GPU Buffer and BufferRangeAllocator keeping track of the offsets of the GPU Buffer
    // T is the type of the elements in the vector
    // ELEMENTS_PER_BLOCK is the number of elements per block in the allocator, defaults to 8
    template <typename T, u32 ELEMENTS_PER_BLOCK = 8>
    class GPUVector
    {
        struct Region
        {
            size_t offset;
            size_t size;
        };

    public:
        static const u32 ELEMENT_SIZE = sizeof(T);

        // Constructor, pass true if you want to validate transfers to the GPU (SLOW, ONLY FOR DEBUGGING)
        GPUVector(bool validateTransfers = false)
            : _validateTransfers(validateTransfers)
        {
            _allocator.Init(0, 0);
        }

        // Non-const [] operator (allows modification)
        T& operator[](u32 index)
        {
            u32 offset = index * ELEMENT_SIZE;
            return *reinterpret_cast<T*>(&_data[offset]);
        }

        // Const [] operator (prevents modification)
        const T& operator[](u32 index) const
        {
            u32 offset = index * ELEMENT_SIZE;
            return *reinterpret_cast<T*>(&_data[offset]);
        }

        // Iterator support
        T* begin()
        {
            return reinterpret_cast<T*>(_data);
        }

        T* end()
        {
            return reinterpret_cast<T*>(_data) + Count();
        }

        const T* begin() const
        {
            return reinterpret_cast<const T*>(_data);
        }

        const T* end() const
        {
            return reinterpret_cast<const T*>(_data) + Count();
        }

        // Adds a default initialized element
        // Returns the index of the added element
        inline u32 Add()
        {
            return Add(T());
        }

        // Adds an element
        // Returns the index of the added element
        inline u32 Add(const T& element)
        {
            BufferRangeFrame frame;
            if (!_allocator.Allocate(ELEMENT_SIZE, frame))
            {
                // Did not have room, so lets grow
                size_t newSize = _allocator.Size() + (ELEMENT_SIZE * ELEMENTS_PER_BLOCK);
                _allocator.Grow(newSize);

                // Try again
                if (!_allocator.Allocate(ELEMENT_SIZE, frame))
                {
                    NC_LOG_ERROR("Failed to allocate memory for GPUVector");
                    return -1;
                }

                // Had to grow, so lets reallocate our data
                u8* oldData = _data;
                _data = new u8[_allocator.Size()];
                if (oldData)
                {
                    memcpy(_data, oldData, frame.offset);
                    delete[] oldData;
                }
            }

            memcpy(&_data[frame.offset], &element, frame.size);
            return frame.offset / ELEMENT_SIZE;
        }

        // Removes an element at the specified index
        // Leaves a hole in the data, you need to .Compress() the data to remove the hole
        // Also doesn't decrease .Count() until compressed
        inline void Remove(u32 index)
        {
            BufferRangeFrame frame;
            frame.offset = index * ELEMENT_SIZE;
            frame.size = ELEMENT_SIZE;
            _allocator.Free(frame);

            _isCompressed = false;
        }

        // Removes elements at the specified index and count
        // Leaves a hole in the data, you need to .Compress() the data to remove the hole
        // Also doesn't decrease .Count() until compressed
        inline void Remove(u32 index, u32 count)
        {
            BufferRangeFrame frame;
            frame.offset = index * ELEMENT_SIZE;
            frame.size = count * ELEMENT_SIZE;
            _allocator.Free(frame);

            _isCompressed = false;
        }

        // Clears the vector
        inline void Clear()
        {
            size_t size = _allocator.Size();
            _allocator.Reset();
            _allocator.Grow(size);
        }

        // Returns the count
        inline u32 Count() const
        {
            return static_cast<u32>(_allocator.AllocatedBytes() / ELEMENT_SIZE);
        }

        // Returns true if the vector is empty
        inline bool IsEmpty() const
        {
            return Count() == 0;
        }

        // Returns the capacity
        inline u32 Capacity() const
        {
            return static_cast<u32>(_allocator.Size() / ELEMENT_SIZE);
        }

        // Returns the count size in bytes
        inline u32 CountByteSize() const
        {
            return static_cast<u32>(_allocator.AllocatedBytes());
        }

        // Returns the capacity size in bytes
        inline u32 CapacityByteSize() const
        {
            return static_cast<u32>(_allocator.Size());
        }

        // This compresses the vector so there are no free spaces between elements
        void Compress()
        {
            // If we are already compressed, there is nothing to do
            if (_isCompressed)
            {
                return;
            }

            // Merge the free frames
            _allocator.TryMergeFrames();

            std::vector<BufferRangeFrame>& freeFrames = _allocator.GetFreeFrames();
            if (freeFrames.empty())
            {
                return;
            }

            u32 lastDataOffset = _allocator.AllocatedBytes();
            u32 dataMovedSize = 0;

            for (BufferRangeFrame& frame : freeFrames)
            {
                // If we are past the last data offset, we are done
                if (frame.offset >= lastDataOffset)
                {
                    break;
                }

                u32 freeRegionStart = frame.offset - dataMovedSize;
                u32 freeRegionEnd = freeRegionStart + frame.size;

                u32 sizeOfDataBehindFrame = lastDataOffset - freeRegionEnd;

                // memmove all data right of this frame left to fill the free region
                memmove(&_data[freeRegionStart], &_data[freeRegionEnd], sizeOfDataBehindFrame);

                dataMovedSize += frame.size;
            }

            // Clear the free frames
            freeFrames.clear();

            size_t newLastDataOffset = lastDataOffset - dataMovedSize;

            size_t allocatorSize = _allocator.Size();
            size_t freeSize = allocatorSize - newLastDataOffset;

            // Reinitialize the allocator
            _allocator.Init(newLastDataOffset, allocatorSize, freeSize);

#if NC_DEBUG
            // Memset the free region to 0xFF to mark the uninitialized memory
            memset(&_data[newLastDataOffset], 0xFF, freeSize);
#endif

            // Set the compressed flag
            _isCompressed = true;
        }

    private:

        bool _gpuInitialized = false;
        bool _validateTransfers = false;
        bool _isCompressed = true;

        Renderer* _renderer = nullptr;

        BufferRangeAllocator _allocator;

        u8* _data = nullptr;
    };
}
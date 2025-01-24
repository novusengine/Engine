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
    // SORT_STRATEGY is the strategy to sort the free frames in the GPU buffer, defaults to SizeAscending
    template <typename T, u32 ELEMENTS_PER_BLOCK = 8, BufferRangeSortStrategy SORT_STRATEGY = BufferRangeSortStrategy::SizeAscending, BufferRangeAllocationStrategy ALLOCATION_STRATEGY = BufferRangeAllocationStrategy::ReuseFirst>
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
        T& operator[](u32 index) const
        {
            u32 offset = index * ELEMENT_SIZE;
            return *reinterpret_cast<T*>(&_data[offset]);
        }

        // Adds a default initialized element
        // Returns the index of the added element
        inline u32 Add()
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

            // Default initialize the new element
            new (_data + frame.offset) T();

            // We only want to add dirty regions on Add() if we're not adding to the end of the vector
            // Otherwise we get tons of these and it's gonna be slow
            if (frame.wasHole)
            {
                SetDirtyRegion(frame.offset, frame.size);
            }

            return static_cast<u32>(frame.offset) / ELEMENT_SIZE;
        }

        // Adds an element
        // Returns the index of the added element
        inline u32 Add(const T& element)
        {
            u32 index = Add();

            (*this)[index] = element;
            return index;
        }

        // Adds a count of default initialized elements
        inline u32 AddCount(u32 count)
        {
            if (count == 0)
                return static_cast<u32>(_allocator.Size()) / ELEMENT_SIZE;

            BufferRangeFrame frame;
            if (!_allocator.Allocate(count * ELEMENT_SIZE, frame))
            {
                // Did not have room, so lets grow
                size_t requiredBytes = count * ELEMENT_SIZE;
                size_t blockBytes = ELEMENTS_PER_BLOCK * ELEMENT_SIZE;
                size_t growBytes = ((requiredBytes + blockBytes - 1) / blockBytes) * blockBytes;
                size_t newSize = _allocator.Size() + growBytes;
                _allocator.Grow(newSize);

                // Try again
                if (!_allocator.Allocate(count * ELEMENT_SIZE, frame))
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

            for (u32 i = 0; i < count; i++)
            {
                new (_data + frame.offset + (i * ELEMENT_SIZE)) T();
            }

            // We only want to add dirty regions on Add() if we're not adding to the end of the vector
            // Otherwise we get tons of these and it's gonna be slow
            if (frame.wasHole)
            {
                SetDirtyRegion(frame.offset, frame.size);
            }

            return static_cast<u32>(frame.offset) / ELEMENT_SIZE;
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

        // Increases capacity but not size
        inline void Reserve(u32 count)
        {
            size_t allocated = _allocator.AllocatedBytes();
            size_t requestedSize = allocated + (count * ELEMENT_SIZE);

            if (_allocator.Size() > requestedSize)
                return;

            size_t requiredBytes = count * ELEMENT_SIZE;
            size_t blockBytes = ELEMENTS_PER_BLOCK * ELEMENT_SIZE;
            size_t growBytes = ((requiredBytes + blockBytes - 1) / blockBytes) * blockBytes;
            size_t newSize = _allocator.Size() + growBytes;
            _allocator.Grow(newSize);

            // Had to grow, so lets reallocate our data
            u8* oldData = _data;
            _data = new u8[_allocator.Size()];
            if (oldData)
            {
                memcpy(_data, oldData, allocated);
                delete[] oldData;
            }
        }

        // Clears the vector
        inline void Clear()
        {
            size_t size = _allocator.Size();
            _allocator.Reset();
            _allocator.Grow(size);

            _gpuBufferUsedBytes = 0;
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
        inline u32 UsedBytes() const
        {
            return static_cast<u32>(_allocator.AllocatedBytes());
        }

        // Returns the capacity size in bytes
        inline u32 TotalBytes() const
        {
            return static_cast<u32>(_allocator.Size());
        }

        // Returns if the vector is compressed so it has no gaps
        inline bool IsCompressed() const
        {
            return _isCompressed;
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

            u32 lastDataOffset = static_cast<u32>(_allocator.AllocatedBytes());
            u32 dataMovedSize = 0;

            // Flag everything behind the first free frame as dirty
            SetDirtyRegion(freeFrames[0].offset, lastDataOffset - freeFrames[0].offset);

            for (BufferRangeFrame& frame : freeFrames)
            {
                // If we are past the last data offset, we are done
                if (frame.offset >= lastDataOffset)
                {
                    break;
                }

                u32 freeRegionStart = static_cast<u32>(frame.offset) - dataMovedSize;
                u32 freeRegionEnd = freeRegionStart + static_cast<u32>(frame.size);

                u32 sizeOfDataBehindFrame = lastDataOffset - freeRegionEnd;

                // memmove all data right of this frame left to fill the free region
                memmove(&_data[freeRegionStart], &_data[freeRegionEnd], sizeOfDataBehindFrame);

                dataMovedSize += static_cast<u32>(frame.size);
            }

            // Clear the free frames
            freeFrames.clear();

            size_t newLastDataOffset = lastDataOffset - dataMovedSize;

            size_t allocatorSize = _allocator.Size();
            size_t freeSize = allocatorSize - newLastDataOffset;

            // Reinitialize the allocator
            _allocator.Init(newLastDataOffset, allocatorSize);
            _gpuBufferUsedBytes = std::min(_gpuBufferUsedBytes, static_cast<u32>(newLastDataOffset));

#if NC_DEBUG
            // Memset the free region to 0xFF to mark the uninitialized memory
            memset(&_data[newLastDataOffset], 0xFF, freeSize);
#endif

            // Set the compressed flag
            _isCompressed = true;
        }

        inline bool IsDirty() const { return _dirtyRegions.size() > 0; }

        void SetDirtyRegion(size_t offset, size_t size)
        {
            if (size == 0)
            {
                NC_LOG_CRITICAL("GPUVector::SetDirtyRegion: Size is 0, this is not allowed!");
                return;
            }

            size_t allocatedBytes = _allocator.AllocatedBytes();
            if (offset >= allocatedBytes)
                return;

            Region& dirtyRegion = _dirtyRegions.emplace_back();

            dirtyRegion.offset = offset;
            dirtyRegion.size = size;
        }

        void SetDirtyElement(size_t elementIndex)
        {
            SetDirtyRegion(elementIndex * ELEMENT_SIZE, ELEMENT_SIZE);
        }

        void SetDirtyElements(size_t startIndex, size_t count)
        {
            SetDirtyRegion(startIndex * ELEMENT_SIZE, count * ELEMENT_SIZE);
        }

        void SetDirty()
        {
            size_t size = _allocator.AllocatedBytes();
            if (size == 0)
                return;

            SetDirtyRegion(0, size);
        }

        // Sets the debug name used for GPU debugging
        void SetDebugName(const std::string& debugName)
        {
            _debugName = debugName;
        }

        // Sets the GPU usage
        void SetUsage(u8 usage)
        {
            _usage = usage;
        }
        // Returns if the gpu buffer is valid
        bool IsValid() const { return _gpuBuffer != BufferID::Invalid(); }
        // Returns the gpu buffer
        BufferID GetBuffer() const { return _gpuBuffer; }

        // Returns true if we had to resize the GPU buffer
        bool SyncToGPU(Renderer* renderer)
        {
            u32 cpuUsedBytes = UsedBytes();
            u32 cpuTotalBytes = TotalBytes();

            bool needsResize = cpuUsedBytes > _gpuBufferTotalBytes;
            if (needsResize)
            {
                // Resize the buffer, this will copy what existed in the old buffer to the new buffer
                ResizeBuffer(renderer, cpuTotalBytes, true);
                _queuedValidation = _validateTransfers;
            }

            bool needsNewUpload = cpuUsedBytes > _gpuBufferUsedBytes;
            if (needsNewUpload)
            {
                // Create a dirty region for the new data
                Region dirtyRegion;
                dirtyRegion.offset = _gpuBufferUsedBytes;
                dirtyRegion.size = cpuUsedBytes - _gpuBufferUsedBytes;
                _dirtyRegions.push_back(dirtyRegion);
                
                _gpuBufferUsedBytes = cpuUsedBytes;
                _queuedValidation = _validateTransfers;
            }

            bool needsDirtyUpload = _dirtyRegions.size() > 0;
            if (needsDirtyUpload)
            {
                UploadDirtyRegions(renderer);
                _queuedValidation = _validateTransfers;
            }

            bool needsDefaultInit = _gpuBuffer == BufferID::Invalid();
            if (needsDefaultInit)
            {
                // Create the buffer if it doesn't exist
                ResizeBuffer(renderer, 1, false);
            }

            // If nothing changed in the buffer this frame we can validate it
            bool canValidate = !needsResize && !needsNewUpload && !needsDirtyUpload && !needsDefaultInit;
            if (_queuedValidation && canValidate)
            {
                Validate();
                _queuedValidation = false;
            }

            return needsResize || needsDefaultInit;
        }

        void SetValidation(bool shouldValidate) { _validateTransfers = shouldValidate; }

        bool Validate()
        {
            // Create (or update) the buffer
            BufferDesc validationDesc;
            validationDesc.name = _debugName + " Validation Buffer";
            validationDesc.size = UsedBytes();
            validationDesc.usage = BufferUsage::TRANSFER_DESTINATION;
            validationDesc.cpuAccess = BufferCPUAccess::ReadOnly;
            _validationBuffer = _renderer->CreateBuffer(_validationBuffer, validationDesc);

            NC_LOG_WARNING("Validating buffer {} ({} items, {} bytes)", _debugName.c_str(), Count(), validationDesc.size);

            // Immediately copy from the GPU buffer to the validation buffer
            _renderer->CopyBufferImmediate(_validationBuffer, 0, _gpuBuffer, 0, validationDesc.size);

            // Map the validation buffer and check for differences
            const T* validationData = reinterpret_cast<const T*>(_renderer->MapBuffer(_validationBuffer));

            int result = memcmp(_data, validationData, validationDesc.size);
            if (result != 0)
            {
                NC_LOG_ERROR("Validation failed for buffer {} ({} items, {} bytes)", _debugName.c_str(), Count(), validationDesc.size);
                _renderer->UnmapBuffer(_validationBuffer);
                return false;
            }
            _renderer->UnmapBuffer(_validationBuffer);

            return true;
        }
    private:
        void ResizeBuffer(Renderer* renderer, size_t newSize, bool copyOld)
        {
            BufferDesc desc;
            desc.name = _debugName;
            desc.size = newSize;
            desc.usage = _usage | BufferUsage::TRANSFER_SOURCE | BufferUsage::TRANSFER_DESTINATION;

            BufferID newBuffer = renderer->CreateBuffer(desc);

            if (_gpuBuffer != BufferID::Invalid())
            {
                if (copyOld)
                {
                    if (_gpuBufferTotalBytes > 0)
                    {
                        renderer->CopyBuffer(newBuffer, 0, _gpuBuffer, 0, _gpuBufferTotalBytes);
                    }
                }
                renderer->QueueDestroyBuffer(_gpuBuffer);
            }

            _gpuBufferTotalBytes = static_cast<u32>(newSize);
            _gpuBuffer = newBuffer;
            _renderer = renderer;
        }

        bool UploadDirtyRegions(Renderer* renderer)
        {
            if (_dirtyRegions.size() == 0)
                return false;

            // Sort dirtyRegions by offset
            std::sort(_dirtyRegions.begin(), _dirtyRegions.end(), [](const Region& a, const Region& b)
            {
                return a.offset < b.offset;
            });

            u32 numDirtyRegions = static_cast<u32>(_dirtyRegions.size());

            std::vector<u32> regionIndexToRemove;
            regionIndexToRemove.reserve(numDirtyRegions);

            i64 lastRegionEnd = -1;
            u32 lastRegionIndex = 0;

            for (u32 i = 0; i < numDirtyRegions; i++)
            {
                Region& curRegion = _dirtyRegions[i];
                i64 curRegionEnd = static_cast<i64>(curRegion.offset + curRegion.size);

                // curRegionEnd exists completely inside of lastRegionEnd
                if (lastRegionEnd >= curRegionEnd)
                {
                    regionIndexToRemove.push_back(i);
                }
                // Partial Overlap / Merge detected
                else if (lastRegionEnd >= static_cast<i64>(curRegion.offset))
                {
                    Region& prevRegion = _dirtyRegions[lastRegionIndex];

                    i64 sizeDiff = curRegionEnd - lastRegionEnd;
                    prevRegion.size += sizeDiff;

                    regionIndexToRemove.push_back(i);
                }
                else
                {
                    lastRegionIndex = i;
                }

                lastRegionEnd = glm::max(lastRegionEnd, curRegionEnd);
            }

            i32 numDirtyRegionsToRemove = static_cast<i32>(regionIndexToRemove.size());
            for (i32 i = numDirtyRegionsToRemove - 1; i >= 0; i--)
            {
                u32 index = regionIndexToRemove[i];
                _dirtyRegions.erase(_dirtyRegions.begin() + index);
            }

            // Upload for all remaining dirtyRegions
            for (const Region& dirtyRegion : _dirtyRegions)
            {
                renderer->UploadToBuffer(_gpuBuffer, dirtyRegion.offset, _data, dirtyRegion.offset, dirtyRegion.size);
            }
            _dirtyRegions.clear();

            return true;
        }

    private:
        bool _validateTransfers = false;
        bool _queuedValidation = false;
        bool _isCompressed = true;

        Renderer* _renderer = nullptr;

        BufferRangeAllocator<SORT_STRATEGY, ALLOCATION_STRATEGY> _allocator;
        u8* _data = nullptr;

        std::string _debugName = "";
        u8 _usage = 0;
        u32 _gpuBufferUsedBytes = 0;
        u32 _gpuBufferTotalBytes = 0;
        BufferID _gpuBuffer;
        BufferID _validationBuffer;
        std::vector<Region> _dirtyRegions;
    };
}
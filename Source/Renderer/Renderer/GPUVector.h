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
    template <typename T>
    class GPUVector
    {
        struct DirtyRegion
        {
            size_t offset;
            size_t size;
        };

    public:
        void SetDirtyRegion(size_t offset, size_t size)
        {
            size_t allocatedBytes = _allocator.AllocatedBytes();
            if (offset >= allocatedBytes)
                return;

            DirtyRegion& dirtyRegion = _dirtyRegions.emplace_back();

            dirtyRegion.offset = offset;
            dirtyRegion.size = size;
        }

        void SetDirtyElement(size_t elementIndex)
        {
            SetDirtyRegion(elementIndex * sizeof(T), sizeof(T));
        }
        
        void SetDirtyElements(size_t startIndex, size_t count)
        {
            SetDirtyRegion(startIndex * sizeof(T), count * sizeof(T));
        }

        // Returns true if we had to resize the buffer
        bool SyncToGPU(Renderer* renderer)
        {
            size_t vectorByteSize = _vector.size() * sizeof(T);

            if (!_initialized)
            {
                _renderer = renderer;
                _allocator.Init(0, 0);
                _initialized = true;

                if (vectorByteSize == 0) // Not sure about this
                {
                    ResizeBuffer(renderer, 1, false);
                    return true;
                }
            }

            if (vectorByteSize == 0) // Not sure about this
            {
                return false;
            }

            size_t allocatedBytes = _allocator.AllocatedBytes();
            if (vectorByteSize == allocatedBytes)
            {
                // We don't need to resize the buffer, but we might have dirty regions that we need to update
                UpdateDirtyRegions(renderer);
                return false; 
            }
            size_t bufferByteSize = _allocator.Size();

            bool didResize = false;
            if (vectorByteSize > bufferByteSize)
            {
                ResizeBuffer(renderer, vectorByteSize, true); // This copies everything that was allocated in the old buffer to the new buffer
                bufferByteSize = _allocator.Size();
                didResize = true;
            }
            
            // Allocate and upload anything that has been added since last sync
            size_t bytesToAllocate = bufferByteSize - allocatedBytes;

            if (bytesToAllocate > 0)
            {
                BufferRangeFrame bufferRangeFrame;
                if (!_allocator.Allocate(bytesToAllocate, bufferRangeFrame))
                {
                    DebugHandler::PrintFatal("[GPUVector] : Failed to allocate GPU Vector %s", _debugName.c_str());
                }
            }

#ifdef NC_Debug
            for (u32 i = 0; i < _dirtyRegions.size(); i++)
            {
                if (_dirtyRegions[i].offset >= allocatedBytes)
                {
                    DebugHandler::PrintFatal("[GPUVector] : UploadToBuffer will attempt to update a region in the buffer that ALSO exists in UpdateDirtyRegions, this will cause data corruption.");
                }
            }
#endif

            // Upload everything between allocatedBytes and allocatedBytes+bytesToAllocate
            renderer->UploadToBuffer(_buffer, allocatedBytes, _vector.data(), allocatedBytes, bytesToAllocate);

            UpdateDirtyRegions(renderer);

            return didResize;
        }

        // Returns true if we had to resize the buffer
        bool ForceSyncToGPU(Renderer* renderer)
        {
            size_t vectorByteSize = _vector.size() * sizeof(T);

            if (vectorByteSize == 0) // Not sure about this
                return false;

            if (!_initialized)
            {
                _renderer = renderer;
                _allocator.Init(0, 0);
                _initialized = true;
            }

            size_t allocatedBytes = _allocator.AllocatedBytes();
            size_t bufferByteSize = _allocator.Size();

            bool didResize = false;
            if (vectorByteSize > bufferByteSize)
            {
                ResizeBuffer(renderer, vectorByteSize, false);
                bufferByteSize = _allocator.Size();
                didResize = true;
            }

            // Allocate the part of the buffer that wasn't allocated before
            size_t bytesToAllocate = bufferByteSize - allocatedBytes;

            if (bytesToAllocate > 0)
            {
                BufferRangeFrame bufferRangeFrame;
                if (!_allocator.Allocate(bytesToAllocate, bufferRangeFrame))
                {
                    DebugHandler::PrintFatal("[GPUVector] : Failed to allocate GPU Vector %s", _debugName.c_str());
                }
            }

            // Then upload the whole buffer
            renderer->UploadToBuffer(_buffer, 0, _vector.data(), 0, vectorByteSize);

            return didResize;
        }

        void SetDebugName(const std::string& debugName)
        {
            _debugName = debugName;
        }

        void SetUsage(u8 usage)
        {
            _usage = usage;
        }

        size_t Size() const
        {
            return _vector.size();
        }

        void Resize(size_t newSize)
        {
            _vector.resize(newSize);
        }

        void Grow(size_t growthSize)
        {
            size_t size = _vector.size();
            _vector.resize(size + growthSize);
        }

        void Reserve(size_t reserveSize)
        {
            _vector.reserve(reserveSize);
        }

        void Clear(bool shouldSync = true)
        {
            _vector.clear();

            if (shouldSync && _renderer != nullptr && _buffer != BufferID::Invalid())
            {
                _allocator.Init(0, 0);
                _renderer->QueueDestroyBuffer(_buffer);
                _buffer = BufferID::Invalid();
                _initialized = false;
            }
            else
            {
                size_t size = _allocator.Size();
                _allocator.Init(0, size);
            }

            _dirtyRegions.clear();
        }

        std::vector<T>& Get() { return _vector; }

        bool HasDirtyRegions() { return _dirtyRegions.Size() > 0; }
        bool IsValid() { return _buffer != BufferID::Invalid(); }

        BufferID GetBuffer() { return _buffer; }

    private:
        void ResizeBuffer(Renderer* renderer, size_t newSize, bool copyOld)
        {
            BufferDesc desc;
            desc.name = _debugName;
            desc.size = newSize;
            desc.usage = _usage | BufferUsage::TRANSFER_SOURCE | BufferUsage::TRANSFER_DESTINATION;

            BufferID newBuffer = renderer->CreateBuffer(desc);

            if (_buffer != BufferID::Invalid())
            {
                if (copyOld)
                {
                    size_t oldSize = _allocator.AllocatedBytes();
                    if (oldSize > 0)
                    {
                        renderer->CopyBuffer(newBuffer, 0, _buffer, 0, oldSize);
                    }
                }
                renderer->QueueDestroyBuffer(_buffer);
            }

            _allocator.Grow(newSize);
            _buffer = newBuffer;
        }

        void UpdateDirtyRegions(Renderer* renderer)
        {
            if (_dirtyRegions.size() == 0)
                return;

            // Sort dirtyRegions by offset
            std::sort(_dirtyRegions.begin(), _dirtyRegions.end(), [](const DirtyRegion& a, const DirtyRegion& b)
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
                DirtyRegion& curRegion = _dirtyRegions[i];
                i64 curRegionEnd = static_cast<i64>(curRegion.offset + curRegion.size);

                // curRegionEnd exists completely inside of lastRegionEnd
                if (lastRegionEnd >= curRegionEnd)
                {
                    regionIndexToRemove.push_back(i);
                }
                // Partial Overlap / Merge detected
                else if (lastRegionEnd >= static_cast<i64>(curRegion.offset))
                {
                    DirtyRegion& prevRegion = _dirtyRegions[lastRegionIndex];

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
            for (const DirtyRegion& dirtyRegion : _dirtyRegions)
            {
                renderer->UploadToBuffer(_buffer, dirtyRegion.offset, _vector.data(), dirtyRegion.offset, dirtyRegion.size);
            }

            _dirtyRegions.clear();
        }

        bool _initialized = false;
        Renderer* _renderer = nullptr;
        BufferID _buffer;
        BufferRangeAllocator _allocator;

        std::string _debugName = "";
        u8 _usage = 0;

        std::vector<DirtyRegion> _dirtyRegions;
        std::vector<T> _vector;
    };
}
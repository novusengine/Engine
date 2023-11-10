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
    template <typename T>
    class GPUBuffer
    {
    public:
        // Returns true if we had to resize the buffer
        bool SyncToGPU(Renderer* renderer)
        {
            if (_initialized && !_isDirty)
                return false;

            _isDirty = false;

            size_t byteSize = _size * sizeof(T);

            if (!_initialized)
            {
                _renderer = renderer;
                _initialized = true;

                if (byteSize == 0) // Not sure about this
                {
                    ResizeBuffer(renderer, 1, false);
                    return true;
                }
            }

            if (byteSize == 0) // Not sure about this
            {
                return false;
            }

            ResizeBuffer(renderer, byteSize, true);
            return true;
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
            return _size;
        }

        void Resize(size_t newSize)
        {
            size_t newSizeBytes = newSize * sizeof(T);

            _size = glm::max(_size, newSizeBytes);
            _isDirty = newSizeBytes > _oldSize;
        }

        void Grow(size_t growthSize)
        {
            _size += growthSize * sizeof(T);
            _isDirty = true;
        }

        void Clear(bool shouldSync = true)
        {
            if (shouldSync && _renderer != nullptr && _buffer != BufferID::Invalid())
            {
                _renderer->QueueDestroyBuffer(_buffer);
                _buffer = BufferID::Invalid();
                _initialized = false;
            }

            _oldSize = 0;
            _size = 0;
            _isDirty = false;
        }

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
                if (copyOld && _oldSize > 0)
                {
                    renderer->CopyBuffer(newBuffer, 0, _buffer, 0, _oldSize);
                }

                renderer->QueueDestroyBuffer(_buffer);
            }

            _buffer = newBuffer;
            _oldSize = _size;
            _size = newSize;
        }

        bool _initialized = false;
        bool _isDirty = false;
        Renderer* _renderer = nullptr;
        BufferID _buffer;

        std::string _debugName = "";
        u8 _usage = 0;

        size_t _size = 0;
        size_t _oldSize = 0;
    };
}

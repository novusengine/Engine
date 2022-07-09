#pragma once
#include "BufferDesc.h"

#include <Base/Types.h>

namespace Renderer
{
    namespace Backend 
    {
        class UploadBufferHandlerVK;
    }

    // Lets strong-typedef an ID type with the underlying type of u16
    STRONG_TYPEDEF(StagingBufferID, u16);

    struct UploadBuffer
    {
        void* mappedMemory;
        size_t size;
    };
}
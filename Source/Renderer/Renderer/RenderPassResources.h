#pragma once
#include <Base/Types.h>

#include <type_safe/strong_typedef.hpp>

namespace Renderer
{
    // We need to separate for example ImageID and ImageResource
    // We do this so we can enforce the user to register their resource usage through the RenderGraphBuilder
    STRONG_TYPEDEF(ImageResource, u16);
    STRONG_TYPEDEF(ImageMutableResource, u16);

    STRONG_TYPEDEF(DepthImageResource, u16);
    STRONG_TYPEDEF(DepthImageMutableResource, u16);

    STRONG_TYPEDEF(BufferResource, u16);
    STRONG_TYPEDEF(BufferMutableResource, u16);

    inline BufferResource ToBufferResource(BufferMutableResource resource)
    {
        return BufferResource(static_cast<BufferResource::type>(resource));
    }
}
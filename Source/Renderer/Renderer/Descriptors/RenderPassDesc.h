#pragma once
#include "ImageDesc.h"
#include "DepthImageDesc.h"
#include "Renderer/RenderStates.h"
#include "Renderer/RenderPassResources.h"

#include <Base/Types.h>

#include <type_safe/strong_typedef.hpp>

namespace Renderer
{
    struct RenderPassDesc
    {
        std::function<ImageID(ImageResource resource)> ResourceToImageID = nullptr;
        std::function<DepthImageID(DepthImageResource resource)> ResourceToDepthImageID = nullptr;
        std::function<ImageID(ImageMutableResource resource)> MutableResourceToImageID = nullptr;
        std::function<DepthImageID(DepthImageMutableResource resource)> MutableResourceToDepthImageID = nullptr;

        // Rendertargets
        ivec2 offset = ivec2(0);
        uvec2 extent = uvec2(0, 0);
        ImageMutableResource renderTargets[MAX_RENDER_TARGETS];
        DepthImageMutableResource depthStencil = DepthImageMutableResource::Invalid();
    };
}
#pragma once

#include <Base/Types.h>

namespace Renderer
{
    struct MeshShaderProperties
    {
        bool meshShaderSupported = false;
        bool taskShaderSupported = false;

        u32 maxOutputVertices = 0;
        u32 maxOutputPrimitives = 0;
        u32 maxWorkGroupInvocations = 0;
        uvec3 maxWorkGroupSize = uvec3(0);
        uvec3 maxWorkGroupCount = uvec3(0);
        u32 preferredWorkGroupInvocations = 0;
    };
}

#pragma once
#include <Renderer/Descriptors/BufferDesc.h>
#include <Renderer/Descriptors/DepthImageDesc.h>
#include <Renderer/Descriptors/ImageDesc.h>

#include <Base/Types.h>

struct FfxCacaoContext;

namespace Renderer
{
    namespace Commands
    {
        struct DispatchCacao
        {
            static const BackendDispatchFunction DISPATCH_FUNCTION;

            FfxCacaoContext* context = nullptr;
            DepthImageID depthImage = DepthImageID::Invalid();
            ImageID normalImage = ImageID::Invalid();
            ImageID outputImage = ImageID::Invalid();
            mat4x4* proj = nullptr;
            mat4x4* normalsToView = nullptr;
            f32 normalUnpackMul;
            f32 normalUnpackAdd;
        };
    }
}
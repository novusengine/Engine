#pragma once
#include <Renderer/Descriptors/ImageDesc.h>
#include <Renderer/Descriptors/DepthImageDesc.h>

#include <Base/Types.h>

#include <FidelityFX/host/ffx_error.h>
#include <FidelityFX/host/ffx_interface.h>
#include <FidelityFX/host/ffx_types.h>

#include <vulkan/vulkan.h>

namespace Renderer
{
    namespace Backend
    {
        class RenderDeviceVK;
        class ImageHandlerVK;

        FfxUInt32 GetSDKVersionVK(FfxInterface* backendInterface);
        FfxErrorCode CreateBackendContextVK(FfxInterface* backendInterface, FfxUInt32* effectContextId);
        FfxErrorCode GetDeviceCapabilitiesVK(FfxInterface* backendInterface, FfxDeviceCapabilities* deviceCapabilities);
        FfxErrorCode DestroyBackendContextVK(FfxInterface* backendInterface, FfxUInt32 effectContextId);
        FfxErrorCode CreateResourceVK(FfxInterface* backendInterface, const FfxCreateResourceDescription* desc, FfxUInt32 effectContextId, FfxResourceInternal* outTexture);
        FfxErrorCode DestroyResourceVK(FfxInterface* backendInterface, FfxResourceInternal resource);
        FfxErrorCode RegisterResourceVK(FfxInterface* backendInterface, const FfxResource* inResource, FfxUInt32 effectContextId, FfxResourceInternal* outResourceInternal);
        FfxResource GetResourceVK(FfxInterface* backendInterface, FfxResourceInternal resource);
        FfxErrorCode UnregisterResourcesVK(FfxInterface* backendInterface, FfxCommandList commandList, FfxUInt32 effectContextId);
        FfxResourceDescription GetResourceDescriptionVK(FfxInterface* backendInterface, FfxResourceInternal resource);
        FfxErrorCode CreatePipelineVK(FfxInterface* backendInterface, FfxEffect effect, FfxPass passId, uint32_t permutationOptions, const FfxPipelineDescription* desc, FfxUInt32 effectContextId, FfxPipelineState* outPass);
        FfxErrorCode DestroyPipelineVK(FfxInterface* backendInterface, FfxPipelineState* pipeline, FfxUInt32 effectContextId);
        FfxErrorCode ScheduleGpuJobVK(FfxInterface* backendInterface, const FfxGpuJobDescription* job);
        FfxErrorCode ExecuteGpuJobsVK(FfxInterface* backendInterface, FfxCommandList commandList);

        class FidelityFXHandlerVK
        {
        public:
            void Init(RenderDeviceVK* device, ImageHandlerVK* imageHandler);

            size_t ffxGetScratchMemorySize(size_t maxContexts);
            FfxResource ffxGetResource(ImageID imageID, FfxResourceStates state);
            FfxResource ffxGetResource(DepthImageID imageID, FfxResourceStates state);

            // prototypes for functions in the interface
            FfxUInt32 GetSDKVersionVK(FfxInterface* backendInterface);
            FfxErrorCode CreateBackendContextVK(FfxInterface* backendInterface, FfxUInt32* effectContextId);
            FfxErrorCode GetDeviceCapabilitiesVK(FfxInterface* backendInterface, FfxDeviceCapabilities* deviceCapabilities);
            FfxErrorCode DestroyBackendContextVK(FfxInterface* backendInterface, FfxUInt32 effectContextId);
            FfxErrorCode CreateResourceVK(FfxInterface* backendInterface, const FfxCreateResourceDescription* desc, FfxUInt32 effectContextId, FfxResourceInternal* outTexture);
            FfxErrorCode DestroyResourceVK(FfxInterface* backendInterface, FfxResourceInternal resource);
            FfxErrorCode RegisterResourceVK(FfxInterface* backendInterface, const FfxResource* inResource, FfxUInt32 effectContextId, FfxResourceInternal* outResourceInternal);
            FfxResource GetResourceVK(FfxInterface* backendInterface, FfxResourceInternal resource);
            FfxErrorCode UnregisterResourcesVK(FfxInterface* backendInterface, FfxCommandList commandList, FfxUInt32 effectContextId);
            FfxResourceDescription GetResourceDescriptionVK(FfxInterface* backendInterface, FfxResourceInternal resource);
            FfxErrorCode CreatePipelineVK(FfxInterface* backendInterface, FfxEffect effect, FfxPass passId, uint32_t permutationOptions, const FfxPipelineDescription* desc, FfxUInt32 effectContextId, FfxPipelineState* outPass);
            FfxErrorCode DestroyPipelineVK(FfxInterface* backendInterface, FfxPipelineState* pipeline, FfxUInt32 effectContextId);
            FfxErrorCode ScheduleGpuJobVK(FfxInterface* backendInterface, const FfxGpuJobDescription* job);
            FfxErrorCode ExecuteGpuJobsVK(FfxInterface* backendInterface, FfxCommandList commandList);

        private:

        private:
            RenderDeviceVK* _device = nullptr;
            ImageHandlerVK* _imageHandler = nullptr;
            
            u32 _backendRefCount = 0;
            PFN_vkGetDeviceProcAddr _vkDeviceProcAddr;   /// The device's function address table
        };

        
    }
}
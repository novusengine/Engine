#include "ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c.h"

typedef union ffx_denoiser_prefilter_reflections_pass_wave64_16bit_PermutationKey {
    struct {
        uint32_t FFX_DENOISER_OPTION_INVERTED_DEPTH : 1;
    };
    uint32_t index;
} ffx_denoiser_prefilter_reflections_pass_wave64_16bit_PermutationKey;

typedef struct ffx_denoiser_prefilter_reflections_pass_wave64_16bit_PermutationInfo {
    const uint32_t       blobSize;
    const unsigned char* blobData;


    const uint32_t  numConstantBuffers;
    const char**    constantBufferNames;
    const uint32_t* constantBufferBindings;
    const uint32_t* constantBufferCounts;
    const uint32_t* constantBufferSpaces;

    const uint32_t  numSRVTextures;
    const char**    srvTextureNames;
    const uint32_t* srvTextureBindings;
    const uint32_t* srvTextureCounts;
    const uint32_t* srvTextureSpaces;

    const uint32_t  numUAVTextures;
    const char**    uavTextureNames;
    const uint32_t* uavTextureBindings;
    const uint32_t* uavTextureCounts;
    const uint32_t* uavTextureSpaces;

    const uint32_t  numSRVBuffers;
    const char**    srvBufferNames;
    const uint32_t* srvBufferBindings;
    const uint32_t* srvBufferCounts;
    const uint32_t* srvBufferSpaces;

    const uint32_t  numUAVBuffers;
    const char**    uavBufferNames;
    const uint32_t* uavBufferBindings;
    const uint32_t* uavBufferCounts;
    const uint32_t* uavBufferSpaces;

    const uint32_t  numSamplers;
    const char**    samplerNames;
    const uint32_t* samplerBindings;
    const uint32_t* samplerCounts;
    const uint32_t* samplerSpaces;

    const uint32_t  numRTAccelerationStructures;
    const char**    rtAccelerationStructureNames;
    const uint32_t* rtAccelerationStructureBindings;
    const uint32_t* rtAccelerationStructureCounts;
    const uint32_t* rtAccelerationStructureSpaces;
} ffx_denoiser_prefilter_reflections_pass_wave64_16bit_PermutationInfo;

static const uint32_t g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_IndirectionTable[] = {
    0,
    0,
};

static const ffx_denoiser_prefilter_reflections_pass_wave64_16bit_PermutationInfo g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_PermutationInfo[] = {
    { g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_size, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_data, 1, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_CBVResourceNames, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_CBVResourceBindings, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_CBVResourceCounts, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_CBVResourceSets, 6, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_TextureSRVResourceNames, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_TextureSRVResourceBindings, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_TextureSRVResourceCounts, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_TextureSRVResourceSets, 2, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_TextureUAVResourceNames, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_TextureUAVResourceBindings, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_TextureUAVResourceCounts, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_TextureUAVResourceSets, 0, 0, 0, 0, 0, 1, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_BufferUAVResourceNames, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_BufferUAVResourceBindings, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_BufferUAVResourceCounts, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_BufferUAVResourceSets, 1, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_SamplerResourceNames, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_SamplerResourceBindings, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_SamplerResourceCounts, g_ffx_denoiser_prefilter_reflections_pass_wave64_16bit_a38b192e9c59ad8bd5a8cd5800a8bb7c_SamplerResourceSets, 0, 0, 0, 0, 0, },
};


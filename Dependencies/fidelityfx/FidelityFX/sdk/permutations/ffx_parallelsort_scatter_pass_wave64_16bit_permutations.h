#include "ffx_parallelsort_scatter_pass_wave64_16bit_3eb20bc07c6cb4505fe95a1fbc2c783d.h"
#include "ffx_parallelsort_scatter_pass_wave64_16bit_64a1fb90eca00bfcc521a10c7b0bb9fa.h"

typedef union ffx_parallelsort_scatter_pass_wave64_16bit_PermutationKey {
    struct {
        uint32_t FFX_PARALLELSORT_OPTION_HAS_PAYLOAD : 1;
    };
    uint32_t index;
} ffx_parallelsort_scatter_pass_wave64_16bit_PermutationKey;

typedef struct ffx_parallelsort_scatter_pass_wave64_16bit_PermutationInfo {
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
} ffx_parallelsort_scatter_pass_wave64_16bit_PermutationInfo;

static const uint32_t g_ffx_parallelsort_scatter_pass_wave64_16bit_IndirectionTable[] = {
    0,
    1,
};

static const ffx_parallelsort_scatter_pass_wave64_16bit_PermutationInfo g_ffx_parallelsort_scatter_pass_wave64_16bit_PermutationInfo[] = {
    { g_ffx_parallelsort_scatter_pass_wave64_16bit_3eb20bc07c6cb4505fe95a1fbc2c783d_size, g_ffx_parallelsort_scatter_pass_wave64_16bit_3eb20bc07c6cb4505fe95a1fbc2c783d_data, 1, g_ffx_parallelsort_scatter_pass_wave64_16bit_3eb20bc07c6cb4505fe95a1fbc2c783d_CBVResourceNames, g_ffx_parallelsort_scatter_pass_wave64_16bit_3eb20bc07c6cb4505fe95a1fbc2c783d_CBVResourceBindings, g_ffx_parallelsort_scatter_pass_wave64_16bit_3eb20bc07c6cb4505fe95a1fbc2c783d_CBVResourceCounts, g_ffx_parallelsort_scatter_pass_wave64_16bit_3eb20bc07c6cb4505fe95a1fbc2c783d_CBVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, g_ffx_parallelsort_scatter_pass_wave64_16bit_3eb20bc07c6cb4505fe95a1fbc2c783d_BufferUAVResourceNames, g_ffx_parallelsort_scatter_pass_wave64_16bit_3eb20bc07c6cb4505fe95a1fbc2c783d_BufferUAVResourceBindings, g_ffx_parallelsort_scatter_pass_wave64_16bit_3eb20bc07c6cb4505fe95a1fbc2c783d_BufferUAVResourceCounts, g_ffx_parallelsort_scatter_pass_wave64_16bit_3eb20bc07c6cb4505fe95a1fbc2c783d_BufferUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_parallelsort_scatter_pass_wave64_16bit_64a1fb90eca00bfcc521a10c7b0bb9fa_size, g_ffx_parallelsort_scatter_pass_wave64_16bit_64a1fb90eca00bfcc521a10c7b0bb9fa_data, 1, g_ffx_parallelsort_scatter_pass_wave64_16bit_64a1fb90eca00bfcc521a10c7b0bb9fa_CBVResourceNames, g_ffx_parallelsort_scatter_pass_wave64_16bit_64a1fb90eca00bfcc521a10c7b0bb9fa_CBVResourceBindings, g_ffx_parallelsort_scatter_pass_wave64_16bit_64a1fb90eca00bfcc521a10c7b0bb9fa_CBVResourceCounts, g_ffx_parallelsort_scatter_pass_wave64_16bit_64a1fb90eca00bfcc521a10c7b0bb9fa_CBVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, g_ffx_parallelsort_scatter_pass_wave64_16bit_64a1fb90eca00bfcc521a10c7b0bb9fa_BufferUAVResourceNames, g_ffx_parallelsort_scatter_pass_wave64_16bit_64a1fb90eca00bfcc521a10c7b0bb9fa_BufferUAVResourceBindings, g_ffx_parallelsort_scatter_pass_wave64_16bit_64a1fb90eca00bfcc521a10c7b0bb9fa_BufferUAVResourceCounts, g_ffx_parallelsort_scatter_pass_wave64_16bit_64a1fb90eca00bfcc521a10c7b0bb9fa_BufferUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};


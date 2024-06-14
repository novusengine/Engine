#include "ffx_parallelsort_reduce_pass_76fbf9d3d25f4ce0bf0267ac22fbbb51.h"

typedef union ffx_parallelsort_reduce_pass_PermutationKey {
    struct {
        uint32_t FFX_PARALLELSORT_OPTION_HAS_PAYLOAD : 1;
    };
    uint32_t index;
} ffx_parallelsort_reduce_pass_PermutationKey;

typedef struct ffx_parallelsort_reduce_pass_PermutationInfo {
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
} ffx_parallelsort_reduce_pass_PermutationInfo;

static const uint32_t g_ffx_parallelsort_reduce_pass_IndirectionTable[] = {
    0,
    0,
};

static const ffx_parallelsort_reduce_pass_PermutationInfo g_ffx_parallelsort_reduce_pass_PermutationInfo[] = {
    { g_ffx_parallelsort_reduce_pass_76fbf9d3d25f4ce0bf0267ac22fbbb51_size, g_ffx_parallelsort_reduce_pass_76fbf9d3d25f4ce0bf0267ac22fbbb51_data, 1, g_ffx_parallelsort_reduce_pass_76fbf9d3d25f4ce0bf0267ac22fbbb51_CBVResourceNames, g_ffx_parallelsort_reduce_pass_76fbf9d3d25f4ce0bf0267ac22fbbb51_CBVResourceBindings, g_ffx_parallelsort_reduce_pass_76fbf9d3d25f4ce0bf0267ac22fbbb51_CBVResourceCounts, g_ffx_parallelsort_reduce_pass_76fbf9d3d25f4ce0bf0267ac22fbbb51_CBVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, g_ffx_parallelsort_reduce_pass_76fbf9d3d25f4ce0bf0267ac22fbbb51_BufferUAVResourceNames, g_ffx_parallelsort_reduce_pass_76fbf9d3d25f4ce0bf0267ac22fbbb51_BufferUAVResourceBindings, g_ffx_parallelsort_reduce_pass_76fbf9d3d25f4ce0bf0267ac22fbbb51_BufferUAVResourceCounts, g_ffx_parallelsort_reduce_pass_76fbf9d3d25f4ce0bf0267ac22fbbb51_BufferUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};


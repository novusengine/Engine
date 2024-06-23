#include "ffx_fsr1_rcas_pass_wave64_22d4143bd1d9b43a963eafbb0ccb7870.h"
#include "ffx_fsr1_rcas_pass_wave64_383771747daa24356d9148d29cfbf350.h"

typedef union ffx_fsr1_rcas_pass_wave64_PermutationKey {
    struct {
        uint32_t FFX_FSR1_OPTION_APPLY_RCAS : 1;
        uint32_t FFX_FSR1_OPTION_RCAS_PASSTHROUGH_ALPHA : 1;
        uint32_t FFX_FSR1_OPTION_SRGB_CONVERSIONS : 1;
    };
    uint32_t index;
} ffx_fsr1_rcas_pass_wave64_PermutationKey;

typedef struct ffx_fsr1_rcas_pass_wave64_PermutationInfo {
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
} ffx_fsr1_rcas_pass_wave64_PermutationInfo;

static const uint32_t g_ffx_fsr1_rcas_pass_wave64_IndirectionTable[] = {
    1,
    1,
    0,
    0,
    1,
    1,
    0,
    0,
};

static const ffx_fsr1_rcas_pass_wave64_PermutationInfo g_ffx_fsr1_rcas_pass_wave64_PermutationInfo[] = {
    { g_ffx_fsr1_rcas_pass_wave64_22d4143bd1d9b43a963eafbb0ccb7870_size, g_ffx_fsr1_rcas_pass_wave64_22d4143bd1d9b43a963eafbb0ccb7870_data, 1, g_ffx_fsr1_rcas_pass_wave64_22d4143bd1d9b43a963eafbb0ccb7870_CBVResourceNames, g_ffx_fsr1_rcas_pass_wave64_22d4143bd1d9b43a963eafbb0ccb7870_CBVResourceBindings, g_ffx_fsr1_rcas_pass_wave64_22d4143bd1d9b43a963eafbb0ccb7870_CBVResourceCounts, g_ffx_fsr1_rcas_pass_wave64_22d4143bd1d9b43a963eafbb0ccb7870_CBVResourceSets, 1, g_ffx_fsr1_rcas_pass_wave64_22d4143bd1d9b43a963eafbb0ccb7870_TextureSRVResourceNames, g_ffx_fsr1_rcas_pass_wave64_22d4143bd1d9b43a963eafbb0ccb7870_TextureSRVResourceBindings, g_ffx_fsr1_rcas_pass_wave64_22d4143bd1d9b43a963eafbb0ccb7870_TextureSRVResourceCounts, g_ffx_fsr1_rcas_pass_wave64_22d4143bd1d9b43a963eafbb0ccb7870_TextureSRVResourceSets, 1, g_ffx_fsr1_rcas_pass_wave64_22d4143bd1d9b43a963eafbb0ccb7870_TextureUAVResourceNames, g_ffx_fsr1_rcas_pass_wave64_22d4143bd1d9b43a963eafbb0ccb7870_TextureUAVResourceBindings, g_ffx_fsr1_rcas_pass_wave64_22d4143bd1d9b43a963eafbb0ccb7870_TextureUAVResourceCounts, g_ffx_fsr1_rcas_pass_wave64_22d4143bd1d9b43a963eafbb0ccb7870_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_fsr1_rcas_pass_wave64_383771747daa24356d9148d29cfbf350_size, g_ffx_fsr1_rcas_pass_wave64_383771747daa24356d9148d29cfbf350_data, 1, g_ffx_fsr1_rcas_pass_wave64_383771747daa24356d9148d29cfbf350_CBVResourceNames, g_ffx_fsr1_rcas_pass_wave64_383771747daa24356d9148d29cfbf350_CBVResourceBindings, g_ffx_fsr1_rcas_pass_wave64_383771747daa24356d9148d29cfbf350_CBVResourceCounts, g_ffx_fsr1_rcas_pass_wave64_383771747daa24356d9148d29cfbf350_CBVResourceSets, 1, g_ffx_fsr1_rcas_pass_wave64_383771747daa24356d9148d29cfbf350_TextureSRVResourceNames, g_ffx_fsr1_rcas_pass_wave64_383771747daa24356d9148d29cfbf350_TextureSRVResourceBindings, g_ffx_fsr1_rcas_pass_wave64_383771747daa24356d9148d29cfbf350_TextureSRVResourceCounts, g_ffx_fsr1_rcas_pass_wave64_383771747daa24356d9148d29cfbf350_TextureSRVResourceSets, 1, g_ffx_fsr1_rcas_pass_wave64_383771747daa24356d9148d29cfbf350_TextureUAVResourceNames, g_ffx_fsr1_rcas_pass_wave64_383771747daa24356d9148d29cfbf350_TextureUAVResourceBindings, g_ffx_fsr1_rcas_pass_wave64_383771747daa24356d9148d29cfbf350_TextureUAVResourceCounts, g_ffx_fsr1_rcas_pass_wave64_383771747daa24356d9148d29cfbf350_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};


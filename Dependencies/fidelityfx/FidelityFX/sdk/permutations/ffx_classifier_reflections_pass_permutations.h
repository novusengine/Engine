#include "ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc.h"
#include "ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0.h"

typedef union ffx_classifier_reflections_pass_PermutationKey {
    struct {
        uint32_t FFX_CLASSIFIER_OPTION_INVERTED_DEPTH : 1;
        uint32_t FFX_CLASSIFIER_OPTION_CLASSIFIER_MODE : 1;
    };
    uint32_t index;
} ffx_classifier_reflections_pass_PermutationKey;

typedef struct ffx_classifier_reflections_pass_PermutationInfo {
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
} ffx_classifier_reflections_pass_PermutationInfo;

static const uint32_t g_ffx_classifier_reflections_pass_IndirectionTable[] = {
    1,
    0,
    1,
    0,
};

static const ffx_classifier_reflections_pass_PermutationInfo g_ffx_classifier_reflections_pass_PermutationInfo[] = {
    { g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_size, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_data, 1, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_CBVResourceNames, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_CBVResourceBindings, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_CBVResourceCounts, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_CBVResourceSets, 7, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_TextureSRVResourceNames, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_TextureSRVResourceBindings, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_TextureSRVResourceCounts, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_TextureSRVResourceSets, 3, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_TextureUAVResourceNames, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_TextureUAVResourceBindings, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_TextureUAVResourceCounts, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_TextureUAVResourceSets, 0, 0, 0, 0, 0, 4, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_BufferUAVResourceNames, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_BufferUAVResourceBindings, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_BufferUAVResourceCounts, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_BufferUAVResourceSets, 2, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_SamplerResourceNames, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_SamplerResourceBindings, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_SamplerResourceCounts, g_ffx_classifier_reflections_pass_72fedef5bf3e9aeacf58fa606e5e8bcc_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_size, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_data, 1, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_CBVResourceNames, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_CBVResourceBindings, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_CBVResourceCounts, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_CBVResourceSets, 7, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_TextureSRVResourceNames, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_TextureSRVResourceBindings, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_TextureSRVResourceCounts, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_TextureSRVResourceSets, 3, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_TextureUAVResourceNames, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_TextureUAVResourceBindings, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_TextureUAVResourceCounts, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_TextureUAVResourceSets, 0, 0, 0, 0, 0, 4, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_BufferUAVResourceNames, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_BufferUAVResourceBindings, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_BufferUAVResourceCounts, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_BufferUAVResourceSets, 2, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_SamplerResourceNames, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_SamplerResourceBindings, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_SamplerResourceCounts, g_ffx_classifier_reflections_pass_84272f7589d223922932ddceff5016b0_SamplerResourceSets, 0, 0, 0, 0, 0, },
};


#include "ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a.h"
#include "ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925.h"

typedef union ffx_dof_composite_pass_wave64_PermutationKey {
    struct {
        uint32_t FFX_DOF_OPTION_MAX_RING_MERGE_LOG : 1;
        uint32_t FFX_DOF_OPTION_COMBINE_IN_PLACE : 1;
        uint32_t FFX_DOF_OPTION_REVERSE_DEPTH : 1;
    };
    uint32_t index;
} ffx_dof_composite_pass_wave64_PermutationKey;

typedef struct ffx_dof_composite_pass_wave64_PermutationInfo {
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
} ffx_dof_composite_pass_wave64_PermutationInfo;

static const uint32_t g_ffx_dof_composite_pass_wave64_IndirectionTable[] = {
    1,
    1,
    0,
    0,
    1,
    1,
    0,
    0,
};

static const ffx_dof_composite_pass_wave64_PermutationInfo g_ffx_dof_composite_pass_wave64_PermutationInfo[] = {
    { g_ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a_size, g_ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a_data, 1, g_ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a_CBVResourceNames, g_ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a_CBVResourceBindings, g_ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a_CBVResourceCounts, g_ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a_CBVResourceSets, 2, g_ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a_TextureSRVResourceNames, g_ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a_TextureSRVResourceBindings, g_ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a_TextureSRVResourceCounts, g_ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a_TextureSRVResourceSets, 3, g_ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a_TextureUAVResourceNames, g_ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a_TextureUAVResourceBindings, g_ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a_TextureUAVResourceCounts, g_ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a_SamplerResourceNames, g_ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a_SamplerResourceBindings, g_ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a_SamplerResourceCounts, g_ffx_dof_composite_pass_wave64_fcff7ae56c85dc6f5bdc357092f31b6a_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925_size, g_ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925_data, 1, g_ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925_CBVResourceNames, g_ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925_CBVResourceBindings, g_ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925_CBVResourceCounts, g_ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925_CBVResourceSets, 3, g_ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925_TextureSRVResourceNames, g_ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925_TextureSRVResourceBindings, g_ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925_TextureSRVResourceCounts, g_ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925_TextureSRVResourceSets, 3, g_ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925_TextureUAVResourceNames, g_ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925_TextureUAVResourceBindings, g_ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925_TextureUAVResourceCounts, g_ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925_SamplerResourceNames, g_ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925_SamplerResourceBindings, g_ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925_SamplerResourceCounts, g_ffx_dof_composite_pass_wave64_788f9f4c5d669f22d2c0408ed740c925_SamplerResourceSets, 0, 0, 0, 0, 0, },
};


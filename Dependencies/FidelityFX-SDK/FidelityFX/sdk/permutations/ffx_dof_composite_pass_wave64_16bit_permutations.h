#include "ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54.h"
#include "ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8.h"

typedef union ffx_dof_composite_pass_wave64_16bit_PermutationKey {
    struct {
        uint32_t FFX_DOF_OPTION_MAX_RING_MERGE_LOG : 1;
        uint32_t FFX_DOF_OPTION_COMBINE_IN_PLACE : 1;
        uint32_t FFX_DOF_OPTION_REVERSE_DEPTH : 1;
    };
    uint32_t index;
} ffx_dof_composite_pass_wave64_16bit_PermutationKey;

typedef struct ffx_dof_composite_pass_wave64_16bit_PermutationInfo {
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
} ffx_dof_composite_pass_wave64_16bit_PermutationInfo;

static const uint32_t g_ffx_dof_composite_pass_wave64_16bit_IndirectionTable[] = {
    0,
    0,
    1,
    1,
    0,
    0,
    1,
    1,
};

static const ffx_dof_composite_pass_wave64_16bit_PermutationInfo g_ffx_dof_composite_pass_wave64_16bit_PermutationInfo[] = {
    { g_ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54_size, g_ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54_data, 1, g_ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54_CBVResourceNames, g_ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54_CBVResourceBindings, g_ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54_CBVResourceCounts, g_ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54_CBVResourceSets, 3, g_ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54_TextureSRVResourceNames, g_ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54_TextureSRVResourceBindings, g_ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54_TextureSRVResourceCounts, g_ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54_TextureSRVResourceSets, 3, g_ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54_TextureUAVResourceNames, g_ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54_TextureUAVResourceBindings, g_ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54_TextureUAVResourceCounts, g_ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54_SamplerResourceNames, g_ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54_SamplerResourceBindings, g_ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54_SamplerResourceCounts, g_ffx_dof_composite_pass_wave64_16bit_01dc4df92d672adecd7536683c417d54_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8_size, g_ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8_data, 1, g_ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8_CBVResourceNames, g_ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8_CBVResourceBindings, g_ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8_CBVResourceCounts, g_ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8_CBVResourceSets, 2, g_ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8_TextureSRVResourceNames, g_ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8_TextureSRVResourceBindings, g_ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8_TextureSRVResourceCounts, g_ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8_TextureSRVResourceSets, 3, g_ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8_TextureUAVResourceNames, g_ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8_TextureUAVResourceBindings, g_ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8_TextureUAVResourceCounts, g_ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8_SamplerResourceNames, g_ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8_SamplerResourceBindings, g_ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8_SamplerResourceCounts, g_ffx_dof_composite_pass_wave64_16bit_e0c0d5a537d38fc99a8db8fe267e15b8_SamplerResourceSets, 0, 0, 0, 0, 0, },
};


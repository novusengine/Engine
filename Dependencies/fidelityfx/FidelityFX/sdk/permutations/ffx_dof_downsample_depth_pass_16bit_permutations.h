#include "ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d.h"
#include "ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22.h"

typedef union ffx_dof_downsample_depth_pass_16bit_PermutationKey {
    struct {
        uint32_t FFX_DOF_OPTION_MAX_RING_MERGE_LOG : 1;
        uint32_t FFX_DOF_OPTION_COMBINE_IN_PLACE : 1;
        uint32_t FFX_DOF_OPTION_REVERSE_DEPTH : 1;
    };
    uint32_t index;
} ffx_dof_downsample_depth_pass_16bit_PermutationKey;

typedef struct ffx_dof_downsample_depth_pass_16bit_PermutationInfo {
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
} ffx_dof_downsample_depth_pass_16bit_PermutationInfo;

static const uint32_t g_ffx_dof_downsample_depth_pass_16bit_IndirectionTable[] = {
    0,
    0,
    0,
    0,
    1,
    1,
    1,
    1,
};

static const ffx_dof_downsample_depth_pass_16bit_PermutationInfo g_ffx_dof_downsample_depth_pass_16bit_PermutationInfo[] = {
    { g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_size, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_data, 1, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_CBVResourceNames, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_CBVResourceBindings, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_CBVResourceCounts, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_CBVResourceSets, 1, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_TextureSRVResourceNames, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_TextureSRVResourceBindings, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_TextureSRVResourceCounts, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_TextureSRVResourceSets, 1, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_TextureUAVResourceNames, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_TextureUAVResourceBindings, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_TextureUAVResourceCounts, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_TextureUAVResourceSets, 0, 0, 0, 0, 0, 1, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_BufferUAVResourceNames, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_BufferUAVResourceBindings, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_BufferUAVResourceCounts, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_BufferUAVResourceSets, 1, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_SamplerResourceNames, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_SamplerResourceBindings, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_SamplerResourceCounts, g_ffx_dof_downsample_depth_pass_16bit_22237156f20b7e38b8f381ac363b578d_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_size, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_data, 1, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_CBVResourceNames, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_CBVResourceBindings, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_CBVResourceCounts, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_CBVResourceSets, 1, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_TextureSRVResourceNames, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_TextureSRVResourceBindings, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_TextureSRVResourceCounts, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_TextureSRVResourceSets, 1, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_TextureUAVResourceNames, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_TextureUAVResourceBindings, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_TextureUAVResourceCounts, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_TextureUAVResourceSets, 0, 0, 0, 0, 0, 1, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_BufferUAVResourceNames, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_BufferUAVResourceBindings, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_BufferUAVResourceCounts, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_BufferUAVResourceSets, 1, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_SamplerResourceNames, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_SamplerResourceBindings, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_SamplerResourceCounts, g_ffx_dof_downsample_depth_pass_16bit_d9a1efd50cabc5bbb7c11e9d3fa61c22_SamplerResourceSets, 0, 0, 0, 0, 0, },
};


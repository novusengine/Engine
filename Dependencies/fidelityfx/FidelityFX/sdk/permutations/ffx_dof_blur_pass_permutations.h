#include "ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7.h"
#include "ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461.h"

typedef union ffx_dof_blur_pass_PermutationKey {
    struct {
        uint32_t FFX_DOF_OPTION_MAX_RING_MERGE_LOG : 1;
        uint32_t FFX_DOF_OPTION_COMBINE_IN_PLACE : 1;
        uint32_t FFX_DOF_OPTION_REVERSE_DEPTH : 1;
    };
    uint32_t index;
} ffx_dof_blur_pass_PermutationKey;

typedef struct ffx_dof_blur_pass_PermutationInfo {
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
} ffx_dof_blur_pass_PermutationInfo;

static const uint32_t g_ffx_dof_blur_pass_IndirectionTable[] = {
    1,
    0,
    1,
    0,
    1,
    0,
    1,
    0,
};

static const ffx_dof_blur_pass_PermutationInfo g_ffx_dof_blur_pass_PermutationInfo[] = {
    { g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_size, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_data, 1, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_CBVResourceNames, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_CBVResourceBindings, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_CBVResourceCounts, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_CBVResourceSets, 2, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_TextureSRVResourceNames, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_TextureSRVResourceBindings, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_TextureSRVResourceCounts, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_TextureSRVResourceSets, 2, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_TextureUAVResourceNames, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_TextureUAVResourceBindings, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_TextureUAVResourceCounts, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_TextureUAVResourceSets, 0, 0, 0, 0, 0, 1, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_BufferUAVResourceNames, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_BufferUAVResourceBindings, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_BufferUAVResourceCounts, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_BufferUAVResourceSets, 2, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_SamplerResourceNames, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_SamplerResourceBindings, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_SamplerResourceCounts, g_ffx_dof_blur_pass_b01d5f59b2eedfb0bcc6c777432d5ec7_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_size, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_data, 1, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_CBVResourceNames, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_CBVResourceBindings, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_CBVResourceCounts, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_CBVResourceSets, 2, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_TextureSRVResourceNames, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_TextureSRVResourceBindings, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_TextureSRVResourceCounts, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_TextureSRVResourceSets, 2, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_TextureUAVResourceNames, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_TextureUAVResourceBindings, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_TextureUAVResourceCounts, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_TextureUAVResourceSets, 0, 0, 0, 0, 0, 1, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_BufferUAVResourceNames, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_BufferUAVResourceBindings, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_BufferUAVResourceCounts, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_BufferUAVResourceSets, 2, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_SamplerResourceNames, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_SamplerResourceBindings, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_SamplerResourceCounts, g_ffx_dof_blur_pass_5fda7ca35fb7e0569fcc63aeba9cd461_SamplerResourceSets, 0, 0, 0, 0, 0, },
};


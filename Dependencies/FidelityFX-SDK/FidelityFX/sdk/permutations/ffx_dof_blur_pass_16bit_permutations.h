#include "ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8.h"
#include "ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e.h"

typedef union ffx_dof_blur_pass_16bit_PermutationKey {
    struct {
        uint32_t FFX_DOF_OPTION_MAX_RING_MERGE_LOG : 1;
        uint32_t FFX_DOF_OPTION_COMBINE_IN_PLACE : 1;
        uint32_t FFX_DOF_OPTION_REVERSE_DEPTH : 1;
    };
    uint32_t index;
} ffx_dof_blur_pass_16bit_PermutationKey;

typedef struct ffx_dof_blur_pass_16bit_PermutationInfo {
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
} ffx_dof_blur_pass_16bit_PermutationInfo;

static const uint32_t g_ffx_dof_blur_pass_16bit_IndirectionTable[] = {
    1,
    0,
    1,
    0,
    1,
    0,
    1,
    0,
};

static const ffx_dof_blur_pass_16bit_PermutationInfo g_ffx_dof_blur_pass_16bit_PermutationInfo[] = {
    { g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_size, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_data, 1, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_CBVResourceNames, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_CBVResourceBindings, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_CBVResourceCounts, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_CBVResourceSets, 2, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_TextureSRVResourceNames, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_TextureSRVResourceBindings, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_TextureSRVResourceCounts, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_TextureSRVResourceSets, 2, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_TextureUAVResourceNames, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_TextureUAVResourceBindings, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_TextureUAVResourceCounts, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_TextureUAVResourceSets, 0, 0, 0, 0, 0, 1, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_BufferUAVResourceNames, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_BufferUAVResourceBindings, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_BufferUAVResourceCounts, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_BufferUAVResourceSets, 2, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_SamplerResourceNames, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_SamplerResourceBindings, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_SamplerResourceCounts, g_ffx_dof_blur_pass_16bit_1826926405ff512b8ffa950cbe4ec6c8_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_size, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_data, 1, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_CBVResourceNames, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_CBVResourceBindings, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_CBVResourceCounts, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_CBVResourceSets, 2, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_TextureSRVResourceNames, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_TextureSRVResourceBindings, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_TextureSRVResourceCounts, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_TextureSRVResourceSets, 2, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_TextureUAVResourceNames, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_TextureUAVResourceBindings, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_TextureUAVResourceCounts, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_TextureUAVResourceSets, 0, 0, 0, 0, 0, 1, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_BufferUAVResourceNames, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_BufferUAVResourceBindings, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_BufferUAVResourceCounts, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_BufferUAVResourceSets, 2, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_SamplerResourceNames, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_SamplerResourceBindings, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_SamplerResourceCounts, g_ffx_dof_blur_pass_16bit_0dd62a0092f4a1382c13f834c60c487e_SamplerResourceSets, 0, 0, 0, 0, 0, },
};


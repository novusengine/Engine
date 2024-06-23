#include "ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39.h"
#include "ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10.h"

typedef union ffx_dof_downsample_depth_pass_wave64_PermutationKey {
    struct {
        uint32_t FFX_DOF_OPTION_MAX_RING_MERGE_LOG : 1;
        uint32_t FFX_DOF_OPTION_COMBINE_IN_PLACE : 1;
        uint32_t FFX_DOF_OPTION_REVERSE_DEPTH : 1;
    };
    uint32_t index;
} ffx_dof_downsample_depth_pass_wave64_PermutationKey;

typedef struct ffx_dof_downsample_depth_pass_wave64_PermutationInfo {
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
} ffx_dof_downsample_depth_pass_wave64_PermutationInfo;

static const uint32_t g_ffx_dof_downsample_depth_pass_wave64_IndirectionTable[] = {
    1,
    1,
    1,
    1,
    0,
    0,
    0,
    0,
};

static const ffx_dof_downsample_depth_pass_wave64_PermutationInfo g_ffx_dof_downsample_depth_pass_wave64_PermutationInfo[] = {
    { g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_size, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_data, 1, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_CBVResourceNames, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_CBVResourceBindings, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_CBVResourceCounts, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_CBVResourceSets, 1, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_TextureSRVResourceNames, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_TextureSRVResourceBindings, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_TextureSRVResourceCounts, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_TextureSRVResourceSets, 1, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_TextureUAVResourceNames, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_TextureUAVResourceBindings, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_TextureUAVResourceCounts, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_TextureUAVResourceSets, 0, 0, 0, 0, 0, 1, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_BufferUAVResourceNames, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_BufferUAVResourceBindings, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_BufferUAVResourceCounts, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_BufferUAVResourceSets, 1, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_SamplerResourceNames, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_SamplerResourceBindings, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_SamplerResourceCounts, g_ffx_dof_downsample_depth_pass_wave64_d7bedcab6a82e49eb1372208433eff39_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_size, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_data, 1, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_CBVResourceNames, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_CBVResourceBindings, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_CBVResourceCounts, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_CBVResourceSets, 1, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_TextureSRVResourceNames, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_TextureSRVResourceBindings, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_TextureSRVResourceCounts, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_TextureSRVResourceSets, 1, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_TextureUAVResourceNames, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_TextureUAVResourceBindings, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_TextureUAVResourceCounts, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_TextureUAVResourceSets, 0, 0, 0, 0, 0, 1, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_BufferUAVResourceNames, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_BufferUAVResourceBindings, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_BufferUAVResourceCounts, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_BufferUAVResourceSets, 1, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_SamplerResourceNames, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_SamplerResourceBindings, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_SamplerResourceCounts, g_ffx_dof_downsample_depth_pass_wave64_2c320d82f30a0a55ba879e40c8010a10_SamplerResourceSets, 0, 0, 0, 0, 0, },
};


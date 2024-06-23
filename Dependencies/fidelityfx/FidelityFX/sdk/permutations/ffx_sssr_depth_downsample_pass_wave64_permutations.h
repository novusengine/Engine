#include "ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c.h"
#include "ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7.h"

typedef union ffx_sssr_depth_downsample_pass_wave64_PermutationKey {
    struct {
        uint32_t FFX_SSSR_OPTION_INVERTED_DEPTH : 1;
    };
    uint32_t index;
} ffx_sssr_depth_downsample_pass_wave64_PermutationKey;

typedef struct ffx_sssr_depth_downsample_pass_wave64_PermutationInfo {
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
} ffx_sssr_depth_downsample_pass_wave64_PermutationInfo;

static const uint32_t g_ffx_sssr_depth_downsample_pass_wave64_IndirectionTable[] = {
    1,
    0,
};

static const ffx_sssr_depth_downsample_pass_wave64_PermutationInfo g_ffx_sssr_depth_downsample_pass_wave64_PermutationInfo[] = {
    { g_ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c_size, g_ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c_data, 0, 0, 0, 0, 0, 1, g_ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c_TextureSRVResourceNames, g_ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c_TextureSRVResourceBindings, g_ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c_TextureSRVResourceCounts, g_ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c_TextureSRVResourceSets, 1, g_ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c_TextureUAVResourceNames, g_ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c_TextureUAVResourceBindings, g_ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c_TextureUAVResourceCounts, g_ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c_TextureUAVResourceSets, 0, 0, 0, 0, 0, 1, g_ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c_BufferUAVResourceNames, g_ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c_BufferUAVResourceBindings, g_ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c_BufferUAVResourceCounts, g_ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c_BufferUAVResourceSets, 1, g_ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c_SamplerResourceNames, g_ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c_SamplerResourceBindings, g_ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c_SamplerResourceCounts, g_ffx_sssr_depth_downsample_pass_wave64_f30b9c59e61e1bd58e7a543c31d0fc2c_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7_size, g_ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7_data, 0, 0, 0, 0, 0, 1, g_ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7_TextureSRVResourceNames, g_ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7_TextureSRVResourceBindings, g_ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7_TextureSRVResourceCounts, g_ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7_TextureSRVResourceSets, 1, g_ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7_TextureUAVResourceNames, g_ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7_TextureUAVResourceBindings, g_ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7_TextureUAVResourceCounts, g_ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7_TextureUAVResourceSets, 0, 0, 0, 0, 0, 1, g_ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7_BufferUAVResourceNames, g_ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7_BufferUAVResourceBindings, g_ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7_BufferUAVResourceCounts, g_ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7_BufferUAVResourceSets, 1, g_ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7_SamplerResourceNames, g_ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7_SamplerResourceBindings, g_ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7_SamplerResourceCounts, g_ffx_sssr_depth_downsample_pass_wave64_965116e121ba3d276b783d5d6cf252b7_SamplerResourceSets, 0, 0, 0, 0, 0, },
};


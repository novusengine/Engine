#include "ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee.h"
#include "ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81.h"

typedef union ffx_sssr_depth_downsample_pass_16bit_PermutationKey {
    struct {
        uint32_t FFX_SSSR_OPTION_INVERTED_DEPTH : 1;
    };
    uint32_t index;
} ffx_sssr_depth_downsample_pass_16bit_PermutationKey;

typedef struct ffx_sssr_depth_downsample_pass_16bit_PermutationInfo {
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
} ffx_sssr_depth_downsample_pass_16bit_PermutationInfo;

static const uint32_t g_ffx_sssr_depth_downsample_pass_16bit_IndirectionTable[] = {
    0,
    1,
};

static const ffx_sssr_depth_downsample_pass_16bit_PermutationInfo g_ffx_sssr_depth_downsample_pass_16bit_PermutationInfo[] = {
    { g_ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee_size, g_ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee_data, 0, 0, 0, 0, 0, 1, g_ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee_TextureSRVResourceNames, g_ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee_TextureSRVResourceBindings, g_ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee_TextureSRVResourceCounts, g_ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee_TextureSRVResourceSets, 1, g_ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee_TextureUAVResourceNames, g_ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee_TextureUAVResourceBindings, g_ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee_TextureUAVResourceCounts, g_ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee_TextureUAVResourceSets, 0, 0, 0, 0, 0, 1, g_ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee_BufferUAVResourceNames, g_ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee_BufferUAVResourceBindings, g_ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee_BufferUAVResourceCounts, g_ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee_BufferUAVResourceSets, 1, g_ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee_SamplerResourceNames, g_ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee_SamplerResourceBindings, g_ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee_SamplerResourceCounts, g_ffx_sssr_depth_downsample_pass_16bit_5d87a86f251ab54826571c3f962735ee_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81_size, g_ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81_data, 0, 0, 0, 0, 0, 1, g_ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81_TextureSRVResourceNames, g_ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81_TextureSRVResourceBindings, g_ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81_TextureSRVResourceCounts, g_ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81_TextureSRVResourceSets, 1, g_ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81_TextureUAVResourceNames, g_ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81_TextureUAVResourceBindings, g_ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81_TextureUAVResourceCounts, g_ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81_TextureUAVResourceSets, 0, 0, 0, 0, 0, 1, g_ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81_BufferUAVResourceNames, g_ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81_BufferUAVResourceBindings, g_ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81_BufferUAVResourceCounts, g_ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81_BufferUAVResourceSets, 1, g_ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81_SamplerResourceNames, g_ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81_SamplerResourceBindings, g_ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81_SamplerResourceCounts, g_ffx_sssr_depth_downsample_pass_16bit_17f6c30839c607ccf7fbba4f13b7af81_SamplerResourceSets, 0, 0, 0, 0, 0, },
};


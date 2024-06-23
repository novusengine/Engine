#include "ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b.h"
#include "ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a.h"

typedef union ffx_sssr_intersect_pass_PermutationKey {
    struct {
        uint32_t FFX_SSSR_OPTION_INVERTED_DEPTH : 1;
    };
    uint32_t index;
} ffx_sssr_intersect_pass_PermutationKey;

typedef struct ffx_sssr_intersect_pass_PermutationInfo {
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
} ffx_sssr_intersect_pass_PermutationInfo;

static const uint32_t g_ffx_sssr_intersect_pass_IndirectionTable[] = {
    1,
    0,
};

static const ffx_sssr_intersect_pass_PermutationInfo g_ffx_sssr_intersect_pass_PermutationInfo[] = {
    { g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_size, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_data, 1, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_CBVResourceNames, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_CBVResourceBindings, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_CBVResourceCounts, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_CBVResourceSets, 6, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_TextureSRVResourceNames, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_TextureSRVResourceBindings, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_TextureSRVResourceCounts, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_TextureSRVResourceSets, 1, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_TextureUAVResourceNames, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_TextureUAVResourceBindings, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_TextureUAVResourceCounts, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_TextureUAVResourceSets, 0, 0, 0, 0, 0, 2, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_BufferUAVResourceNames, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_BufferUAVResourceBindings, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_BufferUAVResourceCounts, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_BufferUAVResourceSets, 1, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_SamplerResourceNames, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_SamplerResourceBindings, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_SamplerResourceCounts, g_ffx_sssr_intersect_pass_3dd8815b40bd48cdaeecb8b8e7c5032b_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_size, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_data, 1, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_CBVResourceNames, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_CBVResourceBindings, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_CBVResourceCounts, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_CBVResourceSets, 6, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_TextureSRVResourceNames, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_TextureSRVResourceBindings, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_TextureSRVResourceCounts, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_TextureSRVResourceSets, 1, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_TextureUAVResourceNames, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_TextureUAVResourceBindings, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_TextureUAVResourceCounts, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_TextureUAVResourceSets, 0, 0, 0, 0, 0, 2, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_BufferUAVResourceNames, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_BufferUAVResourceBindings, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_BufferUAVResourceCounts, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_BufferUAVResourceSets, 1, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_SamplerResourceNames, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_SamplerResourceBindings, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_SamplerResourceCounts, g_ffx_sssr_intersect_pass_ca9fdb447b9505fc97fa39a7d37d237a_SamplerResourceSets, 0, 0, 0, 0, 0, },
};


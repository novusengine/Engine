#include "ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e.h"
#include "ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7.h"

typedef union ffx_sssr_classify_tiles_pass_PermutationKey {
    struct {
        uint32_t FFX_SSSR_OPTION_INVERTED_DEPTH : 1;
    };
    uint32_t index;
} ffx_sssr_classify_tiles_pass_PermutationKey;

typedef struct ffx_sssr_classify_tiles_pass_PermutationInfo {
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
} ffx_sssr_classify_tiles_pass_PermutationInfo;

static const uint32_t g_ffx_sssr_classify_tiles_pass_IndirectionTable[] = {
    1,
    0,
};

static const ffx_sssr_classify_tiles_pass_PermutationInfo g_ffx_sssr_classify_tiles_pass_PermutationInfo[] = {
    { g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_size, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_data, 1, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_CBVResourceNames, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_CBVResourceBindings, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_CBVResourceCounts, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_CBVResourceSets, 5, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_TextureSRVResourceNames, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_TextureSRVResourceBindings, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_TextureSRVResourceCounts, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_TextureSRVResourceSets, 2, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_TextureUAVResourceNames, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_TextureUAVResourceBindings, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_TextureUAVResourceCounts, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_TextureUAVResourceSets, 0, 0, 0, 0, 0, 3, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_BufferUAVResourceNames, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_BufferUAVResourceBindings, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_BufferUAVResourceCounts, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_BufferUAVResourceSets, 1, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_SamplerResourceNames, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_SamplerResourceBindings, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_SamplerResourceCounts, g_ffx_sssr_classify_tiles_pass_5187947b42daae4032cc982efaac301e_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_size, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_data, 1, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_CBVResourceNames, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_CBVResourceBindings, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_CBVResourceCounts, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_CBVResourceSets, 5, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_TextureSRVResourceNames, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_TextureSRVResourceBindings, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_TextureSRVResourceCounts, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_TextureSRVResourceSets, 2, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_TextureUAVResourceNames, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_TextureUAVResourceBindings, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_TextureUAVResourceCounts, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_TextureUAVResourceSets, 0, 0, 0, 0, 0, 3, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_BufferUAVResourceNames, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_BufferUAVResourceBindings, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_BufferUAVResourceCounts, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_BufferUAVResourceSets, 1, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_SamplerResourceNames, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_SamplerResourceBindings, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_SamplerResourceCounts, g_ffx_sssr_classify_tiles_pass_19f4ac8525a041d8bb6ddae74c2ec2a7_SamplerResourceSets, 0, 0, 0, 0, 0, },
};


#include "ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2.h"
#include "ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674.h"

typedef union ffx_sssr_intersect_pass_wave64_16bit_PermutationKey {
    struct {
        uint32_t FFX_SSSR_OPTION_INVERTED_DEPTH : 1;
    };
    uint32_t index;
} ffx_sssr_intersect_pass_wave64_16bit_PermutationKey;

typedef struct ffx_sssr_intersect_pass_wave64_16bit_PermutationInfo {
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
} ffx_sssr_intersect_pass_wave64_16bit_PermutationInfo;

static const uint32_t g_ffx_sssr_intersect_pass_wave64_16bit_IndirectionTable[] = {
    1,
    0,
};

static const ffx_sssr_intersect_pass_wave64_16bit_PermutationInfo g_ffx_sssr_intersect_pass_wave64_16bit_PermutationInfo[] = {
    { g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_size, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_data, 1, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_CBVResourceNames, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_CBVResourceBindings, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_CBVResourceCounts, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_CBVResourceSets, 6, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_TextureSRVResourceNames, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_TextureSRVResourceBindings, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_TextureSRVResourceCounts, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_TextureSRVResourceSets, 1, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_TextureUAVResourceNames, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_TextureUAVResourceBindings, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_TextureUAVResourceCounts, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_TextureUAVResourceSets, 0, 0, 0, 0, 0, 2, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_BufferUAVResourceNames, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_BufferUAVResourceBindings, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_BufferUAVResourceCounts, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_BufferUAVResourceSets, 1, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_SamplerResourceNames, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_SamplerResourceBindings, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_SamplerResourceCounts, g_ffx_sssr_intersect_pass_wave64_16bit_aaa350f9560609843dd14fdd799137e2_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_size, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_data, 1, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_CBVResourceNames, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_CBVResourceBindings, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_CBVResourceCounts, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_CBVResourceSets, 6, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_TextureSRVResourceNames, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_TextureSRVResourceBindings, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_TextureSRVResourceCounts, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_TextureSRVResourceSets, 1, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_TextureUAVResourceNames, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_TextureUAVResourceBindings, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_TextureUAVResourceCounts, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_TextureUAVResourceSets, 0, 0, 0, 0, 0, 2, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_BufferUAVResourceNames, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_BufferUAVResourceBindings, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_BufferUAVResourceCounts, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_BufferUAVResourceSets, 1, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_SamplerResourceNames, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_SamplerResourceBindings, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_SamplerResourceCounts, g_ffx_sssr_intersect_pass_wave64_16bit_f901ae5e902c4490d7ecbd18951dc674_SamplerResourceSets, 0, 0, 0, 0, 0, },
};


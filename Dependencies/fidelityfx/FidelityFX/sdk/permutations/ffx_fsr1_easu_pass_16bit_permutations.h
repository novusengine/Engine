#include "ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa.h"
#include "ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a.h"
#include "ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2.h"
#include "ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871.h"

typedef union ffx_fsr1_easu_pass_16bit_PermutationKey {
    struct {
        uint32_t FFX_FSR1_OPTION_APPLY_RCAS : 1;
        uint32_t FFX_FSR1_OPTION_RCAS_PASSTHROUGH_ALPHA : 1;
        uint32_t FFX_FSR1_OPTION_SRGB_CONVERSIONS : 1;
    };
    uint32_t index;
} ffx_fsr1_easu_pass_16bit_PermutationKey;

typedef struct ffx_fsr1_easu_pass_16bit_PermutationInfo {
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
} ffx_fsr1_easu_pass_16bit_PermutationInfo;

static const uint32_t g_ffx_fsr1_easu_pass_16bit_IndirectionTable[] = {
    2,
    0,
    2,
    0,
    3,
    1,
    3,
    1,
};

static const ffx_fsr1_easu_pass_16bit_PermutationInfo g_ffx_fsr1_easu_pass_16bit_PermutationInfo[] = {
    { g_ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa_size, g_ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa_data, 1, g_ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa_CBVResourceNames, g_ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa_CBVResourceBindings, g_ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa_CBVResourceCounts, g_ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa_CBVResourceSets, 1, g_ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa_TextureSRVResourceNames, g_ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa_TextureSRVResourceBindings, g_ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa_TextureSRVResourceCounts, g_ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa_TextureSRVResourceSets, 1, g_ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa_TextureUAVResourceNames, g_ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa_TextureUAVResourceBindings, g_ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa_TextureUAVResourceCounts, g_ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa_SamplerResourceNames, g_ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa_SamplerResourceBindings, g_ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa_SamplerResourceCounts, g_ffx_fsr1_easu_pass_16bit_6c4a45f2e407cca531ff0495c38bc1aa_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a_size, g_ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a_data, 1, g_ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a_CBVResourceNames, g_ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a_CBVResourceBindings, g_ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a_CBVResourceCounts, g_ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a_CBVResourceSets, 1, g_ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a_TextureSRVResourceNames, g_ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a_TextureSRVResourceBindings, g_ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a_TextureSRVResourceCounts, g_ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a_TextureSRVResourceSets, 1, g_ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a_TextureUAVResourceNames, g_ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a_TextureUAVResourceBindings, g_ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a_TextureUAVResourceCounts, g_ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a_SamplerResourceNames, g_ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a_SamplerResourceBindings, g_ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a_SamplerResourceCounts, g_ffx_fsr1_easu_pass_16bit_0a74e9d78c23c9af798172fdff06b90a_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2_size, g_ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2_data, 1, g_ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2_CBVResourceNames, g_ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2_CBVResourceBindings, g_ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2_CBVResourceCounts, g_ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2_CBVResourceSets, 1, g_ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2_TextureSRVResourceNames, g_ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2_TextureSRVResourceBindings, g_ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2_TextureSRVResourceCounts, g_ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2_TextureSRVResourceSets, 1, g_ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2_TextureUAVResourceNames, g_ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2_TextureUAVResourceBindings, g_ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2_TextureUAVResourceCounts, g_ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2_SamplerResourceNames, g_ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2_SamplerResourceBindings, g_ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2_SamplerResourceCounts, g_ffx_fsr1_easu_pass_16bit_ab4ca463f7238c5ab4883989d9501bd2_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871_size, g_ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871_data, 1, g_ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871_CBVResourceNames, g_ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871_CBVResourceBindings, g_ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871_CBVResourceCounts, g_ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871_CBVResourceSets, 1, g_ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871_TextureSRVResourceNames, g_ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871_TextureSRVResourceBindings, g_ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871_TextureSRVResourceCounts, g_ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871_TextureSRVResourceSets, 1, g_ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871_TextureUAVResourceNames, g_ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871_TextureUAVResourceBindings, g_ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871_TextureUAVResourceCounts, g_ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871_SamplerResourceNames, g_ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871_SamplerResourceBindings, g_ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871_SamplerResourceCounts, g_ffx_fsr1_easu_pass_16bit_6979ec2bc0b64fa2bb5edde6468a9871_SamplerResourceSets, 0, 0, 0, 0, 0, },
};


#include "ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a.h"
#include "ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6.h"
#include "ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901.h"
#include "ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99.h"

typedef union ffx_fsr1_easu_pass_PermutationKey {
    struct {
        uint32_t FFX_FSR1_OPTION_APPLY_RCAS : 1;
        uint32_t FFX_FSR1_OPTION_RCAS_PASSTHROUGH_ALPHA : 1;
        uint32_t FFX_FSR1_OPTION_SRGB_CONVERSIONS : 1;
    };
    uint32_t index;
} ffx_fsr1_easu_pass_PermutationKey;

typedef struct ffx_fsr1_easu_pass_PermutationInfo {
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
} ffx_fsr1_easu_pass_PermutationInfo;

static const uint32_t g_ffx_fsr1_easu_pass_IndirectionTable[] = {
    3,
    2,
    3,
    2,
    0,
    1,
    0,
    1,
};

static const ffx_fsr1_easu_pass_PermutationInfo g_ffx_fsr1_easu_pass_PermutationInfo[] = {
    { g_ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a_size, g_ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a_data, 1, g_ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a_CBVResourceNames, g_ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a_CBVResourceBindings, g_ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a_CBVResourceCounts, g_ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a_CBVResourceSets, 1, g_ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a_TextureSRVResourceNames, g_ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a_TextureSRVResourceBindings, g_ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a_TextureSRVResourceCounts, g_ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a_TextureSRVResourceSets, 1, g_ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a_TextureUAVResourceNames, g_ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a_TextureUAVResourceBindings, g_ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a_TextureUAVResourceCounts, g_ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a_SamplerResourceNames, g_ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a_SamplerResourceBindings, g_ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a_SamplerResourceCounts, g_ffx_fsr1_easu_pass_21b3ea00725cfd6388f122c309d6960a_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6_size, g_ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6_data, 1, g_ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6_CBVResourceNames, g_ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6_CBVResourceBindings, g_ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6_CBVResourceCounts, g_ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6_CBVResourceSets, 1, g_ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6_TextureSRVResourceNames, g_ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6_TextureSRVResourceBindings, g_ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6_TextureSRVResourceCounts, g_ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6_TextureSRVResourceSets, 1, g_ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6_TextureUAVResourceNames, g_ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6_TextureUAVResourceBindings, g_ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6_TextureUAVResourceCounts, g_ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6_SamplerResourceNames, g_ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6_SamplerResourceBindings, g_ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6_SamplerResourceCounts, g_ffx_fsr1_easu_pass_2d8ff36661aea7a18db54db7298650d6_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901_size, g_ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901_data, 1, g_ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901_CBVResourceNames, g_ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901_CBVResourceBindings, g_ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901_CBVResourceCounts, g_ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901_CBVResourceSets, 1, g_ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901_TextureSRVResourceNames, g_ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901_TextureSRVResourceBindings, g_ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901_TextureSRVResourceCounts, g_ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901_TextureSRVResourceSets, 1, g_ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901_TextureUAVResourceNames, g_ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901_TextureUAVResourceBindings, g_ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901_TextureUAVResourceCounts, g_ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901_SamplerResourceNames, g_ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901_SamplerResourceBindings, g_ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901_SamplerResourceCounts, g_ffx_fsr1_easu_pass_0ab2330896f4186cdac7246134218901_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99_size, g_ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99_data, 1, g_ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99_CBVResourceNames, g_ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99_CBVResourceBindings, g_ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99_CBVResourceCounts, g_ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99_CBVResourceSets, 1, g_ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99_TextureSRVResourceNames, g_ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99_TextureSRVResourceBindings, g_ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99_TextureSRVResourceCounts, g_ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99_TextureSRVResourceSets, 1, g_ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99_TextureUAVResourceNames, g_ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99_TextureUAVResourceBindings, g_ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99_TextureUAVResourceCounts, g_ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, g_ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99_SamplerResourceNames, g_ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99_SamplerResourceBindings, g_ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99_SamplerResourceCounts, g_ffx_fsr1_easu_pass_11618bd7a15709296ce37e3ce359bc99_SamplerResourceSets, 0, 0, 0, 0, 0, },
};


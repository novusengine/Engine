#include "ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6.h"
#include "ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833.h"

typedef union ffx_classifier_reflections_pass_16bit_PermutationKey {
    struct {
        uint32_t FFX_CLASSIFIER_OPTION_INVERTED_DEPTH : 1;
        uint32_t FFX_CLASSIFIER_OPTION_CLASSIFIER_MODE : 1;
    };
    uint32_t index;
} ffx_classifier_reflections_pass_16bit_PermutationKey;

typedef struct ffx_classifier_reflections_pass_16bit_PermutationInfo {
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
} ffx_classifier_reflections_pass_16bit_PermutationInfo;

static const uint32_t g_ffx_classifier_reflections_pass_16bit_IndirectionTable[] = {
    1,
    0,
    1,
    0,
};

static const ffx_classifier_reflections_pass_16bit_PermutationInfo g_ffx_classifier_reflections_pass_16bit_PermutationInfo[] = {
    { g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_size, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_data, 1, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_CBVResourceNames, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_CBVResourceBindings, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_CBVResourceCounts, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_CBVResourceSets, 7, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_TextureSRVResourceNames, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_TextureSRVResourceBindings, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_TextureSRVResourceCounts, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_TextureSRVResourceSets, 3, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_TextureUAVResourceNames, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_TextureUAVResourceBindings, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_TextureUAVResourceCounts, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_TextureUAVResourceSets, 0, 0, 0, 0, 0, 4, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_BufferUAVResourceNames, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_BufferUAVResourceBindings, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_BufferUAVResourceCounts, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_BufferUAVResourceSets, 2, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_SamplerResourceNames, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_SamplerResourceBindings, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_SamplerResourceCounts, g_ffx_classifier_reflections_pass_16bit_c1fc5617269ec991320f3ceae8575bc6_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_size, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_data, 1, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_CBVResourceNames, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_CBVResourceBindings, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_CBVResourceCounts, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_CBVResourceSets, 7, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_TextureSRVResourceNames, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_TextureSRVResourceBindings, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_TextureSRVResourceCounts, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_TextureSRVResourceSets, 3, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_TextureUAVResourceNames, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_TextureUAVResourceBindings, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_TextureUAVResourceCounts, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_TextureUAVResourceSets, 0, 0, 0, 0, 0, 4, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_BufferUAVResourceNames, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_BufferUAVResourceBindings, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_BufferUAVResourceCounts, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_BufferUAVResourceSets, 2, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_SamplerResourceNames, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_SamplerResourceBindings, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_SamplerResourceCounts, g_ffx_classifier_reflections_pass_16bit_032706baa7070dc78ae92335347b0833_SamplerResourceSets, 0, 0, 0, 0, 0, },
};


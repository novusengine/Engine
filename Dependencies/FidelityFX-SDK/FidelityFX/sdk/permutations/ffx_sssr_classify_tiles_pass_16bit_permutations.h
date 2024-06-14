#include "ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6.h"
#include "ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130.h"

typedef union ffx_sssr_classify_tiles_pass_16bit_PermutationKey {
    struct {
        uint32_t FFX_SSSR_OPTION_INVERTED_DEPTH : 1;
    };
    uint32_t index;
} ffx_sssr_classify_tiles_pass_16bit_PermutationKey;

typedef struct ffx_sssr_classify_tiles_pass_16bit_PermutationInfo {
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
} ffx_sssr_classify_tiles_pass_16bit_PermutationInfo;

static const uint32_t g_ffx_sssr_classify_tiles_pass_16bit_IndirectionTable[] = {
    1,
    0,
};

static const ffx_sssr_classify_tiles_pass_16bit_PermutationInfo g_ffx_sssr_classify_tiles_pass_16bit_PermutationInfo[] = {
    { g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_size, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_data, 1, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_CBVResourceNames, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_CBVResourceBindings, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_CBVResourceCounts, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_CBVResourceSets, 5, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_TextureSRVResourceNames, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_TextureSRVResourceBindings, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_TextureSRVResourceCounts, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_TextureSRVResourceSets, 2, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_TextureUAVResourceNames, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_TextureUAVResourceBindings, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_TextureUAVResourceCounts, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_TextureUAVResourceSets, 0, 0, 0, 0, 0, 3, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_BufferUAVResourceNames, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_BufferUAVResourceBindings, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_BufferUAVResourceCounts, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_BufferUAVResourceSets, 1, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_SamplerResourceNames, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_SamplerResourceBindings, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_SamplerResourceCounts, g_ffx_sssr_classify_tiles_pass_16bit_019d887b8cac960872393714416c57f6_SamplerResourceSets, 0, 0, 0, 0, 0, },
    { g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_size, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_data, 1, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_CBVResourceNames, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_CBVResourceBindings, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_CBVResourceCounts, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_CBVResourceSets, 5, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_TextureSRVResourceNames, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_TextureSRVResourceBindings, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_TextureSRVResourceCounts, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_TextureSRVResourceSets, 2, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_TextureUAVResourceNames, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_TextureUAVResourceBindings, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_TextureUAVResourceCounts, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_TextureUAVResourceSets, 0, 0, 0, 0, 0, 3, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_BufferUAVResourceNames, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_BufferUAVResourceBindings, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_BufferUAVResourceCounts, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_BufferUAVResourceSets, 1, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_SamplerResourceNames, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_SamplerResourceBindings, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_SamplerResourceCounts, g_ffx_sssr_classify_tiles_pass_16bit_58c54af7805d9269215e127be986d130_SamplerResourceSets, 0, 0, 0, 0, 0, },
};


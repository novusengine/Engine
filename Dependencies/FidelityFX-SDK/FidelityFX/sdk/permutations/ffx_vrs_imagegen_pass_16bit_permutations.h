#include "ffx_vrs_imagegen_pass_16bit_a1699f9433892f534833d057df1816e9.h"
#include "ffx_vrs_imagegen_pass_16bit_38cca9868e82ecebb35738daa199c47b.h"
#include "ffx_vrs_imagegen_pass_16bit_044d6befeb5bd9e7f5901f7eb99a5fba.h"
#include "ffx_vrs_imagegen_pass_16bit_23b977d8f9494272b013c1671ca67732.h"
#include "ffx_vrs_imagegen_pass_16bit_60fa56db56241b778b24073aa9e101a2.h"
#include "ffx_vrs_imagegen_pass_16bit_7086ba07b426b76a5ee8a1b5f5d71bd9.h"

typedef union ffx_vrs_imagegen_pass_16bit_PermutationKey {
    struct {
        uint32_t FFX_VRS_OPTION_ADDITIONALSHADINGRATES : 1;
        uint32_t FFX_VARIABLESHADING_TILESIZE : 2;
    };
    uint32_t index;
} ffx_vrs_imagegen_pass_16bit_PermutationKey;

typedef struct ffx_vrs_imagegen_pass_16bit_PermutationInfo {
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
} ffx_vrs_imagegen_pass_16bit_PermutationInfo;

static const uint32_t g_ffx_vrs_imagegen_pass_16bit_IndirectionTable[] = {
    4,
    5,
    2,
    1,
    3,
    0,
    0,
    0,
};

static const ffx_vrs_imagegen_pass_16bit_PermutationInfo g_ffx_vrs_imagegen_pass_16bit_PermutationInfo[] = {
    { g_ffx_vrs_imagegen_pass_16bit_a1699f9433892f534833d057df1816e9_size, g_ffx_vrs_imagegen_pass_16bit_a1699f9433892f534833d057df1816e9_data, 1, g_ffx_vrs_imagegen_pass_16bit_a1699f9433892f534833d057df1816e9_CBVResourceNames, g_ffx_vrs_imagegen_pass_16bit_a1699f9433892f534833d057df1816e9_CBVResourceBindings, g_ffx_vrs_imagegen_pass_16bit_a1699f9433892f534833d057df1816e9_CBVResourceCounts, g_ffx_vrs_imagegen_pass_16bit_a1699f9433892f534833d057df1816e9_CBVResourceSets, 2, g_ffx_vrs_imagegen_pass_16bit_a1699f9433892f534833d057df1816e9_TextureSRVResourceNames, g_ffx_vrs_imagegen_pass_16bit_a1699f9433892f534833d057df1816e9_TextureSRVResourceBindings, g_ffx_vrs_imagegen_pass_16bit_a1699f9433892f534833d057df1816e9_TextureSRVResourceCounts, g_ffx_vrs_imagegen_pass_16bit_a1699f9433892f534833d057df1816e9_TextureSRVResourceSets, 1, g_ffx_vrs_imagegen_pass_16bit_a1699f9433892f534833d057df1816e9_TextureUAVResourceNames, g_ffx_vrs_imagegen_pass_16bit_a1699f9433892f534833d057df1816e9_TextureUAVResourceBindings, g_ffx_vrs_imagegen_pass_16bit_a1699f9433892f534833d057df1816e9_TextureUAVResourceCounts, g_ffx_vrs_imagegen_pass_16bit_a1699f9433892f534833d057df1816e9_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_vrs_imagegen_pass_16bit_38cca9868e82ecebb35738daa199c47b_size, g_ffx_vrs_imagegen_pass_16bit_38cca9868e82ecebb35738daa199c47b_data, 1, g_ffx_vrs_imagegen_pass_16bit_38cca9868e82ecebb35738daa199c47b_CBVResourceNames, g_ffx_vrs_imagegen_pass_16bit_38cca9868e82ecebb35738daa199c47b_CBVResourceBindings, g_ffx_vrs_imagegen_pass_16bit_38cca9868e82ecebb35738daa199c47b_CBVResourceCounts, g_ffx_vrs_imagegen_pass_16bit_38cca9868e82ecebb35738daa199c47b_CBVResourceSets, 2, g_ffx_vrs_imagegen_pass_16bit_38cca9868e82ecebb35738daa199c47b_TextureSRVResourceNames, g_ffx_vrs_imagegen_pass_16bit_38cca9868e82ecebb35738daa199c47b_TextureSRVResourceBindings, g_ffx_vrs_imagegen_pass_16bit_38cca9868e82ecebb35738daa199c47b_TextureSRVResourceCounts, g_ffx_vrs_imagegen_pass_16bit_38cca9868e82ecebb35738daa199c47b_TextureSRVResourceSets, 1, g_ffx_vrs_imagegen_pass_16bit_38cca9868e82ecebb35738daa199c47b_TextureUAVResourceNames, g_ffx_vrs_imagegen_pass_16bit_38cca9868e82ecebb35738daa199c47b_TextureUAVResourceBindings, g_ffx_vrs_imagegen_pass_16bit_38cca9868e82ecebb35738daa199c47b_TextureUAVResourceCounts, g_ffx_vrs_imagegen_pass_16bit_38cca9868e82ecebb35738daa199c47b_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_vrs_imagegen_pass_16bit_044d6befeb5bd9e7f5901f7eb99a5fba_size, g_ffx_vrs_imagegen_pass_16bit_044d6befeb5bd9e7f5901f7eb99a5fba_data, 1, g_ffx_vrs_imagegen_pass_16bit_044d6befeb5bd9e7f5901f7eb99a5fba_CBVResourceNames, g_ffx_vrs_imagegen_pass_16bit_044d6befeb5bd9e7f5901f7eb99a5fba_CBVResourceBindings, g_ffx_vrs_imagegen_pass_16bit_044d6befeb5bd9e7f5901f7eb99a5fba_CBVResourceCounts, g_ffx_vrs_imagegen_pass_16bit_044d6befeb5bd9e7f5901f7eb99a5fba_CBVResourceSets, 2, g_ffx_vrs_imagegen_pass_16bit_044d6befeb5bd9e7f5901f7eb99a5fba_TextureSRVResourceNames, g_ffx_vrs_imagegen_pass_16bit_044d6befeb5bd9e7f5901f7eb99a5fba_TextureSRVResourceBindings, g_ffx_vrs_imagegen_pass_16bit_044d6befeb5bd9e7f5901f7eb99a5fba_TextureSRVResourceCounts, g_ffx_vrs_imagegen_pass_16bit_044d6befeb5bd9e7f5901f7eb99a5fba_TextureSRVResourceSets, 1, g_ffx_vrs_imagegen_pass_16bit_044d6befeb5bd9e7f5901f7eb99a5fba_TextureUAVResourceNames, g_ffx_vrs_imagegen_pass_16bit_044d6befeb5bd9e7f5901f7eb99a5fba_TextureUAVResourceBindings, g_ffx_vrs_imagegen_pass_16bit_044d6befeb5bd9e7f5901f7eb99a5fba_TextureUAVResourceCounts, g_ffx_vrs_imagegen_pass_16bit_044d6befeb5bd9e7f5901f7eb99a5fba_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_vrs_imagegen_pass_16bit_23b977d8f9494272b013c1671ca67732_size, g_ffx_vrs_imagegen_pass_16bit_23b977d8f9494272b013c1671ca67732_data, 1, g_ffx_vrs_imagegen_pass_16bit_23b977d8f9494272b013c1671ca67732_CBVResourceNames, g_ffx_vrs_imagegen_pass_16bit_23b977d8f9494272b013c1671ca67732_CBVResourceBindings, g_ffx_vrs_imagegen_pass_16bit_23b977d8f9494272b013c1671ca67732_CBVResourceCounts, g_ffx_vrs_imagegen_pass_16bit_23b977d8f9494272b013c1671ca67732_CBVResourceSets, 2, g_ffx_vrs_imagegen_pass_16bit_23b977d8f9494272b013c1671ca67732_TextureSRVResourceNames, g_ffx_vrs_imagegen_pass_16bit_23b977d8f9494272b013c1671ca67732_TextureSRVResourceBindings, g_ffx_vrs_imagegen_pass_16bit_23b977d8f9494272b013c1671ca67732_TextureSRVResourceCounts, g_ffx_vrs_imagegen_pass_16bit_23b977d8f9494272b013c1671ca67732_TextureSRVResourceSets, 1, g_ffx_vrs_imagegen_pass_16bit_23b977d8f9494272b013c1671ca67732_TextureUAVResourceNames, g_ffx_vrs_imagegen_pass_16bit_23b977d8f9494272b013c1671ca67732_TextureUAVResourceBindings, g_ffx_vrs_imagegen_pass_16bit_23b977d8f9494272b013c1671ca67732_TextureUAVResourceCounts, g_ffx_vrs_imagegen_pass_16bit_23b977d8f9494272b013c1671ca67732_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_vrs_imagegen_pass_16bit_60fa56db56241b778b24073aa9e101a2_size, g_ffx_vrs_imagegen_pass_16bit_60fa56db56241b778b24073aa9e101a2_data, 1, g_ffx_vrs_imagegen_pass_16bit_60fa56db56241b778b24073aa9e101a2_CBVResourceNames, g_ffx_vrs_imagegen_pass_16bit_60fa56db56241b778b24073aa9e101a2_CBVResourceBindings, g_ffx_vrs_imagegen_pass_16bit_60fa56db56241b778b24073aa9e101a2_CBVResourceCounts, g_ffx_vrs_imagegen_pass_16bit_60fa56db56241b778b24073aa9e101a2_CBVResourceSets, 2, g_ffx_vrs_imagegen_pass_16bit_60fa56db56241b778b24073aa9e101a2_TextureSRVResourceNames, g_ffx_vrs_imagegen_pass_16bit_60fa56db56241b778b24073aa9e101a2_TextureSRVResourceBindings, g_ffx_vrs_imagegen_pass_16bit_60fa56db56241b778b24073aa9e101a2_TextureSRVResourceCounts, g_ffx_vrs_imagegen_pass_16bit_60fa56db56241b778b24073aa9e101a2_TextureSRVResourceSets, 1, g_ffx_vrs_imagegen_pass_16bit_60fa56db56241b778b24073aa9e101a2_TextureUAVResourceNames, g_ffx_vrs_imagegen_pass_16bit_60fa56db56241b778b24073aa9e101a2_TextureUAVResourceBindings, g_ffx_vrs_imagegen_pass_16bit_60fa56db56241b778b24073aa9e101a2_TextureUAVResourceCounts, g_ffx_vrs_imagegen_pass_16bit_60fa56db56241b778b24073aa9e101a2_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_vrs_imagegen_pass_16bit_7086ba07b426b76a5ee8a1b5f5d71bd9_size, g_ffx_vrs_imagegen_pass_16bit_7086ba07b426b76a5ee8a1b5f5d71bd9_data, 1, g_ffx_vrs_imagegen_pass_16bit_7086ba07b426b76a5ee8a1b5f5d71bd9_CBVResourceNames, g_ffx_vrs_imagegen_pass_16bit_7086ba07b426b76a5ee8a1b5f5d71bd9_CBVResourceBindings, g_ffx_vrs_imagegen_pass_16bit_7086ba07b426b76a5ee8a1b5f5d71bd9_CBVResourceCounts, g_ffx_vrs_imagegen_pass_16bit_7086ba07b426b76a5ee8a1b5f5d71bd9_CBVResourceSets, 2, g_ffx_vrs_imagegen_pass_16bit_7086ba07b426b76a5ee8a1b5f5d71bd9_TextureSRVResourceNames, g_ffx_vrs_imagegen_pass_16bit_7086ba07b426b76a5ee8a1b5f5d71bd9_TextureSRVResourceBindings, g_ffx_vrs_imagegen_pass_16bit_7086ba07b426b76a5ee8a1b5f5d71bd9_TextureSRVResourceCounts, g_ffx_vrs_imagegen_pass_16bit_7086ba07b426b76a5ee8a1b5f5d71bd9_TextureSRVResourceSets, 1, g_ffx_vrs_imagegen_pass_16bit_7086ba07b426b76a5ee8a1b5f5d71bd9_TextureUAVResourceNames, g_ffx_vrs_imagegen_pass_16bit_7086ba07b426b76a5ee8a1b5f5d71bd9_TextureUAVResourceBindings, g_ffx_vrs_imagegen_pass_16bit_7086ba07b426b76a5ee8a1b5f5d71bd9_TextureUAVResourceCounts, g_ffx_vrs_imagegen_pass_16bit_7086ba07b426b76a5ee8a1b5f5d71bd9_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};


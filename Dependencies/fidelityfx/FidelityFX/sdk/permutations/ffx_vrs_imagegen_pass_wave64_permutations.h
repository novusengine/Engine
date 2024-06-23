#include "ffx_vrs_imagegen_pass_wave64_00e83ff17bc5ee9978142fae818fb34d.h"
#include "ffx_vrs_imagegen_pass_wave64_ef2a39bb8013901e48ad2c3d955c10b9.h"
#include "ffx_vrs_imagegen_pass_wave64_1afbfca2cd353be6c31fa807a62208a0.h"
#include "ffx_vrs_imagegen_pass_wave64_56a03f6f9e1f5396a5767d6ddebe380d.h"
#include "ffx_vrs_imagegen_pass_wave64_8e4ce92280acd3dfc4f1de311eb0580b.h"
#include "ffx_vrs_imagegen_pass_wave64_ac8addbfa47504857f9e74fce56fe4b7.h"

typedef union ffx_vrs_imagegen_pass_wave64_PermutationKey {
    struct {
        uint32_t FFX_VRS_OPTION_ADDITIONALSHADINGRATES : 1;
        uint32_t FFX_VARIABLESHADING_TILESIZE : 2;
    };
    uint32_t index;
} ffx_vrs_imagegen_pass_wave64_PermutationKey;

typedef struct ffx_vrs_imagegen_pass_wave64_PermutationInfo {
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
} ffx_vrs_imagegen_pass_wave64_PermutationInfo;

static const uint32_t g_ffx_vrs_imagegen_pass_wave64_IndirectionTable[] = {
    4,
    5,
    1,
    3,
    2,
    0,
    0,
    0,
};

static const ffx_vrs_imagegen_pass_wave64_PermutationInfo g_ffx_vrs_imagegen_pass_wave64_PermutationInfo[] = {
    { g_ffx_vrs_imagegen_pass_wave64_00e83ff17bc5ee9978142fae818fb34d_size, g_ffx_vrs_imagegen_pass_wave64_00e83ff17bc5ee9978142fae818fb34d_data, 1, g_ffx_vrs_imagegen_pass_wave64_00e83ff17bc5ee9978142fae818fb34d_CBVResourceNames, g_ffx_vrs_imagegen_pass_wave64_00e83ff17bc5ee9978142fae818fb34d_CBVResourceBindings, g_ffx_vrs_imagegen_pass_wave64_00e83ff17bc5ee9978142fae818fb34d_CBVResourceCounts, g_ffx_vrs_imagegen_pass_wave64_00e83ff17bc5ee9978142fae818fb34d_CBVResourceSets, 2, g_ffx_vrs_imagegen_pass_wave64_00e83ff17bc5ee9978142fae818fb34d_TextureSRVResourceNames, g_ffx_vrs_imagegen_pass_wave64_00e83ff17bc5ee9978142fae818fb34d_TextureSRVResourceBindings, g_ffx_vrs_imagegen_pass_wave64_00e83ff17bc5ee9978142fae818fb34d_TextureSRVResourceCounts, g_ffx_vrs_imagegen_pass_wave64_00e83ff17bc5ee9978142fae818fb34d_TextureSRVResourceSets, 1, g_ffx_vrs_imagegen_pass_wave64_00e83ff17bc5ee9978142fae818fb34d_TextureUAVResourceNames, g_ffx_vrs_imagegen_pass_wave64_00e83ff17bc5ee9978142fae818fb34d_TextureUAVResourceBindings, g_ffx_vrs_imagegen_pass_wave64_00e83ff17bc5ee9978142fae818fb34d_TextureUAVResourceCounts, g_ffx_vrs_imagegen_pass_wave64_00e83ff17bc5ee9978142fae818fb34d_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_vrs_imagegen_pass_wave64_ef2a39bb8013901e48ad2c3d955c10b9_size, g_ffx_vrs_imagegen_pass_wave64_ef2a39bb8013901e48ad2c3d955c10b9_data, 1, g_ffx_vrs_imagegen_pass_wave64_ef2a39bb8013901e48ad2c3d955c10b9_CBVResourceNames, g_ffx_vrs_imagegen_pass_wave64_ef2a39bb8013901e48ad2c3d955c10b9_CBVResourceBindings, g_ffx_vrs_imagegen_pass_wave64_ef2a39bb8013901e48ad2c3d955c10b9_CBVResourceCounts, g_ffx_vrs_imagegen_pass_wave64_ef2a39bb8013901e48ad2c3d955c10b9_CBVResourceSets, 2, g_ffx_vrs_imagegen_pass_wave64_ef2a39bb8013901e48ad2c3d955c10b9_TextureSRVResourceNames, g_ffx_vrs_imagegen_pass_wave64_ef2a39bb8013901e48ad2c3d955c10b9_TextureSRVResourceBindings, g_ffx_vrs_imagegen_pass_wave64_ef2a39bb8013901e48ad2c3d955c10b9_TextureSRVResourceCounts, g_ffx_vrs_imagegen_pass_wave64_ef2a39bb8013901e48ad2c3d955c10b9_TextureSRVResourceSets, 1, g_ffx_vrs_imagegen_pass_wave64_ef2a39bb8013901e48ad2c3d955c10b9_TextureUAVResourceNames, g_ffx_vrs_imagegen_pass_wave64_ef2a39bb8013901e48ad2c3d955c10b9_TextureUAVResourceBindings, g_ffx_vrs_imagegen_pass_wave64_ef2a39bb8013901e48ad2c3d955c10b9_TextureUAVResourceCounts, g_ffx_vrs_imagegen_pass_wave64_ef2a39bb8013901e48ad2c3d955c10b9_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_vrs_imagegen_pass_wave64_1afbfca2cd353be6c31fa807a62208a0_size, g_ffx_vrs_imagegen_pass_wave64_1afbfca2cd353be6c31fa807a62208a0_data, 1, g_ffx_vrs_imagegen_pass_wave64_1afbfca2cd353be6c31fa807a62208a0_CBVResourceNames, g_ffx_vrs_imagegen_pass_wave64_1afbfca2cd353be6c31fa807a62208a0_CBVResourceBindings, g_ffx_vrs_imagegen_pass_wave64_1afbfca2cd353be6c31fa807a62208a0_CBVResourceCounts, g_ffx_vrs_imagegen_pass_wave64_1afbfca2cd353be6c31fa807a62208a0_CBVResourceSets, 2, g_ffx_vrs_imagegen_pass_wave64_1afbfca2cd353be6c31fa807a62208a0_TextureSRVResourceNames, g_ffx_vrs_imagegen_pass_wave64_1afbfca2cd353be6c31fa807a62208a0_TextureSRVResourceBindings, g_ffx_vrs_imagegen_pass_wave64_1afbfca2cd353be6c31fa807a62208a0_TextureSRVResourceCounts, g_ffx_vrs_imagegen_pass_wave64_1afbfca2cd353be6c31fa807a62208a0_TextureSRVResourceSets, 1, g_ffx_vrs_imagegen_pass_wave64_1afbfca2cd353be6c31fa807a62208a0_TextureUAVResourceNames, g_ffx_vrs_imagegen_pass_wave64_1afbfca2cd353be6c31fa807a62208a0_TextureUAVResourceBindings, g_ffx_vrs_imagegen_pass_wave64_1afbfca2cd353be6c31fa807a62208a0_TextureUAVResourceCounts, g_ffx_vrs_imagegen_pass_wave64_1afbfca2cd353be6c31fa807a62208a0_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_vrs_imagegen_pass_wave64_56a03f6f9e1f5396a5767d6ddebe380d_size, g_ffx_vrs_imagegen_pass_wave64_56a03f6f9e1f5396a5767d6ddebe380d_data, 1, g_ffx_vrs_imagegen_pass_wave64_56a03f6f9e1f5396a5767d6ddebe380d_CBVResourceNames, g_ffx_vrs_imagegen_pass_wave64_56a03f6f9e1f5396a5767d6ddebe380d_CBVResourceBindings, g_ffx_vrs_imagegen_pass_wave64_56a03f6f9e1f5396a5767d6ddebe380d_CBVResourceCounts, g_ffx_vrs_imagegen_pass_wave64_56a03f6f9e1f5396a5767d6ddebe380d_CBVResourceSets, 2, g_ffx_vrs_imagegen_pass_wave64_56a03f6f9e1f5396a5767d6ddebe380d_TextureSRVResourceNames, g_ffx_vrs_imagegen_pass_wave64_56a03f6f9e1f5396a5767d6ddebe380d_TextureSRVResourceBindings, g_ffx_vrs_imagegen_pass_wave64_56a03f6f9e1f5396a5767d6ddebe380d_TextureSRVResourceCounts, g_ffx_vrs_imagegen_pass_wave64_56a03f6f9e1f5396a5767d6ddebe380d_TextureSRVResourceSets, 1, g_ffx_vrs_imagegen_pass_wave64_56a03f6f9e1f5396a5767d6ddebe380d_TextureUAVResourceNames, g_ffx_vrs_imagegen_pass_wave64_56a03f6f9e1f5396a5767d6ddebe380d_TextureUAVResourceBindings, g_ffx_vrs_imagegen_pass_wave64_56a03f6f9e1f5396a5767d6ddebe380d_TextureUAVResourceCounts, g_ffx_vrs_imagegen_pass_wave64_56a03f6f9e1f5396a5767d6ddebe380d_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_vrs_imagegen_pass_wave64_8e4ce92280acd3dfc4f1de311eb0580b_size, g_ffx_vrs_imagegen_pass_wave64_8e4ce92280acd3dfc4f1de311eb0580b_data, 1, g_ffx_vrs_imagegen_pass_wave64_8e4ce92280acd3dfc4f1de311eb0580b_CBVResourceNames, g_ffx_vrs_imagegen_pass_wave64_8e4ce92280acd3dfc4f1de311eb0580b_CBVResourceBindings, g_ffx_vrs_imagegen_pass_wave64_8e4ce92280acd3dfc4f1de311eb0580b_CBVResourceCounts, g_ffx_vrs_imagegen_pass_wave64_8e4ce92280acd3dfc4f1de311eb0580b_CBVResourceSets, 2, g_ffx_vrs_imagegen_pass_wave64_8e4ce92280acd3dfc4f1de311eb0580b_TextureSRVResourceNames, g_ffx_vrs_imagegen_pass_wave64_8e4ce92280acd3dfc4f1de311eb0580b_TextureSRVResourceBindings, g_ffx_vrs_imagegen_pass_wave64_8e4ce92280acd3dfc4f1de311eb0580b_TextureSRVResourceCounts, g_ffx_vrs_imagegen_pass_wave64_8e4ce92280acd3dfc4f1de311eb0580b_TextureSRVResourceSets, 1, g_ffx_vrs_imagegen_pass_wave64_8e4ce92280acd3dfc4f1de311eb0580b_TextureUAVResourceNames, g_ffx_vrs_imagegen_pass_wave64_8e4ce92280acd3dfc4f1de311eb0580b_TextureUAVResourceBindings, g_ffx_vrs_imagegen_pass_wave64_8e4ce92280acd3dfc4f1de311eb0580b_TextureUAVResourceCounts, g_ffx_vrs_imagegen_pass_wave64_8e4ce92280acd3dfc4f1de311eb0580b_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_vrs_imagegen_pass_wave64_ac8addbfa47504857f9e74fce56fe4b7_size, g_ffx_vrs_imagegen_pass_wave64_ac8addbfa47504857f9e74fce56fe4b7_data, 1, g_ffx_vrs_imagegen_pass_wave64_ac8addbfa47504857f9e74fce56fe4b7_CBVResourceNames, g_ffx_vrs_imagegen_pass_wave64_ac8addbfa47504857f9e74fce56fe4b7_CBVResourceBindings, g_ffx_vrs_imagegen_pass_wave64_ac8addbfa47504857f9e74fce56fe4b7_CBVResourceCounts, g_ffx_vrs_imagegen_pass_wave64_ac8addbfa47504857f9e74fce56fe4b7_CBVResourceSets, 2, g_ffx_vrs_imagegen_pass_wave64_ac8addbfa47504857f9e74fce56fe4b7_TextureSRVResourceNames, g_ffx_vrs_imagegen_pass_wave64_ac8addbfa47504857f9e74fce56fe4b7_TextureSRVResourceBindings, g_ffx_vrs_imagegen_pass_wave64_ac8addbfa47504857f9e74fce56fe4b7_TextureSRVResourceCounts, g_ffx_vrs_imagegen_pass_wave64_ac8addbfa47504857f9e74fce56fe4b7_TextureSRVResourceSets, 1, g_ffx_vrs_imagegen_pass_wave64_ac8addbfa47504857f9e74fce56fe4b7_TextureUAVResourceNames, g_ffx_vrs_imagegen_pass_wave64_ac8addbfa47504857f9e74fce56fe4b7_TextureUAVResourceBindings, g_ffx_vrs_imagegen_pass_wave64_ac8addbfa47504857f9e74fce56fe4b7_TextureUAVResourceCounts, g_ffx_vrs_imagegen_pass_wave64_ac8addbfa47504857f9e74fce56fe4b7_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};


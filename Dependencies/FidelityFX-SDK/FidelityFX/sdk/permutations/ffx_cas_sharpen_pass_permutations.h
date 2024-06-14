#include "ffx_cas_sharpen_pass_9102acf8867fa2fa002b6c2bb9b67813.h"
#include "ffx_cas_sharpen_pass_a33ae44ae22477131a379b703e383264.h"
#include "ffx_cas_sharpen_pass_3ad7e3c0030e134c7f746753a7a2b2f5.h"
#include "ffx_cas_sharpen_pass_10bb96e410a90edecde1fa977ebe1a20.h"
#include "ffx_cas_sharpen_pass_2ff92cac3c980a5172bea9a0ebd58c8d.h"
#include "ffx_cas_sharpen_pass_94deaa36d9f911cf8338829f771a4451.h"
#include "ffx_cas_sharpen_pass_732b7e7a488ee20f156397ec732202d9.h"
#include "ffx_cas_sharpen_pass_a815b565d207681093d2aca8e5d43390.h"
#include "ffx_cas_sharpen_pass_34f572d4a633e2d7876dcdf102f34a4b.h"
#include "ffx_cas_sharpen_pass_af668f91c65a5976c01b99f55e42b9e0.h"

typedef union ffx_cas_sharpen_pass_PermutationKey {
    struct {
        uint32_t FFX_CAS_OPTION_SHARPEN_ONLY : 1;
        uint32_t FFX_CAS_COLOR_SPACE_CONVERSION : 3;
    };
    uint32_t index;
} ffx_cas_sharpen_pass_PermutationKey;

typedef struct ffx_cas_sharpen_pass_PermutationInfo {
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
} ffx_cas_sharpen_pass_PermutationInfo;

static const uint32_t g_ffx_cas_sharpen_pass_IndirectionTable[] = {
    2,
    1,
    5,
    4,
    7,
    6,
    3,
    0,
    9,
    8,
    0,
    0,
    0,
    0,
    0,
    0,
};

static const ffx_cas_sharpen_pass_PermutationInfo g_ffx_cas_sharpen_pass_PermutationInfo[] = {
    { g_ffx_cas_sharpen_pass_9102acf8867fa2fa002b6c2bb9b67813_size, g_ffx_cas_sharpen_pass_9102acf8867fa2fa002b6c2bb9b67813_data, 1, g_ffx_cas_sharpen_pass_9102acf8867fa2fa002b6c2bb9b67813_CBVResourceNames, g_ffx_cas_sharpen_pass_9102acf8867fa2fa002b6c2bb9b67813_CBVResourceBindings, g_ffx_cas_sharpen_pass_9102acf8867fa2fa002b6c2bb9b67813_CBVResourceCounts, g_ffx_cas_sharpen_pass_9102acf8867fa2fa002b6c2bb9b67813_CBVResourceSets, 1, g_ffx_cas_sharpen_pass_9102acf8867fa2fa002b6c2bb9b67813_TextureSRVResourceNames, g_ffx_cas_sharpen_pass_9102acf8867fa2fa002b6c2bb9b67813_TextureSRVResourceBindings, g_ffx_cas_sharpen_pass_9102acf8867fa2fa002b6c2bb9b67813_TextureSRVResourceCounts, g_ffx_cas_sharpen_pass_9102acf8867fa2fa002b6c2bb9b67813_TextureSRVResourceSets, 1, g_ffx_cas_sharpen_pass_9102acf8867fa2fa002b6c2bb9b67813_TextureUAVResourceNames, g_ffx_cas_sharpen_pass_9102acf8867fa2fa002b6c2bb9b67813_TextureUAVResourceBindings, g_ffx_cas_sharpen_pass_9102acf8867fa2fa002b6c2bb9b67813_TextureUAVResourceCounts, g_ffx_cas_sharpen_pass_9102acf8867fa2fa002b6c2bb9b67813_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_cas_sharpen_pass_a33ae44ae22477131a379b703e383264_size, g_ffx_cas_sharpen_pass_a33ae44ae22477131a379b703e383264_data, 1, g_ffx_cas_sharpen_pass_a33ae44ae22477131a379b703e383264_CBVResourceNames, g_ffx_cas_sharpen_pass_a33ae44ae22477131a379b703e383264_CBVResourceBindings, g_ffx_cas_sharpen_pass_a33ae44ae22477131a379b703e383264_CBVResourceCounts, g_ffx_cas_sharpen_pass_a33ae44ae22477131a379b703e383264_CBVResourceSets, 1, g_ffx_cas_sharpen_pass_a33ae44ae22477131a379b703e383264_TextureSRVResourceNames, g_ffx_cas_sharpen_pass_a33ae44ae22477131a379b703e383264_TextureSRVResourceBindings, g_ffx_cas_sharpen_pass_a33ae44ae22477131a379b703e383264_TextureSRVResourceCounts, g_ffx_cas_sharpen_pass_a33ae44ae22477131a379b703e383264_TextureSRVResourceSets, 1, g_ffx_cas_sharpen_pass_a33ae44ae22477131a379b703e383264_TextureUAVResourceNames, g_ffx_cas_sharpen_pass_a33ae44ae22477131a379b703e383264_TextureUAVResourceBindings, g_ffx_cas_sharpen_pass_a33ae44ae22477131a379b703e383264_TextureUAVResourceCounts, g_ffx_cas_sharpen_pass_a33ae44ae22477131a379b703e383264_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_cas_sharpen_pass_3ad7e3c0030e134c7f746753a7a2b2f5_size, g_ffx_cas_sharpen_pass_3ad7e3c0030e134c7f746753a7a2b2f5_data, 1, g_ffx_cas_sharpen_pass_3ad7e3c0030e134c7f746753a7a2b2f5_CBVResourceNames, g_ffx_cas_sharpen_pass_3ad7e3c0030e134c7f746753a7a2b2f5_CBVResourceBindings, g_ffx_cas_sharpen_pass_3ad7e3c0030e134c7f746753a7a2b2f5_CBVResourceCounts, g_ffx_cas_sharpen_pass_3ad7e3c0030e134c7f746753a7a2b2f5_CBVResourceSets, 1, g_ffx_cas_sharpen_pass_3ad7e3c0030e134c7f746753a7a2b2f5_TextureSRVResourceNames, g_ffx_cas_sharpen_pass_3ad7e3c0030e134c7f746753a7a2b2f5_TextureSRVResourceBindings, g_ffx_cas_sharpen_pass_3ad7e3c0030e134c7f746753a7a2b2f5_TextureSRVResourceCounts, g_ffx_cas_sharpen_pass_3ad7e3c0030e134c7f746753a7a2b2f5_TextureSRVResourceSets, 1, g_ffx_cas_sharpen_pass_3ad7e3c0030e134c7f746753a7a2b2f5_TextureUAVResourceNames, g_ffx_cas_sharpen_pass_3ad7e3c0030e134c7f746753a7a2b2f5_TextureUAVResourceBindings, g_ffx_cas_sharpen_pass_3ad7e3c0030e134c7f746753a7a2b2f5_TextureUAVResourceCounts, g_ffx_cas_sharpen_pass_3ad7e3c0030e134c7f746753a7a2b2f5_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_cas_sharpen_pass_10bb96e410a90edecde1fa977ebe1a20_size, g_ffx_cas_sharpen_pass_10bb96e410a90edecde1fa977ebe1a20_data, 1, g_ffx_cas_sharpen_pass_10bb96e410a90edecde1fa977ebe1a20_CBVResourceNames, g_ffx_cas_sharpen_pass_10bb96e410a90edecde1fa977ebe1a20_CBVResourceBindings, g_ffx_cas_sharpen_pass_10bb96e410a90edecde1fa977ebe1a20_CBVResourceCounts, g_ffx_cas_sharpen_pass_10bb96e410a90edecde1fa977ebe1a20_CBVResourceSets, 1, g_ffx_cas_sharpen_pass_10bb96e410a90edecde1fa977ebe1a20_TextureSRVResourceNames, g_ffx_cas_sharpen_pass_10bb96e410a90edecde1fa977ebe1a20_TextureSRVResourceBindings, g_ffx_cas_sharpen_pass_10bb96e410a90edecde1fa977ebe1a20_TextureSRVResourceCounts, g_ffx_cas_sharpen_pass_10bb96e410a90edecde1fa977ebe1a20_TextureSRVResourceSets, 1, g_ffx_cas_sharpen_pass_10bb96e410a90edecde1fa977ebe1a20_TextureUAVResourceNames, g_ffx_cas_sharpen_pass_10bb96e410a90edecde1fa977ebe1a20_TextureUAVResourceBindings, g_ffx_cas_sharpen_pass_10bb96e410a90edecde1fa977ebe1a20_TextureUAVResourceCounts, g_ffx_cas_sharpen_pass_10bb96e410a90edecde1fa977ebe1a20_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_cas_sharpen_pass_2ff92cac3c980a5172bea9a0ebd58c8d_size, g_ffx_cas_sharpen_pass_2ff92cac3c980a5172bea9a0ebd58c8d_data, 1, g_ffx_cas_sharpen_pass_2ff92cac3c980a5172bea9a0ebd58c8d_CBVResourceNames, g_ffx_cas_sharpen_pass_2ff92cac3c980a5172bea9a0ebd58c8d_CBVResourceBindings, g_ffx_cas_sharpen_pass_2ff92cac3c980a5172bea9a0ebd58c8d_CBVResourceCounts, g_ffx_cas_sharpen_pass_2ff92cac3c980a5172bea9a0ebd58c8d_CBVResourceSets, 1, g_ffx_cas_sharpen_pass_2ff92cac3c980a5172bea9a0ebd58c8d_TextureSRVResourceNames, g_ffx_cas_sharpen_pass_2ff92cac3c980a5172bea9a0ebd58c8d_TextureSRVResourceBindings, g_ffx_cas_sharpen_pass_2ff92cac3c980a5172bea9a0ebd58c8d_TextureSRVResourceCounts, g_ffx_cas_sharpen_pass_2ff92cac3c980a5172bea9a0ebd58c8d_TextureSRVResourceSets, 1, g_ffx_cas_sharpen_pass_2ff92cac3c980a5172bea9a0ebd58c8d_TextureUAVResourceNames, g_ffx_cas_sharpen_pass_2ff92cac3c980a5172bea9a0ebd58c8d_TextureUAVResourceBindings, g_ffx_cas_sharpen_pass_2ff92cac3c980a5172bea9a0ebd58c8d_TextureUAVResourceCounts, g_ffx_cas_sharpen_pass_2ff92cac3c980a5172bea9a0ebd58c8d_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_cas_sharpen_pass_94deaa36d9f911cf8338829f771a4451_size, g_ffx_cas_sharpen_pass_94deaa36d9f911cf8338829f771a4451_data, 1, g_ffx_cas_sharpen_pass_94deaa36d9f911cf8338829f771a4451_CBVResourceNames, g_ffx_cas_sharpen_pass_94deaa36d9f911cf8338829f771a4451_CBVResourceBindings, g_ffx_cas_sharpen_pass_94deaa36d9f911cf8338829f771a4451_CBVResourceCounts, g_ffx_cas_sharpen_pass_94deaa36d9f911cf8338829f771a4451_CBVResourceSets, 1, g_ffx_cas_sharpen_pass_94deaa36d9f911cf8338829f771a4451_TextureSRVResourceNames, g_ffx_cas_sharpen_pass_94deaa36d9f911cf8338829f771a4451_TextureSRVResourceBindings, g_ffx_cas_sharpen_pass_94deaa36d9f911cf8338829f771a4451_TextureSRVResourceCounts, g_ffx_cas_sharpen_pass_94deaa36d9f911cf8338829f771a4451_TextureSRVResourceSets, 1, g_ffx_cas_sharpen_pass_94deaa36d9f911cf8338829f771a4451_TextureUAVResourceNames, g_ffx_cas_sharpen_pass_94deaa36d9f911cf8338829f771a4451_TextureUAVResourceBindings, g_ffx_cas_sharpen_pass_94deaa36d9f911cf8338829f771a4451_TextureUAVResourceCounts, g_ffx_cas_sharpen_pass_94deaa36d9f911cf8338829f771a4451_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_cas_sharpen_pass_732b7e7a488ee20f156397ec732202d9_size, g_ffx_cas_sharpen_pass_732b7e7a488ee20f156397ec732202d9_data, 1, g_ffx_cas_sharpen_pass_732b7e7a488ee20f156397ec732202d9_CBVResourceNames, g_ffx_cas_sharpen_pass_732b7e7a488ee20f156397ec732202d9_CBVResourceBindings, g_ffx_cas_sharpen_pass_732b7e7a488ee20f156397ec732202d9_CBVResourceCounts, g_ffx_cas_sharpen_pass_732b7e7a488ee20f156397ec732202d9_CBVResourceSets, 1, g_ffx_cas_sharpen_pass_732b7e7a488ee20f156397ec732202d9_TextureSRVResourceNames, g_ffx_cas_sharpen_pass_732b7e7a488ee20f156397ec732202d9_TextureSRVResourceBindings, g_ffx_cas_sharpen_pass_732b7e7a488ee20f156397ec732202d9_TextureSRVResourceCounts, g_ffx_cas_sharpen_pass_732b7e7a488ee20f156397ec732202d9_TextureSRVResourceSets, 1, g_ffx_cas_sharpen_pass_732b7e7a488ee20f156397ec732202d9_TextureUAVResourceNames, g_ffx_cas_sharpen_pass_732b7e7a488ee20f156397ec732202d9_TextureUAVResourceBindings, g_ffx_cas_sharpen_pass_732b7e7a488ee20f156397ec732202d9_TextureUAVResourceCounts, g_ffx_cas_sharpen_pass_732b7e7a488ee20f156397ec732202d9_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_cas_sharpen_pass_a815b565d207681093d2aca8e5d43390_size, g_ffx_cas_sharpen_pass_a815b565d207681093d2aca8e5d43390_data, 1, g_ffx_cas_sharpen_pass_a815b565d207681093d2aca8e5d43390_CBVResourceNames, g_ffx_cas_sharpen_pass_a815b565d207681093d2aca8e5d43390_CBVResourceBindings, g_ffx_cas_sharpen_pass_a815b565d207681093d2aca8e5d43390_CBVResourceCounts, g_ffx_cas_sharpen_pass_a815b565d207681093d2aca8e5d43390_CBVResourceSets, 1, g_ffx_cas_sharpen_pass_a815b565d207681093d2aca8e5d43390_TextureSRVResourceNames, g_ffx_cas_sharpen_pass_a815b565d207681093d2aca8e5d43390_TextureSRVResourceBindings, g_ffx_cas_sharpen_pass_a815b565d207681093d2aca8e5d43390_TextureSRVResourceCounts, g_ffx_cas_sharpen_pass_a815b565d207681093d2aca8e5d43390_TextureSRVResourceSets, 1, g_ffx_cas_sharpen_pass_a815b565d207681093d2aca8e5d43390_TextureUAVResourceNames, g_ffx_cas_sharpen_pass_a815b565d207681093d2aca8e5d43390_TextureUAVResourceBindings, g_ffx_cas_sharpen_pass_a815b565d207681093d2aca8e5d43390_TextureUAVResourceCounts, g_ffx_cas_sharpen_pass_a815b565d207681093d2aca8e5d43390_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_cas_sharpen_pass_34f572d4a633e2d7876dcdf102f34a4b_size, g_ffx_cas_sharpen_pass_34f572d4a633e2d7876dcdf102f34a4b_data, 1, g_ffx_cas_sharpen_pass_34f572d4a633e2d7876dcdf102f34a4b_CBVResourceNames, g_ffx_cas_sharpen_pass_34f572d4a633e2d7876dcdf102f34a4b_CBVResourceBindings, g_ffx_cas_sharpen_pass_34f572d4a633e2d7876dcdf102f34a4b_CBVResourceCounts, g_ffx_cas_sharpen_pass_34f572d4a633e2d7876dcdf102f34a4b_CBVResourceSets, 1, g_ffx_cas_sharpen_pass_34f572d4a633e2d7876dcdf102f34a4b_TextureSRVResourceNames, g_ffx_cas_sharpen_pass_34f572d4a633e2d7876dcdf102f34a4b_TextureSRVResourceBindings, g_ffx_cas_sharpen_pass_34f572d4a633e2d7876dcdf102f34a4b_TextureSRVResourceCounts, g_ffx_cas_sharpen_pass_34f572d4a633e2d7876dcdf102f34a4b_TextureSRVResourceSets, 1, g_ffx_cas_sharpen_pass_34f572d4a633e2d7876dcdf102f34a4b_TextureUAVResourceNames, g_ffx_cas_sharpen_pass_34f572d4a633e2d7876dcdf102f34a4b_TextureUAVResourceBindings, g_ffx_cas_sharpen_pass_34f572d4a633e2d7876dcdf102f34a4b_TextureUAVResourceCounts, g_ffx_cas_sharpen_pass_34f572d4a633e2d7876dcdf102f34a4b_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
    { g_ffx_cas_sharpen_pass_af668f91c65a5976c01b99f55e42b9e0_size, g_ffx_cas_sharpen_pass_af668f91c65a5976c01b99f55e42b9e0_data, 1, g_ffx_cas_sharpen_pass_af668f91c65a5976c01b99f55e42b9e0_CBVResourceNames, g_ffx_cas_sharpen_pass_af668f91c65a5976c01b99f55e42b9e0_CBVResourceBindings, g_ffx_cas_sharpen_pass_af668f91c65a5976c01b99f55e42b9e0_CBVResourceCounts, g_ffx_cas_sharpen_pass_af668f91c65a5976c01b99f55e42b9e0_CBVResourceSets, 1, g_ffx_cas_sharpen_pass_af668f91c65a5976c01b99f55e42b9e0_TextureSRVResourceNames, g_ffx_cas_sharpen_pass_af668f91c65a5976c01b99f55e42b9e0_TextureSRVResourceBindings, g_ffx_cas_sharpen_pass_af668f91c65a5976c01b99f55e42b9e0_TextureSRVResourceCounts, g_ffx_cas_sharpen_pass_af668f91c65a5976c01b99f55e42b9e0_TextureSRVResourceSets, 1, g_ffx_cas_sharpen_pass_af668f91c65a5976c01b99f55e42b9e0_TextureUAVResourceNames, g_ffx_cas_sharpen_pass_af668f91c65a5976c01b99f55e42b9e0_TextureUAVResourceBindings, g_ffx_cas_sharpen_pass_af668f91c65a5976c01b99f55e42b9e0_TextureUAVResourceCounts, g_ffx_cas_sharpen_pass_af668f91c65a5976c01b99f55e42b9e0_TextureUAVResourceSets, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
};


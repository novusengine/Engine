// ffx_parallelsort_reduce_pass_16bit_5b4666dc306868898ea8393dcea69bcb.h.
// Auto generated by FidelityFX-SC.

static const char* g_ffx_parallelsort_reduce_pass_16bit_5b4666dc306868898ea8393dcea69bcb_CBVResourceNames[] = {  "cbParallelSort", };
static const uint32_t g_ffx_parallelsort_reduce_pass_16bit_5b4666dc306868898ea8393dcea69bcb_CBVResourceBindings[] = {  3000, };
static const uint32_t g_ffx_parallelsort_reduce_pass_16bit_5b4666dc306868898ea8393dcea69bcb_CBVResourceCounts[] = {  1, };
static const uint32_t g_ffx_parallelsort_reduce_pass_16bit_5b4666dc306868898ea8393dcea69bcb_CBVResourceSets[] = {  0, };

static const char* g_ffx_parallelsort_reduce_pass_16bit_5b4666dc306868898ea8393dcea69bcb_BufferUAVResourceNames[] = {  "rw_sum_table", "rw_reduce_table", };
static const uint32_t g_ffx_parallelsort_reduce_pass_16bit_5b4666dc306868898ea8393dcea69bcb_BufferUAVResourceBindings[] = {  2000, 2001, };
static const uint32_t g_ffx_parallelsort_reduce_pass_16bit_5b4666dc306868898ea8393dcea69bcb_BufferUAVResourceCounts[] = {  1, 1, };
static const uint32_t g_ffx_parallelsort_reduce_pass_16bit_5b4666dc306868898ea8393dcea69bcb_BufferUAVResourceSets[] = {  0, 0, };

static const uint32_t g_ffx_parallelsort_reduce_pass_16bit_5b4666dc306868898ea8393dcea69bcb_size = 3488;

static const unsigned char g_ffx_parallelsort_reduce_pass_16bit_5b4666dc306868898ea8393dcea69bcb_data[] = {
0x03,0x02,0x23,0x07,0x00,0x03,0x01,0x00,0x0b,0x00,0x08,0x00,0x82,0x01,0x00,0x00,
0x00,0x00,0x00,0x00,0x11,0x00,0x02,0x00,0x01,0x00,0x00,0x00,0x11,0x00,0x02,0x00,
0x3d,0x00,0x00,0x00,0x11,0x00,0x02,0x00,0x3f,0x00,0x00,0x00,0x11,0x00,0x02,0x00,
0x47,0x11,0x00,0x00,0x0a,0x00,0x07,0x00,0x53,0x50,0x56,0x5f,0x4b,0x48,0x52,0x5f,
0x73,0x68,0x61,0x64,0x65,0x72,0x5f,0x62,0x61,0x6c,0x6c,0x6f,0x74,0x00,0x00,0x00,
0x0b,0x00,0x06,0x00,0x01,0x00,0x00,0x00,0x47,0x4c,0x53,0x4c,0x2e,0x73,0x74,0x64,
0x2e,0x34,0x35,0x30,0x00,0x00,0x00,0x00,0x0e,0x00,0x03,0x00,0x00,0x00,0x00,0x00,
0x01,0x00,0x00,0x00,0x0f,0x00,0x08,0x00,0x05,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
0x6d,0x61,0x69,0x6e,0x00,0x00,0x00,0x00,0x68,0x00,0x00,0x00,0xd8,0x00,0x00,0x00,
0xd9,0x00,0x00,0x00,0x10,0x00,0x06,0x00,0x04,0x00,0x00,0x00,0x11,0x00,0x00,0x00,
0x80,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x03,0x00,0x03,0x00,
0x02,0x00,0x00,0x00,0xc2,0x01,0x00,0x00,0x04,0x00,0x07,0x00,0x47,0x4c,0x5f,0x41,
0x52,0x42,0x5f,0x73,0x68,0x61,0x64,0x65,0x72,0x5f,0x62,0x61,0x6c,0x6c,0x6f,0x74,
0x00,0x00,0x00,0x00,0x04,0x00,0x08,0x00,0x47,0x4c,0x5f,0x45,0x58,0x54,0x5f,0x73,
0x68,0x61,0x64,0x65,0x72,0x5f,0x31,0x36,0x62,0x69,0x74,0x5f,0x73,0x74,0x6f,0x72,
0x61,0x67,0x65,0x00,0x04,0x00,0x0b,0x00,0x47,0x4c,0x5f,0x45,0x58,0x54,0x5f,0x73,
0x68,0x61,0x64,0x65,0x72,0x5f,0x65,0x78,0x70,0x6c,0x69,0x63,0x69,0x74,0x5f,0x61,
0x72,0x69,0x74,0x68,0x6d,0x65,0x74,0x69,0x63,0x5f,0x74,0x79,0x70,0x65,0x73,0x00,
0x04,0x00,0x0a,0x00,0x47,0x4c,0x5f,0x47,0x4f,0x4f,0x47,0x4c,0x45,0x5f,0x63,0x70,
0x70,0x5f,0x73,0x74,0x79,0x6c,0x65,0x5f,0x6c,0x69,0x6e,0x65,0x5f,0x64,0x69,0x72,
0x65,0x63,0x74,0x69,0x76,0x65,0x00,0x00,0x04,0x00,0x08,0x00,0x47,0x4c,0x5f,0x47,
0x4f,0x4f,0x47,0x4c,0x45,0x5f,0x69,0x6e,0x63,0x6c,0x75,0x64,0x65,0x5f,0x64,0x69,
0x72,0x65,0x63,0x74,0x69,0x76,0x65,0x00,0x04,0x00,0x0a,0x00,0x47,0x4c,0x5f,0x4b,
0x48,0x52,0x5f,0x73,0x68,0x61,0x64,0x65,0x72,0x5f,0x73,0x75,0x62,0x67,0x72,0x6f,
0x75,0x70,0x5f,0x61,0x72,0x69,0x74,0x68,0x6d,0x65,0x74,0x69,0x63,0x00,0x00,0x00,
0x04,0x00,0x09,0x00,0x47,0x4c,0x5f,0x4b,0x48,0x52,0x5f,0x73,0x68,0x61,0x64,0x65,
0x72,0x5f,0x73,0x75,0x62,0x67,0x72,0x6f,0x75,0x70,0x5f,0x62,0x61,0x73,0x69,0x63,
0x00,0x00,0x00,0x00,0x04,0x00,0x08,0x00,0x47,0x4c,0x5f,0x4b,0x48,0x52,0x5f,0x73,
0x68,0x61,0x64,0x65,0x72,0x5f,0x73,0x75,0x62,0x67,0x72,0x6f,0x75,0x70,0x5f,0x71,
0x75,0x61,0x64,0x00,0x05,0x00,0x04,0x00,0x04,0x00,0x00,0x00,0x6d,0x61,0x69,0x6e,
0x00,0x00,0x00,0x00,0x05,0x00,0x07,0x00,0x2f,0x00,0x00,0x00,0x63,0x62,0x50,0x61,
0x72,0x61,0x6c,0x6c,0x65,0x6c,0x53,0x6f,0x72,0x74,0x5f,0x74,0x00,0x00,0x00,0x00,
0x06,0x00,0x05,0x00,0x2f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x6e,0x75,0x6d,0x4b,
0x65,0x79,0x73,0x00,0x06,0x00,0x09,0x00,0x2f,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
0x6e,0x75,0x6d,0x42,0x6c,0x6f,0x63,0x6b,0x73,0x50,0x65,0x72,0x54,0x68,0x72,0x65,
0x61,0x64,0x47,0x72,0x6f,0x75,0x70,0x00,0x06,0x00,0x07,0x00,0x2f,0x00,0x00,0x00,
0x02,0x00,0x00,0x00,0x6e,0x75,0x6d,0x54,0x68,0x72,0x65,0x61,0x64,0x47,0x72,0x6f,
0x75,0x70,0x73,0x00,0x06,0x00,0x0c,0x00,0x2f,0x00,0x00,0x00,0x03,0x00,0x00,0x00,
0x6e,0x75,0x6d,0x54,0x68,0x72,0x65,0x61,0x64,0x47,0x72,0x6f,0x75,0x70,0x73,0x57,
0x69,0x74,0x68,0x41,0x64,0x64,0x69,0x74,0x69,0x6f,0x6e,0x61,0x6c,0x42,0x6c,0x6f,
0x63,0x6b,0x73,0x00,0x06,0x00,0x0a,0x00,0x2f,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
0x6e,0x75,0x6d,0x52,0x65,0x64,0x75,0x63,0x65,0x54,0x68,0x72,0x65,0x61,0x64,0x67,
0x72,0x6f,0x75,0x70,0x50,0x65,0x72,0x42,0x69,0x6e,0x00,0x00,0x06,0x00,0x07,0x00,
0x2f,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x6e,0x75,0x6d,0x53,0x63,0x61,0x6e,0x56,
0x61,0x6c,0x75,0x65,0x73,0x00,0x00,0x00,0x06,0x00,0x06,0x00,0x2f,0x00,0x00,0x00,
0x06,0x00,0x00,0x00,0x73,0x68,0x69,0x66,0x74,0x42,0x69,0x74,0x00,0x00,0x00,0x00,
0x06,0x00,0x05,0x00,0x2f,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x70,0x61,0x64,0x64,
0x69,0x6e,0x67,0x00,0x05,0x00,0x06,0x00,0x31,0x00,0x00,0x00,0x63,0x62,0x50,0x61,
0x72,0x61,0x6c,0x6c,0x65,0x6c,0x53,0x6f,0x72,0x74,0x00,0x00,0x05,0x00,0x08,0x00,
0x3e,0x00,0x00,0x00,0x50,0x61,0x72,0x61,0x6c,0x6c,0x65,0x6c,0x53,0x6f,0x72,0x74,
0x53,0x75,0x6d,0x54,0x61,0x62,0x6c,0x65,0x5f,0x74,0x00,0x00,0x06,0x00,0x06,0x00,
0x3e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x73,0x75,0x6d,0x5f,0x74,0x61,0x62,0x6c,
0x65,0x00,0x00,0x00,0x05,0x00,0x06,0x00,0x40,0x00,0x00,0x00,0x72,0x77,0x5f,0x73,
0x75,0x6d,0x5f,0x74,0x61,0x62,0x6c,0x65,0x00,0x00,0x00,0x00,0x05,0x00,0x09,0x00,
0x4b,0x00,0x00,0x00,0x50,0x61,0x72,0x61,0x6c,0x6c,0x65,0x6c,0x53,0x6f,0x72,0x74,
0x52,0x65,0x64,0x75,0x63,0x65,0x54,0x61,0x62,0x6c,0x65,0x5f,0x74,0x00,0x00,0x00,
0x06,0x00,0x07,0x00,0x4b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x72,0x65,0x64,0x75,
0x63,0x65,0x5f,0x74,0x61,0x62,0x6c,0x65,0x00,0x00,0x00,0x00,0x05,0x00,0x06,0x00,
0x4d,0x00,0x00,0x00,0x72,0x77,0x5f,0x72,0x65,0x64,0x75,0x63,0x65,0x5f,0x74,0x61,
0x62,0x6c,0x65,0x00,0x05,0x00,0x07,0x00,0x68,0x00,0x00,0x00,0x67,0x6c,0x5f,0x53,
0x75,0x62,0x47,0x72,0x6f,0x75,0x70,0x53,0x69,0x7a,0x65,0x41,0x52,0x42,0x00,0x00,
0x05,0x00,0x09,0x00,0x72,0x00,0x00,0x00,0x67,0x73,0x5f,0x46,0x46,0x58,0x5f,0x50,
0x41,0x52,0x41,0x4c,0x4c,0x45,0x4c,0x53,0x4f,0x52,0x54,0x5f,0x4c,0x44,0x53,0x53,
0x75,0x6d,0x73,0x00,0x05,0x00,0x08,0x00,0xd8,0x00,0x00,0x00,0x67,0x6c,0x5f,0x4c,
0x6f,0x63,0x61,0x6c,0x49,0x6e,0x76,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x49,0x44,
0x00,0x00,0x00,0x00,0x05,0x00,0x06,0x00,0xd9,0x00,0x00,0x00,0x67,0x6c,0x5f,0x57,
0x6f,0x72,0x6b,0x47,0x72,0x6f,0x75,0x70,0x49,0x44,0x00,0x00,0x48,0x00,0x05,0x00,
0x2f,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x48,0x00,0x05,0x00,0x2f,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x23,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x48,0x00,0x05,0x00,0x2f,0x00,0x00,0x00,0x02,0x00,0x00,0x00,
0x23,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x48,0x00,0x05,0x00,0x2f,0x00,0x00,0x00,
0x03,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x48,0x00,0x05,0x00,
0x2f,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x10,0x00,0x00,0x00,
0x48,0x00,0x05,0x00,0x2f,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x23,0x00,0x00,0x00,
0x14,0x00,0x00,0x00,0x48,0x00,0x05,0x00,0x2f,0x00,0x00,0x00,0x06,0x00,0x00,0x00,
0x23,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x48,0x00,0x05,0x00,0x2f,0x00,0x00,0x00,
0x07,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x1c,0x00,0x00,0x00,0x47,0x00,0x03,0x00,
0x2f,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x47,0x00,0x04,0x00,0x31,0x00,0x00,0x00,
0x22,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x47,0x00,0x04,0x00,0x31,0x00,0x00,0x00,
0x21,0x00,0x00,0x00,0xb8,0x0b,0x00,0x00,0x47,0x00,0x04,0x00,0x3d,0x00,0x00,0x00,
0x06,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x48,0x00,0x04,0x00,0x3e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x17,0x00,0x00,0x00,0x48,0x00,0x05,0x00,0x3e,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x47,0x00,0x03,0x00,
0x3e,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x47,0x00,0x04,0x00,0x40,0x00,0x00,0x00,
0x22,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x47,0x00,0x04,0x00,0x40,0x00,0x00,0x00,
0x21,0x00,0x00,0x00,0xd0,0x07,0x00,0x00,0x47,0x00,0x04,0x00,0x4a,0x00,0x00,0x00,
0x06,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x48,0x00,0x04,0x00,0x4b,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x17,0x00,0x00,0x00,0x48,0x00,0x05,0x00,0x4b,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x47,0x00,0x03,0x00,
0x4b,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x47,0x00,0x04,0x00,0x4d,0x00,0x00,0x00,
0x22,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x47,0x00,0x04,0x00,0x4d,0x00,0x00,0x00,
0x21,0x00,0x00,0x00,0xd1,0x07,0x00,0x00,0x47,0x00,0x04,0x00,0x68,0x00,0x00,0x00,
0x0b,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x47,0x00,0x04,0x00,0xd8,0x00,0x00,0x00,
0x0b,0x00,0x00,0x00,0x1b,0x00,0x00,0x00,0x47,0x00,0x04,0x00,0xd9,0x00,0x00,0x00,
0x0b,0x00,0x00,0x00,0x1a,0x00,0x00,0x00,0x47,0x00,0x04,0x00,0xef,0x00,0x00,0x00,
0x0b,0x00,0x00,0x00,0x19,0x00,0x00,0x00,0x13,0x00,0x02,0x00,0x02,0x00,0x00,0x00,
0x21,0x00,0x03,0x00,0x03,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x15,0x00,0x04,0x00,
0x06,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x15,0x00,0x04,0x00,
0x2e,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x1e,0x00,0x0a,0x00,
0x2f,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x2e,0x00,0x00,0x00,0x06,0x00,0x00,0x00,
0x06,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x06,0x00,0x00,0x00,
0x06,0x00,0x00,0x00,0x20,0x00,0x04,0x00,0x30,0x00,0x00,0x00,0x02,0x00,0x00,0x00,
0x2f,0x00,0x00,0x00,0x3b,0x00,0x04,0x00,0x30,0x00,0x00,0x00,0x31,0x00,0x00,0x00,
0x02,0x00,0x00,0x00,0x2b,0x00,0x04,0x00,0x2e,0x00,0x00,0x00,0x32,0x00,0x00,0x00,
0x02,0x00,0x00,0x00,0x20,0x00,0x04,0x00,0x33,0x00,0x00,0x00,0x02,0x00,0x00,0x00,
0x06,0x00,0x00,0x00,0x2b,0x00,0x04,0x00,0x2e,0x00,0x00,0x00,0x38,0x00,0x00,0x00,
0x04,0x00,0x00,0x00,0x1d,0x00,0x03,0x00,0x3d,0x00,0x00,0x00,0x06,0x00,0x00,0x00,
0x1e,0x00,0x03,0x00,0x3e,0x00,0x00,0x00,0x3d,0x00,0x00,0x00,0x20,0x00,0x04,0x00,
0x3f,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x3e,0x00,0x00,0x00,0x3b,0x00,0x04,0x00,
0x3f,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x2b,0x00,0x04,0x00,
0x2e,0x00,0x00,0x00,0x41,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x04,0x00,
0x43,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x2b,0x00,0x04,0x00,
0x06,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1d,0x00,0x03,0x00,
0x4a,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x1e,0x00,0x03,0x00,0x4b,0x00,0x00,0x00,
0x4a,0x00,0x00,0x00,0x20,0x00,0x04,0x00,0x4c,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,
0x4b,0x00,0x00,0x00,0x3b,0x00,0x04,0x00,0x4c,0x00,0x00,0x00,0x4d,0x00,0x00,0x00,
0x0c,0x00,0x00,0x00,0x2b,0x00,0x04,0x00,0x06,0x00,0x00,0x00,0x63,0x00,0x00,0x00,
0x03,0x00,0x00,0x00,0x20,0x00,0x04,0x00,0x67,0x00,0x00,0x00,0x01,0x00,0x00,0x00,
0x06,0x00,0x00,0x00,0x3b,0x00,0x04,0x00,0x67,0x00,0x00,0x00,0x68,0x00,0x00,0x00,
0x01,0x00,0x00,0x00,0x14,0x00,0x02,0x00,0x6b,0x00,0x00,0x00,0x2b,0x00,0x04,0x00,
0x06,0x00,0x00,0x00,0x6f,0x00,0x00,0x00,0x80,0x00,0x00,0x00,0x1c,0x00,0x04,0x00,
0x70,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x6f,0x00,0x00,0x00,0x20,0x00,0x04,0x00,
0x71,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x70,0x00,0x00,0x00,0x3b,0x00,0x04,0x00,
0x71,0x00,0x00,0x00,0x72,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x20,0x00,0x04,0x00,
0x75,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x2b,0x00,0x04,0x00,
0x06,0x00,0x00,0x00,0x77,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x2b,0x00,0x04,0x00,
0x06,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x48,0x0d,0x00,0x00,0x2b,0x00,0x04,0x00,
0x06,0x00,0x00,0x00,0x79,0x00,0x00,0x00,0x08,0x01,0x00,0x00,0x2b,0x00,0x04,0x00,
0x06,0x00,0x00,0x00,0x9e,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x2b,0x00,0x04,0x00,
0x2e,0x00,0x00,0x00,0xc1,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x17,0x00,0x04,0x00,
0xd6,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x20,0x00,0x04,0x00,
0xd7,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0xd6,0x00,0x00,0x00,0x3b,0x00,0x04,0x00,
0xd7,0x00,0x00,0x00,0xd8,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x3b,0x00,0x04,0x00,
0xd7,0x00,0x00,0x00,0xd9,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x2b,0x00,0x04,0x00,
0x06,0x00,0x00,0x00,0xee,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x2c,0x00,0x06,0x00,
0xd6,0x00,0x00,0x00,0xef,0x00,0x00,0x00,0x6f,0x00,0x00,0x00,0xee,0x00,0x00,0x00,
0xee,0x00,0x00,0x00,0x2b,0x00,0x04,0x00,0x06,0x00,0x00,0x00,0x7b,0x01,0x00,0x00,
0x00,0x02,0x00,0x00,0x36,0x00,0x05,0x00,0x02,0x00,0x00,0x00,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0xf8,0x00,0x02,0x00,0x05,0x00,0x00,0x00,
0x41,0x00,0x05,0x00,0x67,0x00,0x00,0x00,0xdb,0x00,0x00,0x00,0xd8,0x00,0x00,0x00,
0x47,0x00,0x00,0x00,0x3d,0x00,0x04,0x00,0x06,0x00,0x00,0x00,0xdc,0x00,0x00,0x00,
0xdb,0x00,0x00,0x00,0x41,0x00,0x05,0x00,0x67,0x00,0x00,0x00,0xde,0x00,0x00,0x00,
0xd9,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x3d,0x00,0x04,0x00,0x06,0x00,0x00,0x00,
0xdf,0x00,0x00,0x00,0xde,0x00,0x00,0x00,0x41,0x00,0x05,0x00,0x33,0x00,0x00,0x00,
0x3d,0x01,0x00,0x00,0x31,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x3d,0x00,0x04,0x00,
0x06,0x00,0x00,0x00,0x3e,0x01,0x00,0x00,0x3d,0x01,0x00,0x00,0x41,0x00,0x05,0x00,
0x33,0x00,0x00,0x00,0x44,0x01,0x00,0x00,0x31,0x00,0x00,0x00,0x32,0x00,0x00,0x00,
0x3d,0x00,0x04,0x00,0x06,0x00,0x00,0x00,0x45,0x01,0x00,0x00,0x44,0x01,0x00,0x00,
0x86,0x00,0x05,0x00,0x06,0x00,0x00,0x00,0x09,0x01,0x00,0x00,0xdf,0x00,0x00,0x00,
0x3e,0x01,0x00,0x00,0x84,0x00,0x05,0x00,0x06,0x00,0x00,0x00,0x0c,0x01,0x00,0x00,
0x09,0x01,0x00,0x00,0x45,0x01,0x00,0x00,0x89,0x00,0x05,0x00,0x06,0x00,0x00,0x00,
0x0f,0x01,0x00,0x00,0xdf,0x00,0x00,0x00,0x3e,0x01,0x00,0x00,0x84,0x00,0x05,0x00,
0x06,0x00,0x00,0x00,0x11,0x01,0x00,0x00,0x0f,0x01,0x00,0x00,0x7b,0x01,0x00,0x00,
0xf9,0x00,0x02,0x00,0x12,0x01,0x00,0x00,0xf8,0x00,0x02,0x00,0x12,0x01,0x00,0x00,
0xf5,0x00,0x07,0x00,0x06,0x00,0x00,0x00,0x7d,0x01,0x00,0x00,0x47,0x00,0x00,0x00,
0x05,0x00,0x00,0x00,0x29,0x01,0x00,0x00,0x2a,0x01,0x00,0x00,0xf5,0x00,0x07,0x00,
0x06,0x00,0x00,0x00,0x7c,0x01,0x00,0x00,0x47,0x00,0x00,0x00,0x05,0x00,0x00,0x00,
0x2c,0x01,0x00,0x00,0x2a,0x01,0x00,0x00,0xb0,0x00,0x05,0x00,0x6b,0x00,0x00,0x00,
0x15,0x01,0x00,0x00,0x7c,0x01,0x00,0x00,0x9e,0x00,0x00,0x00,0xf6,0x00,0x04,0x00,
0x2d,0x01,0x00,0x00,0x2a,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xfa,0x00,0x04,0x00,
0x15,0x01,0x00,0x00,0x16,0x01,0x00,0x00,0x2d,0x01,0x00,0x00,0xf8,0x00,0x02,0x00,
0x16,0x01,0x00,0x00,0x84,0x00,0x05,0x00,0x06,0x00,0x00,0x00,0x19,0x01,0x00,0x00,
0x7c,0x01,0x00,0x00,0x6f,0x00,0x00,0x00,0x80,0x00,0x05,0x00,0x06,0x00,0x00,0x00,
0x1a,0x01,0x00,0x00,0x11,0x01,0x00,0x00,0x19,0x01,0x00,0x00,0x80,0x00,0x05,0x00,
0x06,0x00,0x00,0x00,0x1c,0x01,0x00,0x00,0x1a,0x01,0x00,0x00,0xdc,0x00,0x00,0x00,
0xb0,0x00,0x05,0x00,0x6b,0x00,0x00,0x00,0x1f,0x01,0x00,0x00,0x1c,0x01,0x00,0x00,
0x45,0x01,0x00,0x00,0xf7,0x00,0x03,0x00,0x26,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
0xfa,0x00,0x04,0x00,0x1f,0x01,0x00,0x00,0x20,0x01,0x00,0x00,0x25,0x01,0x00,0x00,
0xf8,0x00,0x02,0x00,0x20,0x01,0x00,0x00,0x80,0x00,0x05,0x00,0x06,0x00,0x00,0x00,
0x23,0x01,0x00,0x00,0x0c,0x01,0x00,0x00,0x1c,0x01,0x00,0x00,0x41,0x00,0x06,0x00,
0x43,0x00,0x00,0x00,0x4e,0x01,0x00,0x00,0x40,0x00,0x00,0x00,0x41,0x00,0x00,0x00,
0x23,0x01,0x00,0x00,0x3d,0x00,0x04,0x00,0x06,0x00,0x00,0x00,0x4f,0x01,0x00,0x00,
0x4e,0x01,0x00,0x00,0xf9,0x00,0x02,0x00,0x26,0x01,0x00,0x00,0xf8,0x00,0x02,0x00,
0x25,0x01,0x00,0x00,0xf9,0x00,0x02,0x00,0x26,0x01,0x00,0x00,0xf8,0x00,0x02,0x00,
0x26,0x01,0x00,0x00,0xf5,0x00,0x07,0x00,0x06,0x00,0x00,0x00,0x81,0x01,0x00,0x00,
0x4f,0x01,0x00,0x00,0x20,0x01,0x00,0x00,0x47,0x00,0x00,0x00,0x25,0x01,0x00,0x00,
0x80,0x00,0x05,0x00,0x06,0x00,0x00,0x00,0x29,0x01,0x00,0x00,0x7d,0x01,0x00,0x00,
0x81,0x01,0x00,0x00,0xf9,0x00,0x02,0x00,0x2a,0x01,0x00,0x00,0xf8,0x00,0x02,0x00,
0x2a,0x01,0x00,0x00,0x80,0x00,0x05,0x00,0x06,0x00,0x00,0x00,0x2c,0x01,0x00,0x00,
0x7c,0x01,0x00,0x00,0xc1,0x00,0x00,0x00,0xf9,0x00,0x02,0x00,0x12,0x01,0x00,0x00,
0xf8,0x00,0x02,0x00,0x2d,0x01,0x00,0x00,0x5d,0x01,0x06,0x00,0x06,0x00,0x00,0x00,
0x56,0x01,0x00,0x00,0x63,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7d,0x01,0x00,0x00,
0x3d,0x00,0x04,0x00,0x06,0x00,0x00,0x00,0x58,0x01,0x00,0x00,0x68,0x00,0x00,0x00,
0x86,0x00,0x05,0x00,0x06,0x00,0x00,0x00,0x59,0x01,0x00,0x00,0xdc,0x00,0x00,0x00,
0x58,0x01,0x00,0x00,0x4d,0x01,0x04,0x00,0x6b,0x00,0x00,0x00,0x5a,0x01,0x00,0x00,
0x63,0x00,0x00,0x00,0xf7,0x00,0x03,0x00,0x5f,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
0xfa,0x00,0x04,0x00,0x5a,0x01,0x00,0x00,0x5b,0x01,0x00,0x00,0x5f,0x01,0x00,0x00,
0xf8,0x00,0x02,0x00,0x5b,0x01,0x00,0x00,0x41,0x00,0x05,0x00,0x75,0x00,0x00,0x00,
0x5e,0x01,0x00,0x00,0x72,0x00,0x00,0x00,0x59,0x01,0x00,0x00,0x3e,0x00,0x03,0x00,
0x5e,0x01,0x00,0x00,0x56,0x01,0x00,0x00,0xf9,0x00,0x02,0x00,0x5f,0x01,0x00,0x00,
0xf8,0x00,0x02,0x00,0x5f,0x01,0x00,0x00,0xe1,0x00,0x03,0x00,0x77,0x00,0x00,0x00,
0x78,0x00,0x00,0x00,0xe0,0x00,0x04,0x00,0x77,0x00,0x00,0x00,0x77,0x00,0x00,0x00,
0x79,0x00,0x00,0x00,0xaa,0x00,0x05,0x00,0x6b,0x00,0x00,0x00,0x61,0x01,0x00,0x00,
0x59,0x01,0x00,0x00,0x47,0x00,0x00,0x00,0xf7,0x00,0x03,0x00,0x6f,0x01,0x00,0x00,
0x00,0x00,0x00,0x00,0xfa,0x00,0x04,0x00,0x61,0x01,0x00,0x00,0x62,0x01,0x00,0x00,
0x6f,0x01,0x00,0x00,0xf8,0x00,0x02,0x00,0x62,0x01,0x00,0x00,0x86,0x00,0x05,0x00,
0x06,0x00,0x00,0x00,0x65,0x01,0x00,0x00,0x6f,0x00,0x00,0x00,0x58,0x01,0x00,0x00,
0xb0,0x00,0x05,0x00,0x6b,0x00,0x00,0x00,0x66,0x01,0x00,0x00,0xdc,0x00,0x00,0x00,
0x65,0x01,0x00,0x00,0xf7,0x00,0x03,0x00,0x6c,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
0xfa,0x00,0x04,0x00,0x66,0x01,0x00,0x00,0x67,0x01,0x00,0x00,0x6b,0x01,0x00,0x00,
0xf8,0x00,0x02,0x00,0x67,0x01,0x00,0x00,0x41,0x00,0x05,0x00,0x75,0x00,0x00,0x00,
0x69,0x01,0x00,0x00,0x72,0x00,0x00,0x00,0xdc,0x00,0x00,0x00,0x3d,0x00,0x04,0x00,
0x06,0x00,0x00,0x00,0x6a,0x01,0x00,0x00,0x69,0x01,0x00,0x00,0xf9,0x00,0x02,0x00,
0x6c,0x01,0x00,0x00,0xf8,0x00,0x02,0x00,0x6b,0x01,0x00,0x00,0xf9,0x00,0x02,0x00,
0x6c,0x01,0x00,0x00,0xf8,0x00,0x02,0x00,0x6c,0x01,0x00,0x00,0xf5,0x00,0x07,0x00,
0x06,0x00,0x00,0x00,0x7e,0x01,0x00,0x00,0x6a,0x01,0x00,0x00,0x67,0x01,0x00,0x00,
0x47,0x00,0x00,0x00,0x6b,0x01,0x00,0x00,0x5d,0x01,0x06,0x00,0x06,0x00,0x00,0x00,
0x6e,0x01,0x00,0x00,0x63,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7e,0x01,0x00,0x00,
0xf9,0x00,0x02,0x00,0x6f,0x01,0x00,0x00,0xf8,0x00,0x02,0x00,0x6f,0x01,0x00,0x00,
0xf5,0x00,0x07,0x00,0x06,0x00,0x00,0x00,0x7f,0x01,0x00,0x00,0x56,0x01,0x00,0x00,
0x5f,0x01,0x00,0x00,0x6e,0x01,0x00,0x00,0x6c,0x01,0x00,0x00,0xaa,0x00,0x05,0x00,
0x6b,0x00,0x00,0x00,0x32,0x01,0x00,0x00,0xdc,0x00,0x00,0x00,0x47,0x00,0x00,0x00,
0xf7,0x00,0x03,0x00,0x37,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xfa,0x00,0x04,0x00,
0x32,0x01,0x00,0x00,0x33,0x01,0x00,0x00,0x37,0x01,0x00,0x00,0xf8,0x00,0x02,0x00,
0x33,0x01,0x00,0x00,0x41,0x00,0x06,0x00,0x43,0x00,0x00,0x00,0x7a,0x01,0x00,0x00,
0x4d,0x00,0x00,0x00,0x41,0x00,0x00,0x00,0xdf,0x00,0x00,0x00,0x3e,0x00,0x03,0x00,
0x7a,0x01,0x00,0x00,0x7f,0x01,0x00,0x00,0xf9,0x00,0x02,0x00,0x37,0x01,0x00,0x00,
0xf8,0x00,0x02,0x00,0x37,0x01,0x00,0x00,0xfd,0x00,0x01,0x00,0x38,0x00,0x01,0x00
};


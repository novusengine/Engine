#pragma once
#include <Base/Types.h>

// Based on: https://gist.github.com/9prady9/a5e1e8bdbc9dc58b3349
class CPUInfo
{
public:
    static CPUInfo& Get();

    const std::string& GetPrettyName() const { return _prettyName; }

    const std::string& GetVendor() const { return _vendorId; }
    const std::string& GetModel() const { return _modelName; }

    i32 GetNumCores() const { return _numCores; }
    i32 GetNumThreads() const { return _numThreads; }

    bool IsSSE() const { return features.isSSE; }
    bool IsSSE2() const { return features.isSSE2; }
    bool IsSSE3() const { return features.isSSE3; }
    bool IsSSE41() const { return features.isSSE41; }
    bool IsSSE42() const { return features.isSSE42; }
    bool IsAVX() const { return features.isAVX; }
    bool IsAVX2() const { return features.isAVX2; }
    bool IsHyperThreaded() const { return features.isHTT; }

    void Print(i32 detailLevel);

private:
    CPUInfo();

private:
    static bool _initialized;
    static CPUInfo* _cpuInfo;

    // Bit positions for data extractions
    static const u32 SSE_POS = 0x02000000;
    static const u32 SSE2_POS = 0x04000000;
    static const u32 SSE3_POS = 0x00000001;
    static const u32 SSE41_POS = 0x00080000;
    static const u32 SSE42_POS = 0x00100000;
    static const u32 AVX_POS = 0x10000000;
    static const u32 AVX2_POS = 0x00000020;
    static const u32 LVL_NUM = 0x000000FF;
    static const u32 LVL_TYPE = 0x0000FF00;
    static const u32 LVL_CORES = 0x0000FFFF;

    // Attributes
    std::string _vendorId;
    std::string _modelName;
    std::string _prettyName;

    i32 _numThreadsPerCore;
    i32 _numCores;
    i32 _numThreads;

    struct Features
    {
        u8 isHTT : 1;
        u8 isSSE : 1;
        u8 isSSE2 : 1;
        u8 isSSE3 : 1;
        u8 isSSE41 : 1;
        u8 isSSE42 : 1;
        u8 isAVX : 1;
        u8 isAVX2 : 1;
    };

    Features features;
};

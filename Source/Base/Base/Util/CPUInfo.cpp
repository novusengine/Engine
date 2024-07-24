#include "CPUInfo.h"
#include <Base/Util/DebugHandler.h>

#ifdef _WIN32
#include <limits.h>
#include <intrin.h>
#else
#include <stdint.h>
#endif

using namespace std;

#define MAX_INTEL_TOP_LVL 4

class CPUID
{
    u32 regs[4];

public:
    CPUID(u32 funcId, u32 subFuncId)
    {
#ifdef _WIN32
        __cpuidex((i32*)regs, (i32)funcId, (i32)subFuncId);
#else
        asm volatile
            ("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
                : "a" (funcId), "c" (subFuncId));
        // ECX is set to zero for CPUID function 4
#endif
    }

    const u32& EAX() const { return regs[0]; }
    const u32& EBX() const { return regs[1]; }
    const u32& ECX() const { return regs[2]; }
    const u32& EDX() const { return regs[3]; }
};

CPUInfo* CPUInfo::_cpuInfo = nullptr;
bool CPUInfo::_initialized = false;

CPUInfo& CPUInfo::Get()
{
    if (!_initialized)
    {
        _cpuInfo = new CPUInfo();
        _initialized = true;
    }

    return *_cpuInfo;
}

void CPUInfo::Print(i32 detailLevel)
{
    if (detailLevel > 0)
    {
        NC_LOG_INFO("[CPUInfo]: {0}", _prettyName);
    }

    if (detailLevel > 1)
    {
        NC_LOG_INFO("[CPUInfo]: Supports Hyperthreading: {0}", features.isHTT ? "yes" : "no");
        NC_LOG_INFO("[CPUInfo]: Supports SSE: {0}", features.isSSE ? "yes" : "no");
        NC_LOG_INFO("[CPUInfo]: Supports SSE2: {0}", features.isSSE2 ? "yes" : "no");
        NC_LOG_INFO("[CPUInfo]: Supports SSE3: {0}", features.isSSE3 ? "yes" : "no");
        NC_LOG_INFO("[CPUInfo]: Supports SSE4.1: {0}", features.isSSE41 ? "yes" : "no");
        NC_LOG_INFO("[CPUInfo]: Supports SSE4.2: {0}", features.isSSE42 ? "yes" : "no");
        NC_LOG_INFO("[CPUInfo]: Supports AVX: {0}", features.isAVX ? "yes" : "no");
        NC_LOG_INFO("[CPUInfo]: Supports AVX2: {0}", features.isAVX2 ? "yes" : "no");
    }
}

static inline void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
    {
        return !std::isspace(ch) && ch != '\0';
    }).base(), s.end());
}

CPUInfo::CPUInfo()
{
    // Get vendor name EAX=0
    CPUID cpuID0(0, 0);
    u32 HFS = cpuID0.EAX();
    _vendorId += string((const char*)&cpuID0.EBX(), 4);
    _vendorId += string((const char*)&cpuID0.EDX(), 4);
    _vendorId += string((const char*)&cpuID0.ECX(), 4);
    // Get SSE instructions availability
    CPUID cpuID1(1, 0);
    features.isHTT = cpuID1.EDX() & AVX_POS;
    features.isSSE = cpuID1.EDX() & SSE_POS;
    features.isSSE2 = cpuID1.EDX() & SSE2_POS;
    features.isSSE3 = cpuID1.ECX() & SSE3_POS;
    features.isSSE41 = cpuID1.ECX() & SSE41_POS;
    features.isSSE42 = cpuID1.ECX() & SSE41_POS;
    features.isAVX = cpuID1.ECX() & AVX_POS;
    // Get AVX2 instructions availability
    CPUID cpuID7(7, 0);
    features.isAVX2 = cpuID7.EBX() & AVX2_POS;

    string upVId = _vendorId;
    for_each(upVId.begin(), upVId.end(), [](char& in)
    {
        in = ::toupper(in);
    });

    // Get num of cores
    if (upVId.find("INTEL") != std::string::npos)
    {
        if (HFS >= 11)
        {
            for (i32 lvl = 0; lvl < MAX_INTEL_TOP_LVL; ++lvl)
            {
                CPUID cpuID4(0x0B, lvl);
                u32 currLevel = (LVL_TYPE & cpuID4.ECX()) >> 8;

                switch (currLevel)
                {
                    case 0x01: _numThreadsPerCore = LVL_CORES & cpuID4.EBX(); break;
                    case 0x02: _numThreads = LVL_CORES & cpuID4.EBX(); break;
                    default: break;
                }
            }
            _numCores = _numThreads / _numThreadsPerCore;
        }
        else
        {
            if (HFS >= 1)
            {
                _numThreads = (cpuID1.EBX() >> 16) & 0xFF;
                if (HFS >= 4)
                {
                    _numCores = 1 + (CPUID(4, 0).EAX() >> 26) & 0x3F;
                }
            }
            if (features.isHTT)
            {
                if (!(_numCores > 1))
                {
                    _numCores = 1;
                    _numThreads = (_numThreads >= 2 ? _numThreads : 2);
                }
            }
            else
            {
                _numCores = _numThreads = 1;
            }
        }
    }
    else if (upVId.find("AMD") != std::string::npos)
    {
        if (HFS >= 1)
        {
            _numThreads = (cpuID1.EBX() >> 16) & 0xFF;
            if (CPUID(0x80000000, 0).EAX() >= 8)
            {
                _numCores = 1 + (CPUID(0x80000008, 0).ECX() & 0xFF);
            }
        }
        if (features.isHTT)
        {
            if (!(_numCores > 1))
            {
                _numCores = 1;
                _numThreads = (_numThreads >= 2 ? _numThreads : 2);
            }
            else
            {
                _numThreadsPerCore = 2;
                _numCores = _numThreads / _numThreadsPerCore;
            }
        }
        else
        {
            _numCores = _numThreads = 1;
        }
    }
    else
    {
        //NC_LOG_CRITICAL("Unexpected vendor id ({0})", upVId.c_str());
    }

    // Get processor brand string
    // This seems to be working for both Intel & AMD vendors
    for (i32 i = 0x80000002; i < 0x80000005; ++i)
    {
        CPUID cpuID(i, 0);
        _modelName += string((const char*)&cpuID.EAX(), 4);
        _modelName += string((const char*)&cpuID.EBX(), 4);
        _modelName += string((const char*)&cpuID.ECX(), 4);
        _modelName += string((const char*)&cpuID.EDX(), 4);
    }
    rtrim(_modelName);

    // Create pretty name
    _prettyName = _modelName + " (" + std::to_string(_numCores) + "c" + std::to_string(_numThreads) + "t)";
}
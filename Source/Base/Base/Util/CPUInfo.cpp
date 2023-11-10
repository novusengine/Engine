#include "CPUInfo.h"
#include <Base/Util/DebugHandler.h>

#ifdef _WIN32
#include <limits.h>
#include <intrin.h>
#include <windows.h>
#else
#include <stdio.h>
#include <stdint.h>
#endif

using namespace std;

#define MAX_INTEL_TOP_LVL 4

class CPUID
{
    u32 regs[4] = { 0 };

public:
    CPUID(u32 funcId, u32 subFuncId)
    {
#ifdef _WIN32
        ::__cpuidex((i32*)regs, (i32)funcId, (i32)subFuncId);
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
        DebugHandler::Print("[CPUInfo]: {0}", _prettyName);
    }

    if (detailLevel > 1)
    {
        DebugHandler::Print("[CPUInfo]: Supports Hyperthreading: {0}", features.isHTT ? "yes" : "no");
        DebugHandler::Print("[CPUInfo]: Supports SSE: {0}", features.isSSE ? "yes" : "no");
        DebugHandler::Print("[CPUInfo]: Supports SSE2: {0}", features.isSSE2 ? "yes" : "no");
        DebugHandler::Print("[CPUInfo]: Supports SSE3: {0}", features.isSSE3 ? "yes" : "no");
        DebugHandler::Print("[CPUInfo]: Supports SSE4.1: {0}", features.isSSE41 ? "yes" : "no");
        DebugHandler::Print("[CPUInfo]: Supports SSE4.2: {0}", features.isSSE42 ? "yes" : "no");
        DebugHandler::Print("[CPUInfo]: Supports AVX: {0}", features.isAVX ? "yes" : "no");
        DebugHandler::Print("[CPUInfo]: Supports AVX2: {0}", features.isAVX2 ? "yes" : "no");
    }
}

static inline void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
    {
        return !std::isspace(ch) && ch != '\0';
    }).base(), s.end());
}

// source : https://github.com/time-killer-games/ween/blob/db69cafca2222c634a1d3a9e58262b5a2dc8d508/system.cpp#L1553
int CPUInfo::cpu_numcpus() {
    if (_numThreads != -1) {
        return _numThreads;
    }
#if defined(_WIN32)
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    _numThreads = sysinfo.dwNumberOfProcessors;
    return _numThreads;
// #elif (defined(__APPLE__) && defined(__MACH__))
//     int logical_cpus = -1;
//     std::size_t len = sizeof(int);
//     if (!sysctlbyname("machdep.cpu.thread_count", &logical_cpus, &len, nullptr, 0)) {
//         numcpus = logical_cpus;
//     }
//     return numcpus;
// #elif defined(__linux__)
//    char buf[1024];
//    const char* result = nullptr;
//    FILE* fp = popen("lscpu | grep 'CPU(s):' | uniq | cut -d' ' -f4- | awk 'NR==1{$1=$1;print}'", "r");
//    if (fp) {
//        if (fgets(buf, sizeof(buf), fp)) {
//            buf[strlen(buf) - 1] = '\0';
//            result = buf;
//        }
//        pclose(fp);
//        static std::string str;
//        str = (result && strlen(result)) ? result : "-1";
//        _numThreads = (int)strtol(str.c_str(), nullptr, 10);
//    }
//    return _numThreads;
#else
    return -1;
#endif
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
    features.isAVX512F = cpuID7.EBX() & AVX512F_POS;

    string upVId = _vendorId;
    for_each(upVId.begin(), upVId.end(), [](char& in)
    {
        in = ::toupper(in);
    });

    _numThreadsPerCore = 0;
    _numCores = 0;
    _numThreads = cpu_numcpus();
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
                    case 0x01: _numThreadsPerCore = LVL_CORES & cpuID4.EBX(); break; //  EAX=0xB, ECX=0 - EBX is the number of logical processors (threads) per core
                    case 0x02: _numThreads = LVL_CORES & cpuID4.EBX(); break; // EAX=0xB, ECX=1 - EBX is the number of logical processors per processor package
                    default: break;
                }
            }
            _numCores = _numThreads / _numThreadsPerCore;
            features.isHTT = _numThreadsPerCore > 1;
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
        _numThreadsPerCore = 1 + ((CPUID(0x8000001e, 0).EBX() >> 8) & 0xff);
        if (_numThreads > 0 && _numThreadsPerCore > 0) {
            _numCores = _numThreads / _numThreadsPerCore;
        }
        else {
            if (HFS >= 1) {
                if (CPUID(0x80000000, 0).EAX() >= 8) {
                    _numCores = 1 + (CPUID(0x80000008, 0).ECX() & 0xFF);
                }
            }
            if (features.isHTT) {
                if (_numCores < 1) {
                    _numCores = 1;
                }
            }
            else {
                _numCores = 1;
            }
        }
    }
    else
    {
        DebugHandler::PrintFatal("Unexpected vendor id ({0})", upVId.c_str());
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
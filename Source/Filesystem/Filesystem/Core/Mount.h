#pragma once
#include "Filesystem/Core/File.h"
#include "Filesystem/Core/Manifest.h"

#include <Base/Types.h>

#include <robinhood/robinhood.h>

#include <atomic>
#include <filesystem>
#include <string>
#include <vector>

namespace PACT
{
    struct PactManifest;

    using PactMountHandle = u32;

    struct PactMountOptions
    {
    public:
    };

    struct PactMount
    {
    public:
        const PactManifest* manifest;
        u32 priority;
        u32 mountIndex;
    };

    struct PactVirtualDirectory
    {
    public:
        std::vector<std::string> directories;
        std::vector<std::string> files;
    };

    struct PactMountTable
    {
    public:
        std::atomic<PactGenerationID> currentGeneration = { 0 };
        u32 currentMountIndex = 0;

        std::vector<PactMount> mounts;
        robin_hood::unordered_set<PactManifestHandle> mountIDSet;
        robin_hood::unordered_map<u64, PactFileRuntimeRecord> pathTable;
        robin_hood::unordered_map<PactFileID, PactFileRuntimeRecord> fileIDTable;
        std::vector<std::string> virtualPathIndex;
        robin_hood::unordered_map<std::string, PactVirtualDirectory> virtualDirectoryIndex;
    };
}

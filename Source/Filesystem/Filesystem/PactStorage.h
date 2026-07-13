#pragma once
#include "Filesystem/Core/File.h"
#include "Filesystem/Core/Manifest.h"
#include "Filesystem/Core/Mount.h"
#include "Filesystem/Core/Root.h"

#include <Base/Types.h>

#include <filesystem>
#include <limits>
#include <vector>

namespace PACT
{
    struct PactOpenOptions
    {
        u32 FallbackToInit : 1;
    };

    class PactStorage
    {
    public:
        bool Init(PactOpenOptions options = {});
        bool Open(const std::filesystem::path& rootDir, PactOpenOptions options = {});
        // All file handles and pins must be released before shutdown. Returns false
        // without tearing storage down when that lifetime contract is violated.
        bool Shutdown();

        PactManifestHandle AddOverlay(const std::filesystem::path& relativeRootDir, bool mountImmediately = true, u32 priority = std::numeric_limits<u32>::max());
        bool ReloadOverlay(PactManifestHandle handle);
        bool ReloadOverlay(const std::filesystem::path& relativeRootDir);

        bool Mount(PactManifestHandle handle, const PactMountOptions& options = {});
        bool Unmount(const PactManifestHandle handle);

        bool FileExists(const u64 hash);
        bool FileExists(const std::string& path);
        const std::string* GetFilePath(const u64 hash);

        PactReadResult ReadFileRecord(const PactFileRuntimeRecord& record, const u64 fileKeyValue, PactFileHandle& outHandle, const PactFileOpenOption option = PactFileOpenOption::None);
        PactReadResult ReadFile(const u64 hash, PactFileHandle& outHandle, const PactFileOpenOption option = PactFileOpenOption::None);
        PactReadResult ReadFile(const std::string& path, PactFileHandle& outHandle, const PactFileOpenOption option = PactFileOpenOption::None);
#if 0 // The asynchronous PACT pipeline is not ready to expose yet.
        PactReadResult ReadFileRecordAsync(const PactFileRuntimeRecord& record, const u64 fileKeyValue, PactFileHandle& outHandle, const PactFileOpenOption option = PactFileOpenOption::None);
        PactReadResult ReadFileAsync(const u64 hash, PactFileHandle& outHandle, const PactFileOpenOption option = PactFileOpenOption::None);
        PactReadResult ReadFileAsync(const std::string& path, PactFileHandle& outHandle, const PactFileOpenOption option = PactFileOpenOption::None);
#endif

    public:
        void MountAll();
        void UnmountAll();
        void BuildMountList();
        void SortMountList();
        void BuildGlobalLookup();

        PactResidentFile* FindOrCreateResidentFile(PactFileKey key);
        PactFileHandle CreateHandle(PactFileKey key);
        void RequestEviction(PactResidentFile& residentFile);

    private:
#if 0 // The asynchronous PACT pipeline is not ready to expose yet.
        void IOThreadMain();
#endif
        bool InitAtRoot(const std::filesystem::path& rootDir, PactOpenOptions options);
        PactManifestHandle GenerateManifestHandle();
        bool BuildOverlayManifest(PactManifest& manifest, const std::filesystem::path& absolutePath, PactManifestHandle handle, u32 priority);
        void EvictResidentFile(PactFileKey key);

    private:
        std::filesystem::path _rootDir;
        std::filesystem::path _manifestDir;
        std::filesystem::path _dataDir;

        PactRoot _root;
        PactManifestTable _manifestTable;
        PactMountTable _mountTable;

#if 0 // The asynchronous PACT pipeline is not ready to expose yet.
        std::thread _ioThread;
        std::atomic<bool> _shutdownRequested = false;
        moodycamel::ConcurrentQueue<PactIORequest> _ioQueue;
#endif

        robin_hood::unordered_node_map<PactFileKey, PactResidentFile> _fileKeyToResidentFile;
        std::mutex _residentFilesMutex;
    };
}

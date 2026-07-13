#include "PactStorage.h"
#include "Filesystem/Config.h"

#include <Base/Memory/Bytebuffer.h>
#include <Base/Memory/FileReader.h>
#include <Base/Memory/FileWriter.h>
#include <Base/Util/DebugHandler.h>

#include <xxhash/xxhash64.h>

#include <algorithm>
#include <fstream>
#include <mutex>
#include <system_error>

namespace fs = std::filesystem;

namespace PACT
{
    bool PactStorage::Init(PactOpenOptions options)
    {
        return InitAtRoot(fs::current_path() / Config::BASE_DIR, options);
    }

    bool PactStorage::InitAtRoot(const fs::path& rootDir, PactOpenOptions options)
    {
        (void)options;

        _rootDir = rootDir;
        _manifestDir = _rootDir / Config::MANIFEST_DIR;
        _dataDir = _rootDir / Config::DATA_DIR;
        fs::path rootFile = _rootDir / Config::ROOT_FILE;

        if (fs::exists(rootFile))
        {
            NC_LOG_ERROR("Pact : Failed to initialize empty storage. Root file exists (\"{0}\")", rootFile.string());
            return false;
        }

        fs::create_directories(_rootDir);
        fs::create_directories(_manifestDir);
        fs::create_directories(_dataDir);

        // Write Root File
        {
            _root =
            {
                .version = Config::ROOT_VERSION,
                .featureSet = {}
            };

            std::shared_ptr<Bytebuffer> buffer = Bytebuffer::Borrow<sizeof(PactRoot)>();
            if (!buffer->Serialize(_root))
            {
                NC_LOG_ERROR("Pact : Failed to initialize empty storage. Root file could not be serialized (\"{0}\")", rootFile.string());
                return false;
            }

            FileWriter fileWriter;
            if (!fileWriter.Open(rootFile) || !fileWriter.Write(buffer))
                return false;
        }

        return true;
    }
    bool PactStorage::Open(const fs::path& rootDir, PactOpenOptions options)
    {
        _rootDir = rootDir;
        _manifestDir = _rootDir / Config::MANIFEST_DIR;
        _dataDir = _rootDir / Config::DATA_DIR;
        fs::path rootFile = _rootDir / Config::ROOT_FILE;

        if (!fs::exists(rootFile))
        {
            if (options.FallbackToInit)
            {
                if (!InitAtRoot(rootDir, options))
                    return false;
            }
            else
            {
                NC_LOG_ERROR("Pact : Failed to find storage. Root file does not exist (\"{0}\")", rootFile.string());
                return false;
            }
        }

        fs::create_directories(_manifestDir);
        fs::create_directories(_dataDir);

        // Read Root File
        {
            std::shared_ptr<Bytebuffer> buffer = Bytebuffer::Borrow<sizeof(PactRoot)>();

            FileReader reader(rootFile.string());
            if (!reader.Open())
            {
                NC_LOG_ERROR("Pact : Failed to initialize storage. Root file could not be opened (\"{0}\")", rootFile.string());
                return false;
            }

            reader.Read(buffer.get(), buffer->size);

            if (!buffer->Deserialize(_root))
            {
                NC_LOG_ERROR("Pact : Failed to initialize storage. Root file is corrupt (\"{0}\")", rootFile.string());
                return false;
            }

            PactRootValidateResult result = _root.Validate();
            switch (result)
            {
                case PactRootValidateResult::InvalidMagic:
                {
                    NC_LOG_ERROR("Pact : Failed to initialize storage. Root file has invalid magic (\"{0}\")", rootFile.string());
                    return false;
                }
                case PactRootValidateResult::MismatchVersion:
                {
                    NC_LOG_ERROR("Pact : Failed to initialize storage. Root file has mismatching version (Found Version [{1}, {2}, {3}] | Expected Version : [{4}, {5}, {6}]) (\"{0}\")", rootFile.string(), _root.version.x, _root.version.y, _root.version.z, Config::ROOT_VERSION.x, Config::ROOT_VERSION.y, Config::ROOT_VERSION.z);
                    return false;
                }

                default: break;
            }
        }

        // Parse Manifest Directory
        {
            fs::recursive_directory_iterator manifestDirItr { _manifestDir };

            for (fs::path path : manifestDirItr)
            {
                if (path.extension().compare(Config::MANIFEST_EXT) != 0)
                    continue;

                FileReader reader(path.string());
                if (!reader.Open())
                {
                    NC_LOG_WARNING("Pact : Failed to open Manifest File (\"{0}\")", path.string());
                    continue;
                }

                size_t fileSize = reader.Length();
                std::shared_ptr<Bytebuffer> buffer = Bytebuffer::BorrowRuntime(fileSize);
                reader.Read(buffer.get(), fileSize);

                PactManifest manifest;

                if (!buffer->Deserialize(manifest))
                {
                    NC_LOG_ERROR("Pact : Manifest File is corrupt (\"{0}\")", path.string());
                    continue;
                }

                PactManifestValidateResult result = manifest.Validate();
                switch (result)
                {
                    case PactManifestValidateResult::InvalidMagic:
                    {
                        NC_LOG_ERROR("Pact : Manifest file has invalid magic (\"{0}\")", path.string());
                        continue;
                    }
                    case PactManifestValidateResult::MismatchVersion:
                    {
                        NC_LOG_ERROR("Pact : Manifest file has mismatching version (Found Version [{1}, {2}, {3}] | Expected Version : [{4}, {5}, {6}]) (\"{0}\")", path.string(), manifest.header.version.x, manifest.header.version.y, manifest.header.version.z, Config::MANIFEST_VERSION.x, Config::MANIFEST_VERSION.y, Config::MANIFEST_VERSION.z);
                        continue;
                    }

                    default: break;
                }

                manifest.dataPath = (_dataDir / path.filename()).replace_extension(Config::DATA_EXT);
                _manifestTable.handleToManifest[manifest.header.manifestID] = std::move(manifest);
            }
        }

        return true;
    }
    bool PactStorage::Shutdown()
    {
        std::unique_lock mountTableLock(_mountTableMutex);

        {
            std::scoped_lock lock(_residentFilesMutex);
            for (const auto& [key, residentFile] : _fileKeyToResidentFile)
            {
                (void)key;
                if (residentFile.handleCount.load(std::memory_order_acquire) != 0 ||
                    residentFile.pinCount.load(std::memory_order_acquire) != 0)
                {
                    NC_LOG_ERROR("Pact : Shutdown failed because resident file handles or pins are still active");
                    return false;
                }
            }

            for (auto& [key, residentFile] : _fileKeyToResidentFile)
            {
                (void)key;
                delete[] static_cast<u8*>(residentFile.data);
                residentFile.data = nullptr;
                residentFile.size = 0;
                residentFile.owner = nullptr;
                residentFile.loadState.store(PactLoadState::Unloaded, std::memory_order_release);
            }
            _fileKeyToResidentFile.clear();
        }

        UnmountAllInternal();
        _mountTable.currentGeneration.store(0, std::memory_order_release);

        _manifestTable.handleToManifest.clear();
        _manifestTable.overlayPathToHandle.clear();
        _root.featureSet = {};

        _rootDir.clear();
        _manifestDir.clear();
        _dataDir.clear();

        return true;
    }

    PactManifestHandle PactStorage::AddOverlay(const fs::path& relativeRootDir, bool mountImmediately, u32 priority)
    {
        fs::path absolutePath = fs::absolute(relativeRootDir);
        absolutePath = fs::weakly_canonical(absolutePath);

        std::unique_lock lock(_mountTableMutex);

        auto itr = _manifestTable.overlayPathToHandle.find(absolutePath);
        if (itr != _manifestTable.overlayPathToHandle.end())
        {
            if (mountImmediately && !_mountTable.mountIDSet.contains(itr->second))
                MountInternal(itr->second, {});

            return itr->second;
        }

        PactManifestHandle handle = GenerateManifestHandle();
        PactManifest& manifest = _manifestTable.handleToManifest[handle];

        if (!BuildOverlayManifest(manifest, absolutePath, handle, priority))
        {
            _manifestTable.handleToManifest.erase(handle);
            return MANIFEST_INVALID_ID;
        }

        _manifestTable.overlayPathToHandle[absolutePath] = handle;

        if (mountImmediately)
        {
            MountInternal(handle, {});
        }

        return handle;
    }

    bool PactStorage::ReloadOverlay(PactManifestHandle handle)
    {
        std::unique_lock lock(_mountTableMutex);
        return ReloadOverlayInternal(handle);
    }

    bool PactStorage::ReloadOverlayInternal(PactManifestHandle handle)
    {
        auto manifestItr = _manifestTable.handleToManifest.find(handle);
        if (manifestItr == _manifestTable.handleToManifest.end())
            return false;

        PactManifest& manifest = manifestItr->second;
        if (!manifest.header.flags.isOverlay || manifest.header.sourceType != PactSourceType::Loose)
            return false;

        std::vector<u64> oldHashes;
        oldHashes.reserve(manifest.entries.size());
        for (const ManifestEntry& entry : manifest.entries)
        {
            if (entry.pathHash != 0)
                oldHashes.push_back(entry.pathHash);
        }

        const fs::path overlayPath = manifest.dataPath;
        const u32 priority = manifest.header.priority;

        PactManifest newManifest;
        if (!BuildOverlayManifest(newManifest, overlayPath, handle, priority))
            return false;

        manifest = std::move(newManifest);

        robin_hood::unordered_set<u64> hashesToEvict;
        hashesToEvict.reserve(oldHashes.size() + manifest.entries.size());
        for (u64 hash : oldHashes)
            hashesToEvict.insert(hash);

        for (const ManifestEntry& entry : manifest.entries)
        {
            if (entry.pathHash != 0)
                hashesToEvict.insert(entry.pathHash);
        }

        for (u64 hash : hashesToEvict)
        {
            EvictResidentFile(PactFileKey
            {
                .type = PactFileKey::Type::Loose,
                .value = hash
            });
        }

        if (_mountTable.mountIDSet.contains(handle))
        {
            _mountTable.currentGeneration.fetch_add(1, std::memory_order_release);
            BuildGlobalLookup();
        }

        return true;
    }

    bool PactStorage::ReloadOverlay(const fs::path& relativeRootDir)
    {
        fs::path absolutePath = fs::absolute(relativeRootDir);
        absolutePath = fs::weakly_canonical(absolutePath);

        std::unique_lock lock(_mountTableMutex);

        auto itr = _manifestTable.overlayPathToHandle.find(absolutePath);
        if (itr == _manifestTable.overlayPathToHandle.end())
            return false;

        return ReloadOverlayInternal(itr->second);
    }

    bool PactStorage::Mount(PactManifestHandle handle, const PactMountOptions& options)
    {
        std::unique_lock lock(_mountTableMutex);
        return MountInternal(handle, options);
    }

    bool PactStorage::MountInternal(PactManifestHandle handle, const PactMountOptions& options)
    {
        (void)options;

        auto manifestItr = _manifestTable.handleToManifest.find(handle);
        bool manifestIsMissing = manifestItr == _manifestTable.handleToManifest.end();
        if (manifestIsMissing)
            return false;

        bool isManifestAlreadyMounted = _mountTable.mountIDSet.contains(handle);
        if (isManifestAlreadyMounted)
            return false;

        _mountTable.currentGeneration.fetch_add(1, std::memory_order_release);

        PactMount& mount = _mountTable.mounts.emplace_back();
        mount.manifest = &manifestItr->second;
        mount.priority = manifestItr->second.header.priority;
        mount.mountIndex = _mountTable.currentMountIndex++;

        _mountTable.mountIDSet.insert(mount.manifest->header.manifestID);

        SortMountList();
        BuildGlobalLookup();

        return true;
    }
    bool PactStorage::Unmount(const PactManifestHandle handle)
    {
        std::unique_lock lock(_mountTableMutex);

        bool manifestIsMissing = !_manifestTable.handleToManifest.contains(handle);
        if (manifestIsMissing)
            return false;

        bool manifestIsNotMounted = !_mountTable.mountIDSet.contains(handle);
        if (manifestIsNotMounted)
            return false;

        _mountTable.currentGeneration.fetch_add(1, std::memory_order_release);
        _mountTable.mountIDSet.erase(handle);
        std::erase_if(_mountTable.mounts, [handle](const PactMount& mount)
        {
            return mount.manifest->header.manifestID == handle;
        });

        BuildGlobalLookup();

        return true;
    }

    bool PactStorage::FileExists(const u64 hash)
    {
        std::shared_lock lock(_mountTableMutex);
        auto pathItr = _mountTable.pathTable.find(hash);
        return pathItr != _mountTable.pathTable.end();
    }
    bool PactStorage::FileExists(const std::string& path)
    {
        u64 hash = XXHash64::hash(path.c_str(), path.length(), 0);
        return FileExists(hash);
    }

    bool PactStorage::GetFilePath(const u64 hash, std::string& outPath)
    {
        std::shared_lock lock(_mountTableMutex);

        auto pathItr = _mountTable.pathTable.find(hash);
        if (pathItr == _mountTable.pathTable.end())
            return false;

        const PactFileRuntimeRecord& runtimeRecord = pathItr->second;
        if (!runtimeRecord.entry || !runtimeRecord.source || !runtimeRecord.source->manifest)
            return false;

        outPath = runtimeRecord.source->manifest->stringTable.GetString(runtimeRecord.entry->pathIndex);
        return true;
    }

    const std::string* PactStorage::GetFilePath(const u64 hash)
    {
        std::shared_lock lock(_mountTableMutex);

        auto pathItr = _mountTable.pathTable.find(hash);
        if (pathItr == _mountTable.pathTable.end())
            return nullptr;

        const PactFileRuntimeRecord& runtimeRecord = pathItr->second;
        if (!runtimeRecord.entry || !runtimeRecord.source)
            return nullptr;

        if (!runtimeRecord.source->manifest)
            return nullptr;

        const std::string& path = runtimeRecord.source->manifest->stringTable.GetString(runtimeRecord.entry->pathIndex);
        return &path;
    }

    PactReadResult PactStorage::ReadFileRecordInternal(const PactFileRuntimeRecord& record, const u64 fileKeyValue, PactFileHandle& outHandle, const PactFileOpenOption option)
    {
        PactFileKey fileKey =
        {
            .type = static_cast<PactFileKey::Type>(record.source->manifest->header.sourceType),
            .value = fileKeyValue
        };

        PactFileHandle handle = CreateHandle(fileKey);
        if (option == PactFileOpenOption::Pinned)
            handle.Pin();

        PactReadResult result = handle.GetOwner()->EnsureLoadedSync(record);
        if (result != PactReadResult::Success)
            return result;

        outHandle = std::move(handle);
        return PactReadResult::Success;
    }
    PactReadResult PactStorage::ReadFile(const u64 hash, PactFileHandle& outHandle, const PactFileOpenOption option)
    {
        std::shared_lock lock(_mountTableMutex);

        auto pathItr = _mountTable.pathTable.find(hash);
        if (pathItr == _mountTable.pathTable.end())
            return PactReadResult::FileNotFound;

        const PactFileRuntimeRecord& record = pathItr->second;
        return ReadFileRecordInternal(record, hash, outHandle, option);
    }
    PactReadResult PactStorage::ReadFile(const u64 hash, PactFileHandle& outHandle, std::string& outPath, const PactFileOpenOption option)
    {
        std::shared_lock lock(_mountTableMutex);

        auto pathItr = _mountTable.pathTable.find(hash);
        if (pathItr == _mountTable.pathTable.end())
            return PactReadResult::FileNotFound;

        const PactFileRuntimeRecord& record = pathItr->second;
        if (!record.entry || !record.source || !record.source->manifest)
            return PactReadResult::FileNotFound;

        outPath = record.source->manifest->stringTable.GetString(record.entry->pathIndex);
        return ReadFileRecordInternal(record, hash, outHandle, option);
    }
    PactReadResult PactStorage::ReadFile(const std::string& path, PactFileHandle& outHandle, const PactFileOpenOption option)
    {
        u64 hash = XXHash64::hash(path.c_str(), path.length(), 0);
        return ReadFile(hash, outHandle, option);
    }
#if 0 // The asynchronous PACT pipeline is not ready to expose yet.
    PactReadResult PactStorage::ReadFileRecordAsync(const PactFileRuntimeRecord& record, const u64 fileKeyValue, PactFileHandle& outHandle, const PactFileOpenOption option)
    {
        PactFileKey fileKey =
        {
            .type = static_cast<PactFileKey::Type>(record.source->manifest->header.sourceType),
            .value = fileKeyValue
        };

        PactResidentFile* residentFile = nullptr;

        {
            std::scoped_lock lock(_residentFilesMutex);

            residentFile = FindOrCreateResidentFile(fileKey);
            residentFile->AddHandle();
        }

        outHandle = PactFileHandle(residentFile);
        if (option == PactFileOpenOption::Pinned)
            outHandle.Pin();

        PactLoadState expected = PactLoadState::Unloaded;
        if (residentFile->loadState.compare_exchange_strong(expected, PactLoadState::Loading, std::memory_order_acq_rel))
        {
            _ioQueue.enqueue(PactIORequest{ residentFile, record });
        }

        if (expected == PactLoadState::Loaded)
        {
            return PactReadResult::Success;
        }
        else if (expected == PactLoadState::Failed)
        {
            return PactReadResult::Failed;
        }

        return PactReadResult::Pending;
    }
    PactReadResult PactStorage::ReadFileAsync(const u64 hash, PactFileHandle& outHandle, const PactFileOpenOption option)
    {
        auto pathItr = _mountTable.pathTable.find(hash);
        if (pathItr == _mountTable.pathTable.end())
            return PactReadResult::FileNotFound;

        const PactFileRuntimeRecord& record = pathItr->second;
        return ReadFileRecordAsync(record, hash, outHandle, option);
    }
    PactReadResult PactStorage::ReadFileAsync(const std::string& path, PactFileHandle& outHandle, const PactFileOpenOption option)
    {
        u64 hash = XXHash64::hash(path.c_str(), path.length(), 0);
        return ReadFileAsync(hash, outHandle, option);
    }
#endif

    void PactStorage::MountAll()
    {
        std::unique_lock lock(_mountTableMutex);
        BuildMountList();
        BuildGlobalLookup();
    }

    void PactStorage::UnmountAll()
    {
        std::unique_lock lock(_mountTableMutex);
        UnmountAllInternal();
    }

    void PactStorage::UnmountAllInternal()
    {
        _mountTable.currentGeneration.fetch_add(1, std::memory_order_release);
        _mountTable.currentMountIndex = 0;

        _mountTable.mounts.clear();
        _mountTable.mountIDSet.clear();

        BuildGlobalLookup();
    }

    void PactStorage::BuildMountList()
    {
        _mountTable.currentGeneration.fetch_add(1, std::memory_order_release);
        _mountTable.currentMountIndex = 0;

        _mountTable.mounts.reserve(64);
        _mountTable.mounts.clear();

        _mountTable.mountIDSet.reserve(64);
        _mountTable.mountIDSet.clear();

        for (auto& [id, manifest] : _manifestTable.handleToManifest)
        {
            PactMount& mount = _mountTable.mounts.emplace_back();

            mount.manifest = &manifest;
            mount.priority = manifest.header.priority;
            mount.mountIndex = _mountTable.currentMountIndex++;

            _mountTable.mountIDSet.insert(mount.manifest->header.manifestID);
        }

        SortMountList();
    }
    void PactStorage::SortMountList()
    {
        std::sort(_mountTable.mounts.begin(), _mountTable.mounts.end(), [](const PactMount& a, const PactMount& b)
        {
            if (a.manifest->header.sourceType != b.manifest->header.sourceType)
                return a.manifest->header.sourceType < b.manifest->header.sourceType;

            if (a.priority != b.priority)
                return a.priority < b.priority;

            return a.mountIndex < b.mountIndex;
        });
    }
    void PactStorage::BuildGlobalLookup()
    {
        _mountTable.pathTable.reserve(4096);
        _mountTable.pathTable.clear();

        _mountTable.fileIDTable.reserve(4096);
        _mountTable.fileIDTable.clear();

        for (const PactMount& mount : _mountTable.mounts)
        {
            for (const auto& entry : mount.manifest->entries)
            {
                PactFileRuntimeRecord record =
                {
                    .entry = &entry,
                    .source = &mount
                };

                if (entry.pathHash != 0)
                {
                    auto pathItr = _mountTable.pathTable.find(entry.pathHash);
                    if (pathItr == _mountTable.pathTable.end())
                    {
                        _mountTable.pathTable.emplace(entry.pathHash, record);
                    }
                    else
                    {
                        pathItr->second = record;
                    }
                }

                if (entry.fileID != 0)
                {
                    auto fileIDItr = _mountTable.fileIDTable.find(entry.fileID);
                    if (fileIDItr == _mountTable.fileIDTable.end())
                    {
                        _mountTable.fileIDTable.emplace(entry.fileID, record);
                    }
                    else
                    {
                        fileIDItr->second = record;
                    }
                }
            }
        }
    }

    PactResidentFile* PactStorage::FindOrCreateResidentFile(PactFileKey key)
    {
        auto itr = _fileKeyToResidentFile.find(key);
        if (itr != _fileKeyToResidentFile.end())
            return &itr->second;

        PactResidentFile& residentFile = _fileKeyToResidentFile[key];
        residentFile.key = key;
        residentFile.data = nullptr;
        residentFile.size = 0;
        residentFile.handleCount.store(0, std::memory_order_release);
        residentFile.pinCount.store(0, std::memory_order_release);
        residentFile.loadState.store(PactLoadState::Unloaded, std::memory_order_release);
        residentFile.owner = this;

        return &residentFile;
    }

    PactFileHandle PactStorage::CreateHandle(PactFileKey key)
    {
        std::scoped_lock lock(_residentFilesMutex);

        PactResidentFile* residentFile = FindOrCreateResidentFile(key);
        residentFile->AddHandle();

        return PactFileHandle(residentFile);
    }

    bool PactStorage::BuildOverlayManifest(PactManifest& manifest, const fs::path& absolutePath, PactManifestHandle handle, u32 priority)
    {
        std::error_code errorCode;
        if (!fs::exists(absolutePath, errorCode))
        {
            if (!fs::create_directories(absolutePath, errorCode))
            {
                NC_LOG_ERROR("Pact : Failed to create Overlay Directory (\"{0}\")", absolutePath.string());
                return false;
            }
        }

        manifest.header.version = Config::MANIFEST_VERSION;
        manifest.header.manifestID = handle;
        manifest.header.flags = { .isOverlay = true };
        manifest.header.priority = priority;
        manifest.header.sourceType = PactSourceType::Loose;
        manifest.header.entryCount = 0;
        manifest.header.reserved = 0;

        manifest.stringTable.Clear();
        manifest.entries.clear();
        manifest.dataPath = absolutePath;

        fs::recursive_directory_iterator dirpos{ absolutePath, fs::directory_options::skip_permission_denied, errorCode };
        if (errorCode)
        {
            NC_LOG_ERROR("Pact : Failed to scan Overlay Directory (\"{0}\")", absolutePath.string());
            return false;
        }

        for (const fs::directory_entry& entry : dirpos)
        {
            if (!entry.is_regular_file(errorCode) || errorCode)
            {
                errorCode.clear();
                continue;
            }

            fs::path relativePath = fs::relative(entry.path(), absolutePath, errorCode);
            if (errorCode)
            {
                errorCode.clear();
                continue;
            }

            std::string filePath = relativePath.generic_string();
            ManifestEntry& manifestEntry = manifest.entries.emplace_back();
            manifestEntry.fileID = 0;
            manifestEntry.flags = {};
            manifestEntry.pathIndex = manifest.stringTable.AddString(filePath);
            manifestEntry.pathHash = XXHash64::hash(filePath.c_str(), filePath.length(), 0);
            manifestEntry.dataOffset = 0;
            manifestEntry.dataSize = 0;
            manifestEntry.dataCompressedSize = 0;
        }

        manifest.header.entryCount = static_cast<u32>(manifest.entries.size());
        return true;
    }

    void PactStorage::EvictResidentFile(PactFileKey key)
    {
        std::scoped_lock lock(_residentFilesMutex);

        auto itr = _fileKeyToResidentFile.find(key);
        if (itr == _fileKeyToResidentFile.end())
            return;

        PactResidentFile& residentFile = itr->second;
        if (residentFile.handleCount.load(std::memory_order_acquire) != 0 || residentFile.pinCount.load(std::memory_order_acquire) != 0)
            return;

        if (residentFile.data != nullptr)
        {
            delete[] static_cast<u8*>(residentFile.data);
            residentFile.data = nullptr;
        }

        residentFile.size = 0;
        residentFile.loadState.store(PactLoadState::Unloaded, std::memory_order_release);
    }

    void PactStorage::RequestEviction(PactResidentFile& residentFile)
    {
        std::scoped_lock lock(_residentFilesMutex);

        if (residentFile.handleCount.load(std::memory_order_acquire) == 0 && residentFile.pinCount.load(std::memory_order_acquire) == 0)
        {
            if (residentFile.data != nullptr)
            {
                delete[] static_cast<u8*>(residentFile.data);
                residentFile.data = nullptr;
            }

            residentFile.size = 0;
            residentFile.loadState.store(PactLoadState::Unloaded, std::memory_order_release);
        }
    }

#if 0 // The asynchronous PACT pipeline is not ready to expose yet.
    void PactStorage::IOThreadMain()
    {
        while (!_shutdownRequested.load(std::memory_order_acquire))
        {
            PactIORequest req;
            while (_ioQueue.try_dequeue(req))
            {
                PactResidentFile* file = req.file;

                // Load data (same code as sync path)
                PactReadResult result = file->LoadData(req.record);
                file->loadState.store(result == PactReadResult::Success ? PactLoadState::Loaded : PactLoadState::Failed, std::memory_order_release);
            }

            std::this_thread::yield();
        }
    }
#endif

    PactManifestHandle PactStorage::GenerateManifestHandle()
    {
        PactManifestHandle handle = 0;
        do
        {
            handle = _manifestTable.handleGenerator.Generate();
        }
        while (_manifestTable.handleToManifest.contains(handle));

        return handle;
    }
}

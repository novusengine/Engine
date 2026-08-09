#include "ShaderCache.h"

#include <Base/Memory/Bytebuffer.h>
#include <Base/Memory/FileReader.h>
#include <Base/Util/StringUtils.h>
#include <Base/Util/DebugHandler.h>

namespace fs = std::filesystem;

namespace
{
    constexpr u32 SHADER_CACHE_MAGIC = 0x4348534Eu;

    // Increment when compiler-wide defines or reflection rules change without touching shader sources.
    constexpr u32 SHADER_COMPILER_ABI_VERSION = 2;

    i64 LastWriteStamp(const fs::path& path)
    {
        return static_cast<i64>(fs::last_write_time(path).time_since_epoch().count());
    }
}

namespace ShaderCooker
{
    bool ShaderCache::Load(fs::path path)
    {
        path = path.make_preferred();

        if (!fs::exists(path))
        {
            return false;
        }

        if (!fs::is_regular_file(path))
        {
            return false;
        }

        // Open the file
        std::string pathAsStr = path.string();
        FileReader fileReader(pathAsStr);
        if (!fileReader.Open())
        {
            return false;
        }

        // Read into a Bytebuffer
        Bytebuffer buffer(nullptr, fileReader.Length());
        fileReader.Read(&buffer, buffer.size);
        fileReader.Close();

        if (buffer.size < sizeof(u32) * 3)
            return false;

        u32 magic = 0;
        u32 compilerABIVersion = 0;
        buffer.Get<u32>(magic);
        buffer.Get<u32>(compilerABIVersion);
        if (magic != SHADER_CACHE_MAGIC || compilerABIVersion != SHADER_COMPILER_ABI_VERSION)
            return false;

        // Get number of cached files
        u32 numberOfFiles = 0;
        buffer.Get<u32>(numberOfFiles);

        // Clear our currently loaded list of files
        _filesLastTouchTime.clear();

        // Get the cached files
        for (u32 i = 0; i < numberOfFiles; i++)
        {
            u32 fileHash;
            buffer.Get<u32>(fileHash);
            i64 lastWriteTime;
            buffer.Get<i64>(lastWriteTime);

            _filesLastTouchTime.emplace(fileHash, lastWriteTime);
        }
        return true;
    }

    void ShaderCache::Save(fs::path path)
    {
        path = path.make_preferred();

        // Create a file
        std::ofstream output(path, std::ofstream::out | std::ofstream::binary);
        if (!output)
        {
            NC_LOG_ERROR("Failed to create shader cache. Check admin permissions");
            return;
        }

        // Get a bytebuffer
        std::shared_ptr<Bytebuffer> byteBuffer = Bytebuffer::Borrow<1048576>();

        byteBuffer->Put<u32>(SHADER_CACHE_MAGIC);
        byteBuffer->Put<u32>(SHADER_COMPILER_ABI_VERSION);

        // Write number of cached files
        u32 numberOfCachedFiles = static_cast<u32>(_filesLastTouchTime.size());
        byteBuffer->Put<u32>(numberOfCachedFiles);

        std::vector<u32> keys;
        keys.reserve(numberOfCachedFiles);

        for (auto key : _filesLastTouchTime)
        {
            keys.push_back(key.first);
        }

        // Write the cached files
        for (u32 i = 0; i < numberOfCachedFiles; i++)
        {
            u32 key = keys[i];
            byteBuffer->Put<u32>(key);

            i64 time = _filesLastTouchTime[key];
            byteBuffer->Put<i64>(time);
        }

        output.write(reinterpret_cast<char const*>(byteBuffer->GetDataPointer()), byteBuffer->writtenData);
        output.close();
    }

    void ShaderCache::Touch(fs::path shaderPath)
    {
        shaderPath = shaderPath.make_preferred();

        std::string shaderPathString = shaderPath.string();
        //std::transform(shaderPathString.begin(), shaderPathString.end(), shaderPathString.begin(), ::tolower);

        u32 shaderPathHash = StringUtils::fnv1a_32(shaderPathString.c_str(), shaderPathString.length());
        const i64 lastWriteTime = LastWriteStamp(shaderPath);

        if (_filesLastTouchTime.find(shaderPathHash) == _filesLastTouchTime.end())
        {
            _filesLastTouchTime.emplace(shaderPathHash, lastWriteTime);
        }
        else
        {
            _filesLastTouchTime[shaderPathHash] = lastWriteTime;
        }
    }

    bool ShaderCache::HasChanged(fs::path shaderPath)
    {
        shaderPath = shaderPath.make_preferred();

        std::string shaderPathString = shaderPath.string();
        //std::transform(shaderPathString.begin(), shaderPathString.end(), shaderPathString.begin(), ::tolower);

        u32 shaderPathHash = StringUtils::fnv1a_32(shaderPathString.c_str(), shaderPathString.length());
        const i64 lastWriteTime = LastWriteStamp(shaderPath);

        if (_filesLastTouchTime.find(shaderPathHash) == _filesLastTouchTime.end())
        {
            return true;
        }
        
        return _filesLastTouchTime[shaderPathHash] < lastWriteTime;
    }
}

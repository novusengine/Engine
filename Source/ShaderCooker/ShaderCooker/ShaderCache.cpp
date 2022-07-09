#include "ShaderCache.h"

#include <Base/Memory/Bytebuffer.h>
#include <Base/Memory/FileReader.h>
#include <Base/Util/StringUtils.h>
#include <Base/Util/DebugHandler.h>

namespace fs = std::filesystem;

namespace ShaderCooker
{
    template <typename TP>
    std::time_t to_time_t(TP tp)
    {
        using namespace std::chrono;
        auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
            + system_clock::now());
        return system_clock::to_time_t(sctp);
    }

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
        std::string filenameAsStr = path.filename().string();
        FileReader fileReader(pathAsStr, filenameAsStr);
        if (!fileReader.Open())
        {
            return false;
        }

        // Read into a Bytebuffer
        Bytebuffer buffer(nullptr, fileReader.Length());
        fileReader.Read(&buffer, buffer.size);
        fileReader.Close();

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
            std::time_t lastWriteTime;
            buffer.Get<std::time_t>(lastWriteTime);

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
            DebugHandler::PrintError("Failed to create shader cache. Check admin permissions");
            return;
        }

        // Get a bytebuffer
        std::shared_ptr<Bytebuffer> byteBuffer = Bytebuffer::Borrow<1048576>();

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

            std::time_t time = _filesLastTouchTime[key];
            byteBuffer->Put<std::time_t>(time);
        }

        output.write(reinterpret_cast<char const*>(byteBuffer->GetDataPointer()), byteBuffer->writtenData);
        output.close();
    }

    void ShaderCache::Touch(fs::path shaderPath)
    {
        shaderPath = shaderPath.make_preferred();

        std::string shaderPathString = shaderPath.string();
        std::transform(shaderPathString.begin(), shaderPathString.end(), shaderPathString.begin(), ::tolower);

        u32 shaderPathHash = StringUtils::fnv1a_32(shaderPathString.c_str(), shaderPathString.length());
        std::time_t lastWriteTime = to_time_t(fs::last_write_time(shaderPath));

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
        std::transform(shaderPathString.begin(), shaderPathString.end(), shaderPathString.begin(), ::tolower);

        u32 shaderPathHash = StringUtils::fnv1a_32(shaderPathString.c_str(), shaderPathString.length());
        std::time_t lastWriteTime = to_time_t(fs::last_write_time(shaderPath));

        if (_filesLastTouchTime.find(shaderPathHash) == _filesLastTouchTime.end())
        {
            return true;
        }
        
        return _filesLastTouchTime[shaderPathHash] < lastWriteTime;
    }
}
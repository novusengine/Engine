#pragma once
#include "Base/Types.h"
#include "Base/Memory/Bytebuffer.h"
#include "Base/Util/DebugHandler.h"

#include <filesystem>
#include <fstream>
#include <utility>

class Bytebuffer;
class FileWriter
{
public:
    FileWriter() { }
    ~FileWriter() { Close(); }

    const std::filesystem::path& GetPath() { return _path; }
    void SetPath(std::filesystem::path path)
    {
        _path = path;
    }

    bool Open(std::filesystem::path path, std::ios_base::openmode openmode = std::ofstream::out | std::ofstream::binary)
    {
        _path = std::move(path);
        _stream = std::ofstream(_path, openmode);
        if (!_stream)
        {
            NC_LOG_ERROR("[FileWriter] Failed to create/open file ({0}). Check user permissions", _path.string());
            return false;
        }

        return true;
    }

    bool Close()
    {
        if (!_stream)
            return true;

        _stream.close();
        return true;
    }

    bool Write(std::shared_ptr<Bytebuffer>& buffer)
    {
        if (!_stream)
            return false;

        _stream.write(reinterpret_cast<char const*>(buffer->GetDataPointer()), buffer->writtenData);
        return true;
    }

    bool Write(std::vector<u8>& buffer)
    {
        if (!_stream)
            return false;

        _stream.write(reinterpret_cast<char const*>(buffer.data()), buffer.size());
        return true;
    }

private:
    std::filesystem::path _path;
    std::ofstream _stream;
};

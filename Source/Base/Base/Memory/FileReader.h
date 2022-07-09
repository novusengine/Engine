#pragma once
#include "Bytebuffer.h"

#include <iostream>
#include <fstream>

class FileReader
{
public:
    FileReader(std::string& path, std::string& fileName) : _path(path), _fileName(fileName), _length(0) {}
    ~FileReader() { Close(); }

    bool Open()
    {
        _fileStream.open(_path, std::ios_base::in | std::ios_base::binary);
        if (!_fileStream)
            return false;

        // Find length of file
        _fileStream.seekg(0, _fileStream.end);
        _length = size_t(_fileStream.tellg());

        // Restore read pointer back to the start of the file
        _fileStream.seekg(0, _fileStream.beg);

        return true;
    }

    void Close()
    {
        if (_fileStream)
            _fileStream.close();
    }

    void Read(Bytebuffer* buffer, size_t length)
    {
        // Soft check to ensure we don't try to read from empty file
        if (_length == 0)
            return;

        _fileStream.read(reinterpret_cast<char*>(buffer->GetDataPointer()), length);
        buffer->writtenData += length;
    }

    std::string Path() { return _path; }
    std::string FileName() { return _fileName; }
    size_t Length() { return _length; }
    
    void SetPath(const std::string& path)
    {
        assert(!_fileStream);
        _path = path;
    }
    void SetFileName(const std::string& fileName)
    {
        assert(!_fileStream);
        _fileName = fileName;
    }

private:
    std::ifstream _fileStream;
    std::string _path;
    std::string _fileName;
    size_t _length;
};
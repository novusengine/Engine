#include "StringTable.h"

#include "Base/Memory/Bytebuffer.h"
#include "Base/Util/StringUtils.h"

namespace Novus::Container
{
    u32 StringTable::AddString(const std::string& string)
    {
        std::unique_lock<std::shared_mutex> lock(StringTable::_mutex);

        u32 stringHash = 0;

        if (string.size() > 0)
        {
            // We need the hash of the string
            stringHash = StringUtils::fnv1a_32(string.c_str(), string.length());

            // Check if the string already exists in this table, if so return that index

            u32 index = 0;
            if (TryFindHashedString(stringHash, index))
            {
                return index;
            }
        }
        else
        {
            if (_strings.size() > 0)
                return 0;
        }

        // Add the string to the table
        u32 index = static_cast<u32>(_strings.size());

        _strings.push_back(string);
        _hashes.push_back(stringHash);
        _numBytes += string.size() + 1;

        return index;
    }

    u32 StringTable::AddStringReturnByteIndex(const std::string& string)
    {
        u32 stringAddress = static_cast<u32>(_numBytes);
        AddString(string);

        return stringAddress;
    }

    const std::string& StringTable::GetString(u32 index)
    {
        std::shared_lock<std::shared_mutex> lock(StringTable::_mutex);

        assert(index < _strings.size());

        return _strings[index];
    }

    u32 StringTable::GetStringHash(u32 index)
    {
        std::shared_lock<std::shared_mutex> lock(StringTable::_mutex);

        assert(index < _hashes.size());

        return _hashes[index];
    }

    bool StringTable::Serialize(Bytebuffer* bytebuffer) const
    {
        // Here we use result as an inverse value to calculate cheaply if we serialized successfully
        bool result = false;

        // Write the number of bytes stored in the Stringtable
        if (!bytebuffer->Put<u32>(static_cast<u32>(_numBytes)))
            return false;

        // Then we go ahead and put each string
        for (const auto& string : _strings)
        {
            result |= !bytebuffer->PutBytes((u8*)(string.c_str()), string.size() + 1);
        }

        return !result;
    }

    bool StringTable::Deserialize(Bytebuffer* bytebuffer)
    {
        // First we read the total size of strings to read
        u32 totalSize;
        if (!bytebuffer->GetU32(totalSize))
        {
            assert(false);
            return false;
        }

        if (totalSize > bytebuffer->GetReadSpace())
            return false;

        if (totalSize == 0)
            return true;

        std::string string;
        u32 readSize = 0;
        while (readSize < totalSize)
        {
            if (!bytebuffer->GetString(string))
                return false;

            u32 hashedString = StringUtils::fnv1a_32(string.c_str(), string.size());
            _strings.push_back(string);
            _hashes.push_back(hashedString);
            readSize += static_cast<u32>(string.length() + 1);
        }

        _numBytes = readSize;

        return true;
    }

    void StringTable::CopyFrom(StringTable& other)
    {
        std::unique_lock<std::shared_mutex> ourLock(StringTable::_mutex);
        std::shared_lock<std::shared_mutex> theirLock(other._mutex);

        _strings = other._strings;
        _hashes = other._hashes;
        _numBytes = other._numBytes;
    }

    bool StringTable::TryFindHashedString(u32 hash, u32& index) const
    {
        for (size_t i = 0; i < _hashes.size(); i++)
        {
            if (hash == _hashes[i])
            {
                index = static_cast<u32>(i);
                return true;
            }
        }

        return false;
    }

    bool StringTable::TryFindHashedStringStrict(u32 hash, u32& index) const
    {
        for (size_t i = 0; i < _strings.size(); i++)
        {
            std::string str = _strings[i];
            std::transform(str.begin(), str.end(), str.begin(), ::tolower);

            u32 newStrHash = StringUtils::fnv1a_32(str.c_str(), str.size());
            if (hash == newStrHash)
            {
                index = static_cast<u32>(i);
                return true;
            }
        }

        return false;
    }

    void StringTable::Clear()
    {
        std::unique_lock<std::shared_mutex> ourLock(StringTable::_mutex);

        _strings.clear();
        _hashes.clear();
    }
}
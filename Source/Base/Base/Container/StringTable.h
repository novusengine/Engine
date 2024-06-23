#pragma once
#include "Base/Types.h"

#include <robinhood/robinhood.h>

#include <vector>
#include <shared_mutex>

class Bytebuffer;
class DynamicBytebuffer;
namespace Novus::Container
{
    class StringTableSafe
    {
    public:
        StringTableSafe() { }
        StringTableSafe(size_t numToReserve)
        {
            Reserve(numToReserve);
        }
        StringTableSafe(StringTableSafe& other)
        {
            CopyFrom(other);
        }

        // Add string, return index into table
        u32 AddString(const std::string& string);

        // Adds string, return byte index into string data;
        u32 AddStringReturnByteIndex(const std::string& string);

        const std::string& GetString(u32 index);
        u32 GetStringHash(u32 index);

        size_t GetNumStrings() const { return _strings.size(); }
        size_t GetNumBytes() const { return _numBytes; }

        bool Serialize(Bytebuffer* bytebuffer) const;
        bool Deserialize(Bytebuffer* bytebuffer);
        void Reserve(size_t numToReserve)
        {
            _strings.reserve(numToReserve);
            _hashes.reserve(numToReserve);
        }

        void CopyFrom(StringTableSafe& other);

        void Clear();

        bool TryFindString(const std::string& string, u32& index) const;
        bool TryFindHashedString(u32 hash, u32& index) const;
        bool TryFindHashedStringStrict(u32 hash, u32& index) const;

    private:
        std::vector<std::string> _strings;
        std::vector<u32> _hashes;
        size_t _numBytes = 0;

        std::shared_mutex _mutex;
    };

    class StringTableUnsafe
    {
    public:
        StringTableUnsafe() { }
        StringTableUnsafe(size_t numToReserve)
        {
            Reserve(numToReserve);
        }
        StringTableUnsafe(StringTableUnsafe& other)
        {
            CopyFrom(other);
        }

        // Add string, return index into table
        u32 AddString(const std::string& string);

        const std::string& GetString(u32 index) const;
        u32 GetStringHash(u32 index) const;

        size_t GetNumStrings() const { return _strings.size(); }
        size_t GetNumBytes() const { return _numBytes; }

        bool Serialize(Bytebuffer* bytebuffer) const;
        bool Deserialize(Bytebuffer* bytebuffer);
        void Reserve(size_t numToReserve)
        {
            _strings.reserve(numToReserve);
            _hashes.reserve(numToReserve);
        }

        void CopyFrom(StringTableUnsafe& other);

        void Clear();

        bool TryFindString(const std::string& string, u32& index) const;
        bool TryFindHashedString(u32 hash, u32& index) const;
        bool TryFindHashedStringStrict(u32 hash, u32& index) const;

    private:
        std::vector<std::string> _strings;
        std::vector<u32> _hashes;
        size_t _numBytes = 0;
    };
}
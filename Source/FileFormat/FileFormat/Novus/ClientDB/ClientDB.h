#pragma once
#include "FileFormat/Novus/FileHeader.h"

#include <Base/Types.h>
#include <Base/Platform.h>
#include <Base/Container/StringTable.h>
#include <Base/Memory/Bytebuffer.h>
#include <Base/Memory/FileWriter.h>

#include <robinhood/robinhood.h>
#include <vector>

class Bytebuffer;
namespace DB::Client
{
	PRAGMA_NO_PADDING_START;
	template <class T>
	struct ClientDB
	{
	public:
		static const u32 CURRENT_VERSION = 1;

		struct Flags
		{
			u32 unused : 32;
		};

        // Helper Functions
    public:
        bool ContainsIndex(u32 index)
        {
            return index < static_cast<u32>(_data.size());
        }
        const T& GetEntryByIndex(u32 index)
        {
            return _data[index];
        }
        bool ContainsEntryWithID(u32 entryID)
        {
            return _idToIndexMap.contains(entryID);
        }
        T& GetEntryByID(u32 ID)
        {
            return _data[_idToIndexMap[ID]];
        }
        T& NewEntry()
        {
            bool hasElements = _data.size() > 0;

            u32 index = Count();
            T& element = _data.emplace_back();

            // Set ID, 0 if no elements exist, otherwise _maxID + 1
            u32* id = reinterpret_cast<u32*>(&element);
            *id = (_maxID + 1) * hasElements;
            _maxID += 1 * hasElements;

            _idToIndexMap[*id] = index;

            return element;
        }

        void Clear()
        {
            flags = { };

            _data.clear();
            _idToIndexMap.clear();

            _stringTable.Clear();
        }
        u32 Count()
        {
            u32 numEntries = static_cast<u32>(_data.size());
            return numEntries;
        }

        bool HasString(u32 index)
        {
            return index < _stringTable.GetNumStrings();
        }
        const std::string& GetString(u32 index)
        {
            return _stringTable.GetString(index);
        }
        u32 AddString(const std::string& string)
        {
            return _stringTable.AddString(string);
        }

        // Save/Read/Write Helper Functions
    public:
        size_t GetSerializedSize()
        {
            size_t result = 0;

            result += sizeof(FileHeader);
            result += sizeof(Flags);
            result += sizeof(u32) + (_data.size() * sizeof(T));
            result += sizeof(u32) + _stringTable.GetNumBytes();

            return result;
        }
        bool Save(std::string& path)
        {
            size_t size = GetSerializedSize();
            std::shared_ptr<Bytebuffer> resultBuffer = Bytebuffer::BorrowRuntime(size);

            if (!Write(resultBuffer))
                return false;

            FileWriter fileWriter(path, resultBuffer);
            if (!fileWriter.Write())
                return false;

            return true;
        }
        bool Read(std::shared_ptr<Bytebuffer>& buffer)
        {
            if (!buffer->Get(header))
                return false;

            if (!buffer->Get(flags))
                return false;

            // Read Elements
            {
                _data.clear();
                _idToIndexMap.clear();

                u32 numElements = 0;
                if (!buffer->GetU32(numElements))
                    return false;

                if (numElements)
                {
                    _data.resize(numElements);

                    constexpr size_t elementSize = sizeof(T);
                    if (!buffer->GetBytes(reinterpret_cast<u8*>(&_data[0]), numElements * elementSize))
                        return false;

                    for (u32 i = 0; i < numElements; i++)
                    {
                        u32 id = *reinterpret_cast<u32*>(&_data[i]);
                        _idToIndexMap[id] = i;

                        _minID = glm::min(_minID, id);
                        _maxID = glm::max(_maxID, id);
                    }
                }
            }

            // Read Stringtable
            {
                _stringTable.Clear();

                if (!_stringTable.Deserialize(buffer.get()))
                    return false;
            }

            return true;
        }
        bool Write(std::shared_ptr<Bytebuffer>& buffer)
        {
            if (!buffer->Put(header))
                return false;

            if (!buffer->Put(flags))
                return false;

            // Read Elements
            {
                u32 numElements = static_cast<u32>(_data.size());
                if (!buffer->PutU32(numElements))
                    return false;

                if (numElements)
                {
                    if (!buffer->PutBytes(reinterpret_cast<u8*>(&_data[0]), numElements * sizeof(T)))
                        return false;
                }
            }

            if (!_stringTable.Serialize(buffer.get()))
                return false;

            return true;
        }

	public:
		FileHeader header = FileHeader(FileHeader::Type::ClientDB, CURRENT_VERSION);
		Flags flags = { };

    private:
        std::vector<T> _data = { };
        robin_hood::unordered_map<u32, u32> _idToIndexMap = { };
        Novus::Container::StringTable _stringTable = { };

        u32 _minID = 0;
        u32 _maxID = 0;
	};
	PRAGMA_NO_PADDING_END;
}
#pragma once
#include "Definitions.h"

#include "FileFormat/Novus/FileHeader.h"

#include <Base/Types.h>
#include <Base/Platform.h>
#include <Base/Container/StringTable.h>
#include <Base/Memory/Bytebuffer.h>
#include <Base/Memory/FileWriter.h>
#include <Base/Util/DebugHandler.h>

#include <robinhood/robinhood.h>

#include <fstream>
#include <string>
#include <vector>

namespace ClientDB
{
    static const std::string FILE_EXTENSION = ".cdb";

    struct StorageRaw
    {
    public:
        static const u32 CURRENT_VERSION = 1;

        struct Flags
        {
            u32 unused : 32;
        };

        StorageRaw(const std::string& name)
        {
            _name = name;
        }

    public:
        const std::string& GetName() { return _name; }

        u32 GetSizeOfElement()
        {
            return _sizeOfElement;
        }
        bool IsInitialized()
        {
            return GetSizeOfElement() != std::numeric_limits<u32>().max();
        }
        bool Init(size_t sizeOfElement)
        {
            if (IsInitialized())
                return false;

            if (sizeOfElement == 0)
                return false;

            u32 elementSize = static_cast<u32>(sizeOfElement);
            _sizeOfElement = elementSize;

            u32 numBytes = static_cast<u32>(_data.size());
            if (numBytes > 0)
            {
                u32 numElements = numBytes / elementSize;
                f32 remainder = glm::mod(static_cast<f32>(numBytes), static_cast<f32>(numElements));

                if (remainder != 0.0f)
                    return false;

                _idToIndexMap.clear();
                for (u32 i = 0; i < numElements; i++)
                {
                    u32 id = *reinterpret_cast<u32*>(&_data[i * elementSize]);

                    _idToIndexMap[id] = i;
                    _maxID = glm::max(_maxID, id);
                }
            }

            return true;
        }

        template <typename T>
        T& GetByIndex(u32 index)
        {
            u32 sizeOfElement = GetSizeOfElement();
            assert(sizeOfElement == sizeof(T));

            u32 offset = index * sizeOfElement;
            assert(offset < GetDataBytes());

            return *reinterpret_cast<T*>(&_data[offset]);
        }

        template <typename T>
        T& GetByID(u32 id)
        {
            assert(GetSizeOfElement() == sizeof(T));
            assert(_idToIndexMap.contains(id));

            u32 index = _idToIndexMap[id];

            return GetByIndex<T>(index);
        }

        template <typename T>
        bool Add(T& element)
        {
            u32 sizeOfElement = GetSizeOfElement();
            assert(sizeOfElement == sizeof(T));

            u32 newID = ++_maxID;
            element._id = newID;

            u32 sizeBeforeAdd = GetDataBytes();
            _data.resize(sizeBeforeAdd + sizeOfElement);
            memcpy(&_data[sizeBeforeAdd], &element, sizeOfElement);

            u32 index = sizeBeforeAdd / sizeOfElement;
            _idToIndexMap[newID] = index;

            MarkDirty();
            return true;
        }

        template <typename T>
        void Replace(u32 id, T& element)
        {
            u32 sizeOfElement = GetSizeOfElement();
            assert(sizeOfElement == sizeof(T));

            element._id = id;

            if (_idToIndexMap.contains(id))
            {
                u32 index = _idToIndexMap[id];
                u32 offset = index * sizeOfElement;
                assert(offset < GetDataBytes());

                *reinterpret_cast<T*>(&_data[offset]) = element;
            }
            else
            {
                u32 sizeBeforeAdd = GetDataBytes();
                _data.resize(sizeBeforeAdd + sizeOfElement);
                memcpy(&_data[sizeBeforeAdd], &element, sizeOfElement);

                u32 index = sizeBeforeAdd / sizeOfElement;
                _idToIndexMap[id] = index;
            }

            MarkDirty();
        }
        template <typename T>
        bool Copy(u32 idToCopy, u32 idForCopy)
        {
            u32 sizeOfElement = GetSizeOfElement();
            assert(sizeOfElement == sizeof(T));

            if (!_idToIndexMap.contains(idToCopy))
                return false;

            u32 indexToCopy = _idToIndexMap[idToCopy];
            T* elementToCopy = reinterpret_cast<T*>(&_data[indexToCopy * sizeOfElement]);

            T copyElement = *elementToCopy;
            Replace(idForCopy, copyElement);

            return true;
        }

        template <typename T>
        bool Remove(u32 id)
        {
            u32 sizeOfElement = GetSizeOfElement();
            assert(sizeOfElement == sizeof(T));

            if (!_idToIndexMap.contains(id))
                return false;

            u32 index = _idToIndexMap[id];
            _idToIndexMap.erase(id);

            u32 offset = index * sizeOfElement;
            assert(offset < GetDataBytes());
            *reinterpret_cast<u32*>(&_data[offset]) = std::numeric_limits<u32>().max();

            MarkDirty();
            return true;
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
            u32 numStrings = static_cast<u32>(_stringTable.GetNumStrings());

            u32 stringIndex = _stringTable.AddString(string);
            if (stringIndex == numStrings)
                MarkDirty();

            return stringIndex;
        }

        u32 Count()
        {
            u32 numEntries = static_cast<u32>(_idToIndexMap.size());
            return numEntries;
        }
        u32 GetDataBytes() { return static_cast<u32>(_data.size()); }
        bool Contains(u32 id)
        {
            return _idToIndexMap.contains(id);
        }

        bool IsDirty() { return _isDirty; }
        void MarkDirty() { _isDirty = true; }
        void ClearDirty() { _isDirty = false; }

        void Clear()
        {
            flags = { };

            _data.clear();
            _idToIndexMap.clear();
            _stringTable.Clear();
        }
        void Reserve(size_t numElements, size_t numStringsPerElement = 0)
        {
            u32 sizeOfElement = GetSizeOfElement();

            _data.reserve(numElements * sizeOfElement);
            _idToIndexMap.reserve(numElements);

            _stringTable.Reserve(numElements * numStringsPerElement);
        }
        u32 GetMaxID() { return _maxID; }
        void SetMaxID(u32 maxID)
        {
            _maxID = maxID;
        }

    public: // Save/Read/Write Helper Functions
        size_t GetSerializedSize()
        {
            size_t result = 0;

            result += sizeof(FileHeader);
            result += sizeof(Flags);
            result += sizeof(u32) + GetDataBytes();
            result += sizeof(u32) + _stringTable.GetNumBytes();

            return result;
        }
        bool Save(const std::string& path)
        {
            size_t size = GetSerializedSize();
            std::shared_ptr<Bytebuffer> resultBuffer = Bytebuffer::BorrowRuntime(size);

            if (!Write(resultBuffer))
                return false;

            FileWriter fileWriter(path, resultBuffer);
            if (!fileWriter.Write())
                return false;

            ClearDirty();
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

                u32 numBytes = 0;
                if (!buffer->GetU32(numBytes))
                    return false;

                if (numBytes)
                {
                    _data.resize(numBytes);

                    if (!buffer->GetBytes(&_data[0], numBytes))
                        return false;

                    _maxID = std::numeric_limits<u32>().max();
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
            u32 sizeOfElement = GetSizeOfElement();

            // Require Init to have been called before writing
            if (sizeOfElement == std::numeric_limits<u32>().max())
                return false;

            if (!buffer->Put(header))
                return false;

            if (!buffer->Put(flags))
                return false;

            // Write Elements
            {
                u32 numBytes = Count() * sizeOfElement;
                if (!buffer->PutU32(numBytes))
                    return false;

                if (numBytes)
                {
                    struct IdIndexPair
                    {
                    public:
                        u32 id;
                        u32 index;
                    };

                    std::vector<IdIndexPair> pairs;
                    pairs.reserve(Count());

                    for (const auto& pair : _idToIndexMap)
                    {
                        IdIndexPair& idIndexPair = pairs.emplace_back();
                        idIndexPair.id = pair.first;
                        idIndexPair.index = pair.second;
                    }

                    std::sort(pairs.begin(), pairs.end(), [&](const IdIndexPair& a, const IdIndexPair& b) { return a.id < b.id; });

                    for (const IdIndexPair& pair : pairs)
                    {
                        u32 offset = pair.index * sizeOfElement;
                        assert(offset < GetDataBytes());

                        if (!buffer->PutBytes(&_data[offset], sizeOfElement))
                            return false;
                    }
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
        template <class T>
        friend struct Storage;

        std::string _name;

        u32 _sizeOfElement = std::numeric_limits<u32>().max();
        std::vector<u8> _data;
        robin_hood::unordered_map<u32, u32> _idToIndexMap;
        Novus::Container::StringTable _stringTable = { };

        u32 _maxID = 0;
        bool _isDirty = false;
    };

	template <class T>
	struct Storage
	{
	public:
        static_assert(std::is_base_of<Definitions::Base, T>::value, "ClientDB::Storage Type must derive from Base");

        Storage(StorageRaw* storage)
        {
            _storage = storage;
        }

        // Helper Functions
    public:
        const std::string& GetName() { return _storage->GetName(); }

        bool Contains(u32 id)
        {
            return _storage->Contains(id);
        }

        T& GetByIndex(u32 index)
        {
            return _storage->GetByIndex<T>(index);
        }
        T& GetByID(u32 id)
        {
            return _storage->GetByID<T>(id);
        }

        bool Add(T& element)
        {
            return _storage->Add<T>(element);
        }
        void Replace(u32 id, T& element)
        {
            _storage->Replace<T>(id, element);
        }
        bool Copy(u32 idToCopy, u32 idForCopy)
        {
            return _storage->Copy<T>(idToCopy, idForCopy);
        }
        bool Remove(u32 id)
        {
            return _storage->Remove<T>(id);
        }
        void Clear()
        {
            _storage->Clear();
        }
        void Reserve(u32 numElements, u32 numStringsPerElement = 0)
        {
            _storage->Reserve(numElements, numStringsPerElement);
        }

        bool HasString(u32 index)
        {
            return _storage->HasString(index);
        }
        const std::string& GetString(u32 index)
        {
            return _storage->GetString(index);
        }
        u32 AddString(const std::string& string)
        {
            return _storage->AddString(string);
        }

        u32 Count() { return _storage->Count(); }
        u32 Size() { return Count(); }

        bool IsValid(const T& element) { return element.GetID() != std::numeric_limits<u32>().max(); }

        bool IsDirty() { return _storage->IsDirty(); }
        void MarkDirty() { _storage->MarkDirty(); }
        void ClearDirty() { _storage->ClearDirty(); }

    public:
        struct Iterator
        {
        public:
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T*;
            using reference = T&;

            Iterator(pointer ptr) : _ptr(ptr) {}

        public:
            reference operator*() const { return *_ptr; }
            pointer operator->() { return _ptr; }

            // Prefix increment
            Iterator& operator++() { _ptr++; return *this; }

            // Postfix increment
            Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

            friend bool operator== (const Iterator& a, const Iterator& b) { return a._ptr == b._ptr; };
            friend bool operator!= (const Iterator& a, const Iterator& b) { return a._ptr != b._ptr; };

        private:
            pointer _ptr;
        };

        auto begin() { return Iterator(reinterpret_cast<T*>(_storage->_data.data())); }
        auto end() { return Iterator(reinterpret_cast<T*>(_storage->_data.data() + _storage->_data.size())); }

    public:
        size_t GetSerializedSize()
        {
            return _storage->GetSerializedSize();
        }
        bool Save(std::string& path)
        {
            return _storage->Save(path);
        }
        bool Read(std::shared_ptr<Bytebuffer>& buffer)
        {
            return _storage->Read(buffer);
        }
        bool Write(std::shared_ptr<Bytebuffer>& buffer)
        {
            return _storage->Write(buffer);
        }

    private:
        StorageRaw* _storage = nullptr;
	};
}

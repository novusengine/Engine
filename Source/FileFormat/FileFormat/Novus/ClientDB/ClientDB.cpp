#include "ClientDB.h"

#include <Base/Util/StringUtils.h>

namespace ClientDB
{
    bool Data::Initialize(u8 numBytesPerRow)
    {
        if (_header.Flags.IsInitialized)
            return false;

        if (numBytesPerRow == 0)
            return false;

        _header.Flags.IsInitialized = true;
        _header.numBytesPerRow = numBytesPerRow;

        Grow(1);
        SetZero(0);
        Link(0, 0);
        _stringTable.AddString("");

        auto& idEntry = _idList.back();
        idEntry.id = 0;
        idEntry.index = 0;

        return true;
    }

    bool Data::Initialize(const std::vector<FieldInfo>& fieldInfos)
    {
        u32 numBytesPerRow = GetTotalSizeForFields(fieldInfos);

        if (!Initialize(numBytesPerRow))
            return false;

        _header.Flags.HasFieldInfo = true;
        _fieldInfo = fieldInfos;
        CalculateFieldOffsets();

        return true;
    }

    void Data::Reserve(u32 numRows)
    {
        NC_ASSERT(_header.Flags.IsInitialized, "ClientDB::Data - Storage must be initialized before calling Reserve");

        u32 numCurrentRows = GetNumRows();
        u32 numBytesToReserve = (_header.numBytesPerRow * numCurrentRows) + (_header.numBytesPerRow * numRows);

        _idList.reserve(numCurrentRows + numRows);
        _idToIndex.reserve(numCurrentRows + numRows);
        _data.reserve(numBytesToReserve);
    }

    void Data::Grow(u32 numRows)
    {
        NC_ASSERT(_header.Flags.IsInitialized, "ClientDB::Data - Storage must be initialized before calling Grow");

        const u32 numBytesToAddPerElement = _header.numBytesPerRow;
        const u32 numBytesToAdd = numBytesToAddPerElement * numRows;
        NC_ASSERT(numBytesToAddPerElement == _header.numBytesPerRow, "ClientDB::Data - Element being added must match the size of the Storage Container's element size");

        const u32 currentNumRows = GetNumRows();
        const u32 numRowsNeeded = currentNumRows + numRows;

        const u32 currentNumBytes = static_cast<u32>(_data.size());
        const u32 numTotalBytesNeeded = currentNumBytes + numBytesToAdd;

        _idList.resize(numRowsNeeded);
        _idToIndex.reserve(numRowsNeeded);
        _data.resize(numTotalBytesNeeded);
    }

    void Data::Clear()
    {
        _fileHeader = FileHeader(FileHeader::Type::ClientDB, CURRENT_VERSION);
        _header = { };

        _fieldInfo.clear();
        _fieldOffset.clear();

        _idList.clear();
        _data.clear();

        _idToIndex.clear();
        _stringTable.Clear();
    }

    u32 Data::GetNumRows() const
    {
        NC_ASSERT(_header.Flags.IsInitialized, "ClientDB::Data - Storage must be initialized before calling GetNumRows");

        u32 numRows = static_cast<u32>(_idList.size());
        return numRows;
    }

    u32 Data::GetNumFields() const
    {
        NC_ASSERT(_header.Flags.IsInitialized, "ClientDB::Data - Storage must be initialized before calling GetNumRows");

        u32 numFields = static_cast<u32>(_fieldInfo.size());
        return numFields;
    }

    bool Data::Copy(u32 oldID, u32& newID)
    {
        NC_ASSERT(_header.Flags.IsInitialized, "ClientDB::Data - Storage must be initialized before calling Copy");

        u32 existingDataOffset = 0;
        if (!GetLink(oldID, existingDataOffset))
            return false;

        newID = ++_header.maxID;

        u32 dataOffset = static_cast<u32>(_data.size());

        Grow(1);

        auto& idEntry = _idList.back();
        idEntry.id = newID;
        idEntry.index = dataOffset;

        Link(idEntry.id, idEntry.index);

        memcpy(&_data[dataOffset], &_data[existingDataOffset], _header.numBytesPerRow);

        return true;
    }

    bool Data::HasString(u32 stringHash)
    {
        NC_ASSERT(_header.Flags.IsInitialized, "ClientDB::Data - Storage must be initialized before calling HasString");

        u32 index = 0;
        return _stringTable.TryFindHashedString(stringHash, index);
    }

    bool Data::HasString(const std::string& string)
    {
        NC_ASSERT(_header.Flags.IsInitialized, "ClientDB::Data - Storage must be initialized before calling HasString");

        u32 index = 0;
        return _stringTable.TryFindString(string, index);
    }

    u32 Data::AddString(const std::string& string)
    {
        NC_ASSERT(_header.Flags.IsInitialized, "ClientDB::Data - Storage must be initialized before calling AddString");

        return _stringTable.AddString(string);
    }

    const std::string& Data::GetString(u32 index)
    {
        NC_ASSERT(_header.Flags.IsInitialized, "ClientDB::Data - Storage must be initialized before calling GetString");

        bool isIndexValid = index < _stringTable.GetNumStrings();
        u32 strIndex = index * isIndexValid; // Force to 0 (Default "" string, if index is invalid)

        return _stringTable.GetString(strIndex);
    }

    u32 Data::GetStringHash(u32 index)
    {
        NC_ASSERT(_header.Flags.IsInitialized, "ClientDB::Data - Storage must be initialized before calling GetStringHash");

        bool isIndexValid = index < _stringTable.GetNumStrings();
        u32 strIndex = index * isIndexValid; // Force to 0 (Default "" string, if index is invalid)

        return _stringTable.GetStringHash(strIndex);
    }

    void Data::Sort()
    {
        std::sort(_idList.begin(), _idList.end(), [](const IDListEntry& a, const IDListEntry& b)
        {
            return a.id < b.id;
        });
    }

    void Data::Compact()
    {
        Sort();

        // Clone old data
        std::vector<u8> oldData(_data);

        // Set new data to 0s
        memset(&_data[0], 0, _data.size());

        _idToIndex.clear();
        _header.maxID = 0;

        u32 nextIndex = 0;
        for (auto& idEntry : _idList)
        {
            u32 newIndex = nextIndex++;
            u32 oldIndex = idEntry.index;

            idEntry.index = newIndex * _header.numBytesPerRow;
            Link(idEntry.id, idEntry.index);

            SetData(idEntry.index, &oldData[oldIndex]);

            _header.maxID = glm::max(_header.maxID, idEntry.id);
        }

        u32 numBytesUsed = nextIndex * _header.numBytesPerRow;
        _data.resize(numBytesUsed);

        if (_header.Flags.HasFieldInfo)
        {
            struct StringFieldPairs
            {
            public:
                u16 index;
                u16 offset;
                u8 count;
            };

            u32 numFields = static_cast<u32>(_fieldInfo.size());

            std::vector<StringFieldPairs> stringFieldsPairs;
            stringFieldsPairs.reserve(numFields);

            for (u32 i = 0; i < numFields; i++)
            {
                const FieldInfo& fieldInfo = _fieldInfo[i];

                if (fieldInfo.type != FieldType::StringRef)
                    continue;

                StringFieldPairs& stringFieldPair = stringFieldsPairs.emplace_back();
                stringFieldPair.index = i;
                stringFieldPair.offset = _fieldOffset[i];
                stringFieldPair.count = fieldInfo.count;
            }

            u32 numStringFields = static_cast<u32>(stringFieldsPairs.size());
            if (numStringFields > 0)
            {
                Novus::Container::StringTableSafe oldStringTable(_stringTable);

                _stringTable.Clear();
                _stringTable.AddString("");

                for (auto& idEntry : _idList)
                {
                    for (u32 i = 0; i < numStringFields; i++)
                    {
                        StringFieldPairs& stringFieldPair = stringFieldsPairs[i];

                        for (u32 stringArrIndex = 0; stringArrIndex < stringFieldPair.count; stringArrIndex++)
                        {
                            u32* stringRef = reinterpret_cast<u32*>(&_data[idEntry.index + stringFieldPair.offset + (stringArrIndex * 4)]);
                            const std::string& stringVal = oldStringTable.GetString(*stringRef);

                            *stringRef = _stringTable.AddString(stringVal);
                        }
                    }
                }
            }
        }
    }

    size_t Data::GetSerializedSize()
    {
        size_t result = 0;

        result += sizeof(FileHeader);
        result += sizeof(Header);

        result += sizeof(u32); // Num Field Infos

        // Field Info Names
        for (const auto& fieldInfo : _fieldInfo)
        {
            result += fieldInfo.name.size() + 1;
        }
        result += (sizeof(FieldType) + sizeof(u8)) * _fieldInfo.size(); // Field Info Types
        result += sizeof(u32) * _fieldInfo.size(); // Field Offsets

        result += sizeof(u32); // Num Rows
        result += sizeof(IDListEntry) * _idList.size(); // ID List

        result += sizeof(u32); // Num Bytes (Exists to validate with the header to ensure it was properly saved)
        result += sizeof(u8) * _data.size(); // Raw Data Bytes

        result += sizeof(u32); // Num Stringtable Bytes
        result += _stringTable.GetNumBytes(); // Raw Stringtable Bytes

        return result;
    }

    bool Data::Save(const std::string& path)
    {
        Compact();

        size_t size = GetSerializedSize();
        std::shared_ptr<Bytebuffer> buffer = Bytebuffer::BorrowRuntime(size);

        // Headers
        {
            if (_fileHeader.version != CURRENT_VERSION)
                _fileHeader.version = CURRENT_VERSION;

            if (!buffer->Put(_fileHeader))
                return false;

            if (!buffer->Put(_header))
                return false;
        }

        // Field Info
        {
            u32 numFieldInfos = static_cast<u32>(_fieldInfo.size());
            if (!buffer->PutU32(numFieldInfos))
                return false;

            if (numFieldInfos > 0)
            {
                for (const auto& fieldInfo : _fieldInfo)
                {
                    if (!buffer->PutString(fieldInfo.name))
                        return false;

                    if (!buffer->Put(fieldInfo.type))
                        return false;

                    if (!buffer->PutU8(fieldInfo.count))
                        return false;
                }

                u32 numFieldOffsetBytes = numFieldInfos * sizeof(u32);
                if (!buffer->PutBytes(&_fieldOffset[0], numFieldOffsetBytes))
                    return false;
            }
        }

        // Rows & ID List
        {
            u32 numRows = GetNumRows();
            if (!buffer->PutU32(numRows))
                return false;

            u32 numIDListBytes = numRows * sizeof(IDListEntry);
            if (!buffer->PutBytes(&_idList[0], numIDListBytes))
                return false;
        }

        // Data
        {
            u32 numDataBytes = static_cast<u32>(_data.size());
            if (!buffer->PutU32(numDataBytes))
                return false;

            if (!buffer->PutBytes(&_data[0], numDataBytes))
                return false;
        }

        // Stringtable
        {
            if (!buffer->Serialize(_stringTable))
                return false;
        }

        FileWriter fileWriter(path, buffer);
        if (!fileWriter.Write())
            return false;

        return true;
    }

    bool Data::Read(std::shared_ptr<Bytebuffer>& buffer)
    {
        Clear();

        // Headers
        {
            if (!buffer->Get(_fileHeader))
                return false;

            if (!buffer->Get(_header))
                return false;
        }

        // Field Info
        {
            u32 numFieldInfos = 0;
            if (!buffer->GetU32(numFieldInfos))
                return false;

            if (numFieldInfos > 0)
            {
                _fieldInfo.resize(numFieldInfos);
                _fieldOffset.resize(numFieldInfos);

                for (auto& fieldInfo : _fieldInfo)
                {
                    if (!buffer->GetString(fieldInfo.name))
                        return false;

                    if (!buffer->Get(fieldInfo.type))
                        return false;

                    if (_fileHeader.version > 2)
                    {
                        if (!buffer->Get(fieldInfo.count))
                            return false;
                    }
                }

                u32 numFieldOffsetBytes = numFieldInfos * sizeof(u32);
                if (!buffer->GetBytes(&_fieldOffset[0], numFieldOffsetBytes))
                    return false;
            }
        }

        // Rows & ID List
        {
            u32 numRows = 0;
            if (!buffer->GetU32(numRows))
                return false;

            if (!buffer->GetVector(_idList, numRows))
                return false;

            _idToIndex.reserve(numRows);

            for (const auto& idEntry : _idList)
            {
                Link(idEntry.id, idEntry.index);
            }
        }

        // Data
        {
            u32 numDataBytes = static_cast<u32>(_data.size());
            if (!buffer->GetU32(numDataBytes))
                return false;

            if (!buffer->GetVector(_data, numDataBytes))
                return false;
        }

        // Stringtable
        {
            if (!buffer->Deserialize(_stringTable))
                return false;
        }

        return true;
    }

    bool Data::IsInitialized()
    {
        return _header.Flags.IsInitialized;
    }
    bool Data::IsDirty()
    {
        return _isDirty;
    }
    void Data::MarkDirty()
    {
        _isDirty = true;
    }
    void Data::ClearDirty()
    {
        _isDirty = false;
    }

    void Data::SetData(u32 index, const void* data, u32 numRows)
    {
        NC_ASSERT(_header.Flags.IsInitialized, "ClientDB::Data - Storage must be initialized before calling SetData");

        if (_data.size() <= index)
            return;

        memcpy(&_data[index], data, numRows * _header.numBytesPerRow);
    }

    void Data::SetZero(u32 index)
    {
        NC_ASSERT(_header.Flags.IsInitialized, "ClientDB::Data - Storage must be initialized before calling SetZero");

        if (_data.size() <= index)
            return;

        memset(&_data[index], 0, _header.numBytesPerRow);
    }

    void Data::Link(u32 id, u32 index)
    {
        NC_ASSERT(_header.Flags.IsInitialized, "ClientDB::Data - Storage must be initialized before calling Link");

        _idToIndex[id] = index;
    }

    void Data::Unlink(u32 id)
    {
        NC_ASSERT(_header.Flags.IsInitialized, "ClientDB::Data - Storage must be initialized before calling Link");

        _idToIndex.erase(id);
    }

    bool Data::HasLink(u32 id)
    {
        NC_ASSERT(_header.Flags.IsInitialized, "ClientDB::Data - Storage must be initialized before calling HasLink");

        return _idToIndex.contains(id);
    }

    bool Data::GetLink(u32 id, u32& index)
    {
        NC_ASSERT(_header.Flags.IsInitialized, "ClientDB::Data - Storage must be initialized before calling GetLink");

        index = 0;

        if (!HasLink(id))
            return false;

        index = _idToIndex[id];
        return true;
    }
    
    u32 Data::GetSizeForField(const FieldInfo& fieldInfo)
    {
        u32 fieldSize = 0;

        switch (fieldInfo.type)
        {
            case FieldType::i8:
            case FieldType::u8: fieldSize = 1; break;

            case FieldType::i16:
            case FieldType::u16: fieldSize = 2; break;

            case FieldType::i32:
            case FieldType::u32:
            case FieldType::f32:
            case FieldType::StringRef: fieldSize = 4; break;

            case FieldType::i64:
            case FieldType::u64:
            case FieldType::f64:
            case FieldType::vec2:
            case FieldType::ivec2:
            case FieldType::uvec2: fieldSize = 8; break;

            case FieldType::vec3:
            case FieldType::ivec3:
            case FieldType::uvec3: fieldSize = 12; break;

            case FieldType::vec4:
            case FieldType::ivec4:
            case FieldType::uvec4: fieldSize = 16; break;

            default: break;
        }

        fieldSize *= fieldInfo.count;
        return fieldSize;
    }

    u32 Data::GetAlignmentForField(const FieldInfo& fieldInfo)
    {
        switch (fieldInfo.type)
        {
            case FieldType::i8:
            case FieldType::u8: return 1;

            case FieldType::i16:
            case FieldType::u16: return 2;

            case FieldType::i32:
            case FieldType::u32:
            case FieldType::f32:
            case FieldType::StringRef:
            case FieldType::vec2:
            case FieldType::ivec2:
            case FieldType::uvec2:
            case FieldType::vec3:
            case FieldType::ivec3:
            case FieldType::uvec3:
            case FieldType::vec4:
            case FieldType::ivec4:
            case FieldType::uvec4: return 4;

            case FieldType::i64:
            case FieldType::u64:
            case FieldType::f64: return 8;

            default: break;
        }

        return 0;
    }

    u32 Data::GetTotalSizeForFields(const std::vector<FieldInfo>& fieldInfos)
    {
        u32 numFields = static_cast<u32>(fieldInfos.size());
        u32 totalSize = 0;
        u32 maxAlignment = 1;

        for (u32 i = 0; i < numFields; i++)
        {
            const auto& fieldInfo = fieldInfos[i];
            u32 fieldSize = GetSizeForField(fieldInfo);
            u32 fieldAlignment = GetAlignmentForField(fieldInfo);

            if (fieldAlignment > maxAlignment)
                maxAlignment = fieldAlignment;

            // Calculate padding to align the current field
            u32 padding = (fieldAlignment - (totalSize % fieldAlignment)) % fieldAlignment;

            totalSize += padding;
            totalSize += fieldSize;
        }

        // Add tail padding to align the total size to the struct's maximum alignment.
        u32 tailPadding = (maxAlignment - (totalSize % maxAlignment)) % maxAlignment;
        totalSize += tailPadding;

        return totalSize;
    }

    void Data::CalculateFieldOffsets()
    {
        u32 numFields = static_cast<u32>(_fieldInfo.size());
        _fieldOffset.clear();
        _fieldOffset.resize(numFields);

        u32 totalSize = 0;
        u32 alignment = 1;

        for (u32 i = 0; i < numFields; i++)
        {
            const auto& fieldInfo = _fieldInfo[i];
            u32 fieldSize = GetSizeForField(fieldInfo);
            u32 fieldAlignment = GetAlignmentForField(fieldInfo);

            // Calculate padding to align the current field
            u32 padding = (fieldAlignment - (totalSize % fieldAlignment)) % fieldAlignment;

            // Add padding before setting the field's offset
            totalSize += padding;

            // Assign offset for the current field
            _fieldOffset[i] = totalSize;

            // Update total size after setting the field's offset
            totalSize += fieldSize;
        }
    }
}
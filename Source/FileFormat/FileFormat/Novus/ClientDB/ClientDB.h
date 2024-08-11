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

#include <filesystem>

namespace ClientDB
{
    static const std::string FILE_EXTENSION = ".cdb";

    template <class T>
    struct Storage
    {
    public:
        static_assert(std::is_same<Definitions::Base, T>::value || std::is_base_of<Definitions::Base, T>::value, "ClientDB::Storage Type must derive from Base");
        static const u32 CURRENT_VERSION = 2;

        struct Flags
        {
            u32 unused : 32;
        };

        Storage(const std::string& name) : _name(name) { }
        ~Storage()
        {
            for (T* row : _rows)
            {
                if (row)
                    delete row;
            }
        }

        void Each(std::function<void(const Storage<T>* storage, const T*)> func) const
        {
            for (T* row : _rows)
            {
                if (!row)
                    continue;

                func(this, row);
            }
        }

        void EachAfterID(u32 id, std::function<void(const Storage<T>* storage, const T*)> func)
        {
            u32 storageMaxID = _minID + Count();
            if (id < _minID || id > storageMaxID)
                return;

            u32 count = storageMaxID - id;
            for (u32 i = 0; i < count; i++)
            {
                T* row = _rows[i - _minID];
                if (!row)
                    continue;

                func(this, row);
            }
        }
        void EachInRange(u32 min, u32 max, std::function<void(const Storage<T>* storage, const T*)> func)
        {
            u32 count = 0;

            for (T* row : _rows)
            {
                if (!row)
                    continue;

                u32 currentCount = count++;
                if (currentCount < min)
                    continue;

                if (currentCount > max)
                    break;

                func(this, row);
            }
        }

        T* GetRow(u32 id)
        {
            u32 maxID = _minID + Count();
            if (id < _minID || id > maxID)
                return nullptr;

            return _rows[id - _minID];
        }
        bool HasRow(u32 id)
        {
            const T* row = GetRow(id);
            return row != nullptr;
        }

        void AddRow(T& row)
        {
            u32 originalSize = Count();
            u32 originalMinID = _minID;
            u32 originalMaxID = originalSize == 0 ? 0 : (originalSize + originalMinID) - 1;

            // auto assign id if id is u32 max
            if (row.id == std::numeric_limits<u32>().max())
            {
                row.id = originalMaxID + 1;
            }

            bool hasNewMin = row.id < originalMinID;
            bool hasNewMax = row.id > originalMaxID || originalSize == 0;
            if (hasNewMin || hasNewMax)
            {
                u32 newMinID = glm::min(originalMinID, row.id);
                u32 newMaxID = glm::max(originalMaxID, row.id);

                u32 newSize = row.id - newMinID + 1;
                _rows.resize(newSize);

                if (hasNewMin)
                {
                    u32 numNewRows = originalMinID - newMinID;

                    memcpy(_rows.data() + numNewRows, _rows.data(), sizeof(T*) * originalSize);
                    memset(_rows.data(), 0, sizeof(T*) * numNewRows);

                    _minID = newMinID;
                }
            }

            bool hasRowAtID = _rows[row.id - _minID] != nullptr;
            _numRows += 1 * !hasRowAtID;
            _rows[row.id - _minID] = new T(row);
        }
        bool CopyRow(u32 rowToCopyID, u32 newRowID)
        {
            if (!HasRow(rowToCopyID))
                return false;

            T newRow = *_rows[rowToCopyID - _minID];
            newRow.id = newRowID;

            AddRow(newRow);
            return true;
        }

        void RemoveRow(u32 id)
        {
            u32 numRows = Count();
            u32 maxID = _minID + numRows;
            if (id < _minID || id > maxID)
                return;
            
            T* row = _rows[id - _minID];
            if (row)
            {
                delete row;

                _numRows -= 1;
                _rows[id - _minID] = nullptr;

                bool wasLastRow = id == maxID - 1;
                if (wasLastRow)
                {
                    _rows.resize(numRows - 1);
                }
            }
        }

        u32 Count()
        {
            u32 numRows = static_cast<u32>(_rows.size());
            return numRows;
        }
        u32 GetNumRows() const
        {
            return _numRows;
        }
        void Reserve(u32 count)
        {
            _rows.reserve(count);
        }

        size_t GetSerializedSize(const Novus::Container::StringTableUnsafe& stringTable)
        {
            size_t result = 0;

            result += sizeof(FileHeader);
            result += sizeof(Flags);
            result += sizeof(u32); // numRows
            result += sizeof(u32); // minID
            result += sizeof(u32) + stringTable.GetNumBytes();
            result += sizeof(T) * Count(); // Rows

            return result;
        }
        bool Read(std::shared_ptr<Bytebuffer>& buffer)
        {
            if (!buffer->Get(header))
                return false;

            if (!buffer->Get(flags))
                return false;

            if (header.type != FileHeader::Type::ClientDB)
            {
                NC_LOG_ERROR("ClientDB::Storage::Read('{0}') - Type mismatch. Got: {1}, Expected: {2}", _name, (i32)FileHeader::Type::ClientDB, (i32)header.type);
                return false;
            }

            if (header.version != CURRENT_VERSION)
            {
                NC_LOG_ERROR("ClientDB::Storage::Read('{0}') - Version mismatch. Got: {1}, Expected: {2}", _name, header.version, CURRENT_VERSION);
                return false;
            }

            u32 maxID = 0;

            if (!buffer->GetU32(_minID))
                return false;

            if (!buffer->GetU32(maxID))
                return false;

            u32 numRows = maxID - _minID + 1;
            _rows.resize(numRows);

            Novus::Container::StringTableUnsafe stringTable;
            if (!stringTable.Deserialize(buffer.get()))
                return false;

            while (buffer->GetActiveSize() > 0)
            {
                T* row = new T();
                if (!row->Read(buffer, stringTable))
                {
                    delete row;
                    return false;
                }

                if (row->id < _minID || row->id > maxID)
                {
                    NC_LOG_ERROR("ClientDB::Storage::Read('{0}') - Row ID out of bounds. Got: {1} (Min: {2}, Max: {3})", _name, row->id, _minID, maxID);

                    delete row;
                    continue;
                }

                bool hasRowAtID = _rows[row->id - _minID] != nullptr;
                _numRows += 1 * !hasRowAtID;

                _rows[row->id - _minID] = row;
            }

            return true;
        }

        bool Write(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable)
        {
            if (!buffer->Put(header))
                return false;

            if (!buffer->Put(flags))
                return false;

            u32 minID = std::numeric_limits<u32>().max();
            u32 maxID = 0;

            for (T* row : _rows)
            {
                if (!row)
                    continue;

                minID = glm::min(minID, row->id);
                maxID = glm::max(maxID, row->id);
            }

            if (minID == std::numeric_limits<u32>().max())
                minID = 0;

            if (!buffer->PutU32(minID))
                return false;

            if (!buffer->PutU32(maxID))
                return false;

            stringTable.Serialize(buffer.get());

            for (T* row : _rows)
            {
                if (!row)
                    continue;

                if (!row->Write(buffer, stringTable))
                    return false;
            }

            return true;
        }

        bool Save(const std::filesystem::path& path)
        {
            Novus::Container::StringTableUnsafe stringTable;

            for (T* row : _rows)
            {
                if (!row)
                    continue;

                if (!row->WriteStringTable(stringTable))
                    return false;
            }

            size_t size = GetSerializedSize(stringTable);
            std::shared_ptr<Bytebuffer> resultBuffer = Bytebuffer::BorrowRuntime(size);

            if (!Write(resultBuffer, stringTable))
                return false;

            FileWriter fileWriter(path, resultBuffer);
            if (!fileWriter.Write())
                return false;

            ClearDirty();
            return true;
        }

        const std::string& GetName() { return _name; }
        bool IsDirty() { return _isDirty; }
        void SetDirty() { _isDirty = true; }
        void ClearDirty() { _isDirty = false; }

    public:
        FileHeader header = FileHeader(FileHeader::Type::ClientDB, CURRENT_VERSION);
        Flags flags = { };

    private:
        std::string _name;

        u32 _minID = 0;
        u32 _numRows = 0;
        std::vector<T*> _rows;

        bool _isDirty = false;
    };
}

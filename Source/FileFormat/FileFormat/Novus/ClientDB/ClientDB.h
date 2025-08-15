#pragma once
#include "FileFormat/Novus/FileHeader.h"

#include <Base/FunctionTraits.h>
#include <Base/Types.h>
#include <Base/Container/StringTable.h>
#include <Base/Memory/Bytebuffer.h>
#include <Base/Memory/FileWriter.h>
#include <Base/Util/DebugHandler.h>

#include <robinhood/robinhood.h>

#include <concepts>
#include <vector>
#include <filesystem>
#include <type_traits>

namespace ClientDB
{
    static const std::string FILE_EXTENSION = ".cdb";

    struct Header
    {
    public:
        struct Flags
        {
            u8 IsInitialized : 1 = 0;
            u8 HasFieldInfo : 1 = 0;
        };

    public:
        Flags Flags = { };
        u8 numBytesPerRow = 0;

        u32 maxID = 0;
    };

    enum class FieldType : u8
    {
        i8,
        i16,
        i32,
        i64,
        f32,
        f64,
        StringRef,
        u8,
        u16,
        u32,
        u64,
        vec2,
        vec3,
        vec4,
        ivec2,
        ivec3,
        ivec4,
        uvec2,
        uvec3,
        uvec4
    };
    static const char* FieldTypeNames[] =
    {
        "i8", "i16", "i32", "i64", "f32", "f64", "StringRef", "u8", "u16", "u32", "u64", "vec2", "vec3", "vec4", "ivec2", "ivec3", "ivec4", "uvec2", "uvec3", "uvec4"
    };

    struct FieldInfo
    {
    public:
        std::string name;
        FieldType type;
        u8 count = 1;
    };

    struct IDListEntry
    {
    public:
        u32 id = 0;
        u32 index = 0;
    };

    template <typename T>
    concept ValidName = std::is_class_v<T> && requires { T::Name; } && std::same_as<std::decay_t<decltype(T::Name)>, std::string>;
    
    template <typename T>
    concept ValidNameHash = std::is_class_v<T> && requires { T::NameHash; } && std::same_as<std::decay_t<decltype(T::NameHash)>, u32>;

    template <typename T>
    concept ValidFieldInfo = std::is_class_v<T> && requires { T::FieldInfo; } && std::same_as<std::decay_t<decltype(T::FieldInfo)>, std::vector<FieldInfo>>;

    template <typename T>
    concept ValidClientDB = ValidName<T> && ValidNameHash<T> && ValidFieldInfo<T>;

    struct Data
    {
    public:
        inline static const u32 CURRENT_VERSION = 3;

    public:
        bool Initialize(u8 numBytesPerRow);
        bool Initialize(const std::vector<FieldInfo>& fieldInfos);
        
        // Preferred overload if T meets the requirements.
        template <typename T> requires ValidFieldInfo<T>
        bool Initialize()
        {
            return Initialize(T::FieldInfo);
        }

        // Fallback overload to produce a custom error message when T is invalid.
        template <typename T, typename = std::enable_if_t<!ValidFieldInfo<T>>>
        bool Initialize()
        {
            static_assert(ValidFieldInfo<T>, "T must be a struct or class with a static member FieldInfo of type std::vector<FieldInfo>");
            return false;
        }

        void Reserve(u32 numRows);
        void Grow(u32 numRows);
        void Clear();
        u32 GetNumRows() const;
        u32 GetNumFields() const;

        const Header& GetHeader() { return _header; }
        const std::vector<FieldInfo>& GetFields() { return _fieldInfo; }
        const std::vector<u32>& GetFieldOffsets() { return _fieldOffset; }
        const std::vector<IDListEntry>& GetIDList() { return _idList; }
        std::vector<u8>& GetData() { return _data; }

        Novus::Container::StringTableSafe& GetStringTable() { return _stringTable; }

        bool Copy(u32 oldID, u32& newID);

        template <typename T>
        u32 Add(const T& element)
        {
            return AddCount(&element, 1);
        }
        template <typename T>
        u32 AddCount(const T* elements, u32 numRows)
        {
            const u32 numBytesToAddPerElement = Assure<T>();

            u32 nextID = _header.maxID + 1;
            _header.maxID += numRows;

            const u32 currentNumRows = GetNumRows();
            const u32 currentNumBytes = static_cast<u32>(_data.size());
            Grow(numRows);

            SetData(currentNumBytes, elements, numRows);

            for (u32 i = 0; i < numRows; i++)
            {
                auto& idEntry = _idList[currentNumRows + i];

                idEntry.id = nextID + i;
                idEntry.index = currentNumBytes + (numBytesToAddPerElement * i);

                Link(idEntry.id, idEntry.index);
            }

            return nextID;
        }

        template <typename T>
        void Replace(u32 id, const T& element, bool& didOverride)
        {
            Assure<T>();

#ifdef NC_DEBUG
            if (id == 0)
            {
                NC_LOG_WARNING("ClientDB::Data - Replaced default ID 0");
            }
#endif

            _header.maxID = glm::max(_header.maxID, id);

            u32 dataOffset = 0;
            didOverride = GetLink(id, dataOffset);
            if (!didOverride)
            {
                dataOffset = static_cast<u32>(_data.size());

                Grow(1);

                auto& idEntry = _idList.back();
                idEntry.id = id;
                idEntry.index = dataOffset;

                Link(idEntry.id, idEntry.index);
            }

            SetData(dataOffset, &element);
        }
        template <typename T>
        void Replace(u32 id, const T& element)
        {
            bool didOverride = false;
            Replace<T>(id, element, didOverride);
        }

        template <typename T>
        bool Clone(u32 oldID, u32 newID, bool& didOverride)
        {
            didOverride = false;

            if (!HasLink(oldID))
                return false;

            const T& element = Get<T>(oldID);
            Replace(newID, element, didOverride);

            return true;
        }

        bool Clone(u32 oldID, u32 newID, bool& didOverride)
        {
            NC_ASSERT(_header.Flags.IsInitialized, "ClientDB::Data - Storage must be initialized before calling Clone");
            didOverride = false;

            u32 oldIndex = 0;
            if (!GetLink(oldID, oldIndex))
                return false;

            if (oldID == newID)
                return true;

            u32 newIndex = 0;
            didOverride = GetLink(newID, newIndex);
            if (!didOverride)
            {
                newIndex = static_cast<u32>(_data.size());

                Grow(1);

                auto& idEntry = _idList.back();
                idEntry.id = newID;
                idEntry.index = newIndex;

                Link(idEntry.id, idEntry.index);
            }

            SetData(newIndex, &_data[oldIndex]);
            return true;
        }

        bool HasString(u32 stringHash);
        bool HasString(const std::string& string);
        u32 AddString(const std::string& string);
        const std::string& GetString(u32 index);
        u32 GetStringHash(u32 index);

        // Documentation
        // When the provided Callback return false, the function will interrupt the iteration.
        // The returning value of the function will tell you if the iteration was interrupted (true for success, false for interrupted)
        template <typename Callback>
        bool Each(Callback callback)
        {
            // Deduce T from the second parameter of the callback
            using T = std::decay_t<std::tuple_element_t<1, function_args_t<Callback>>>;
            static_assert(std::is_invocable_r_v<bool, Callback, unsigned int, T&>, "ClientDB::Data - The Callback provided to 'Each' must return a bool and take 2 parameters (u32, T)");

            Assure<T>();

            bool wasInterrupted = false;
            for (const auto& idEntry : _idList)
            {
                T* element = reinterpret_cast<T*>(&_data[idEntry.index]);

                wasInterrupted |= !callback(idEntry.id, *element);
                if (wasInterrupted)
                    break;
            }

            return !wasInterrupted;
        }

        // Documentation
        // When the provided Callback return false, the function will interrupt the iteration.
        // The returning value of the function will tell you if the iteration was interrupted (true for success, false for interrupted)
        template <typename Callback>
        bool EachInRange(u32 startIndex, u32 count, Callback callback)
        {
            // Deduce T from the second parameter of the callback
            using T = std::decay_t<std::tuple_element_t<1, function_args_t<Callback>>>;
            static_assert(std::is_invocable_r_v<bool, Callback, unsigned int, T&>, "ClientDB::Data - The Callback provided to 'EachInRange' must return a bool and take 2 parameters (u32, T)");

            Assure<T>();

            u32 numRows = GetNumRows();
            if (startIndex >= numRows)
                return true;

            u32 maxIndex = startIndex + count;
            maxIndex = glm::min(maxIndex, numRows);

            u32 elementsToProcess = maxIndex - startIndex;

            bool wasInterrupted = false;
            for (u32 i = 0; i < elementsToProcess; i++)
            {
                const auto& idEntry = _idList[startIndex + i];
                T* element = reinterpret_cast<T*>(&_data[idEntry.index]);

                wasInterrupted |= !callback(idEntry.id, *element);
                if (wasInterrupted)
                    break;
            }

            return !wasInterrupted;
        }

        template <typename T>
        T& Get(u32 id)
        {
            Assure<T>();

            u32 dataIndex = 0;
            GetLink(id, dataIndex);

            return *reinterpret_cast<T*>(&_data[dataIndex]);
        }

        template <typename T>
        T* TryGet(u32 id)
        {
            if (id == 0)
                return nullptr;

            Assure<T>();
            u32 dataIndex = 0;
            if (!GetLink(id, dataIndex))
                return nullptr;

            return reinterpret_cast<T*>(&_data[dataIndex]);
        }

        bool Remove(u32 id)
        {
            if (!HasLink(id) || id == 0)
                return false;

            Unlink(id);
            std::erase_if(_idList, [id](const IDListEntry& idEntry)
            {
                return idEntry.id == id;
            });

            return true;
        }

        bool Has(u32 id)
        {
            return HasLink(id);
        }

        void Sort();
        void Compact();

        size_t GetSerializedSize();
        bool Save(const std::string& path);
        bool Read(std::shared_ptr<Bytebuffer>& buffer);

        bool IsInitialized();
        bool IsDirty();
        void MarkDirty();
        void ClearDirty();

        static u32 GetSizeForField(const FieldInfo& fieldInfo);
        static u32 GetAlignmentForField(const FieldInfo& fieldInfo);
        static u32 GetTotalSizeForFields(const std::vector<FieldInfo>& fieldInfos);
    private:
        template <typename T>
        u32 Assure()
        {
            static_assert(std::is_standard_layout<T>::value, "ClientDB::Data - only supports 'Standard Layout' structures");
            NC_ASSERT(_header.Flags.IsInitialized, "ClientDB::Data - Storage must be initialized before using it");

            const u32 numBytesToAddPerElement = sizeof(T);
            NC_ASSERT(numBytesToAddPerElement == _header.numBytesPerRow, "ClientDB::Data - Element being added must match the size of the Storage Container's element size");

            return numBytesToAddPerElement;
        }

        void SetData(u32 index, const void* data, u32 numRows = 1);
        void SetZero(u32 index);
        void Link(u32 id, u32 index);
        void Unlink(u32 id);
        bool HasLink(u32 id);
        bool GetLink(u32 id, u32& index);
        void CalculateFieldOffsets();

    private:
        FileHeader _fileHeader = FileHeader(FileHeader::Type::ClientDB, CURRENT_VERSION);

        Header _header;
        std::vector<FieldInfo> _fieldInfo;
        std::vector<u32> _fieldOffset;
        std::vector<IDListEntry> _idList;
        std::vector<u8> _data;

        robin_hood::unordered_map<u32, u32> _idToIndex;
        Novus::Container::StringTableSafe _stringTable;

        bool _isDirty = false;
    };
}
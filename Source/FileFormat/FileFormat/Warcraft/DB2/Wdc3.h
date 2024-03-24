#pragma once
#include <Base/Types.h>
#include <Base/Platform.h>

#include <robinhood/robinhood.h>

namespace DB2::WDC3
{
    struct Layout
    {
    public:
        struct Header
        {
            struct Flags
            {
                u16 HasOffsetMap : 1;
                u16 : 1;
                u16 HasIDList : 1;
                u16 : 1;
            };

            u32 token = 0;

            u32 recordCount = 0;
            u32 fieldCount = 0;

            u32 recordSize = 0;
            u32 stringTableSize = 0;

            u32 tableHash = 0;
            u32 layoutHash = 0;

            u32 minID = 0;
            u32 maxID = 0;

            u32 locale = 0;
            Flags flags = { };
            u16 fieldIDIndex = 0;

            u32 totalFieldCount = 0;
            u32 bitpackedDataOffset = 0;
            u32 lookupColumnCount = 0;
            u32 fieldStorageInfoSize = 0;
            u32 commonDataSize = 0;
            u32 palletDataSize = 0;

            u32 sectionCount = 0;
        };

        struct SectionHeader
        {
            u64 tactKeyHash = 0;

            u32 fileOffset = 0;
            u32 recordCount = 0;

            u32 stringTableSize = 0;
            u32 recordEndOffset = 0;
            u32 idListSize = 0;
            u32 relationshipMapDataSize = 0;
            u32 offsetMapIDCount = 0;
            u32 copyTableCount = 0;
        };
        struct FieldStructure
        {
            u16 size = 0;
            u16 offset = 0;
        };
        struct FieldStorageInfo
        {
        public:
            enum class CompressionType
            {
                None, // No Compression used
                Bitpacked, // The field is a bitpacked integer
                CommonData, // The field's data is stored inside of CommonData (Basically a default value is used)
                BitpackedIndexed, // The field is stored inside of palette data and it's index is bitpacked
                BitpackedIndexedArray, // The field is a bitpacked and it's data is located in pallet data
                BitpackedSigned // The field is a bitpacked integer, this type explicitly states the integer is signed.
            };

            u16 offsetInBits = 0;
            u16 sizeInBits = 0;

            u32 extraDataSize = 0;
            CompressionType compressionType = CompressionType::None;

            union
            {
                struct
                {
                public:
                    u32 bitpackingOffsetInBits;
                    u32 bitpackingSizeInBits;
                    u32 flags;
                } fieldBitpacked;

                struct
                {
                public:
                    u32 defaultValue;
                    u32 unused1;
                    u32 unused2;
                } fieldCommonData;

                struct
                {
                public:
                    u32 bitpackingOffsetInBits;
                    u32 bitpackingSizeInBits;
                    u32 unused1;
                } fieldBitpackedIndexed;

                struct
                {
                public:
                    u32 bitpackingOffsetInBits;
                    u32 bitpackingSizeInBits;
                    u32 arrayCount;
                } fieldBitpackedIndexedArray;

                struct
                {
                public:
                    u32 unused1;
                    u32 unused2;
                    u32 unused3;
                } fieldDefault;
            };
        };

        struct OffsetMapEntry
        {
            u32 offset = 0;
            u16 size = 0;
        };
        struct CopyTableEntry
        {
            u32 newRowID = 0;
            u32 oldRowID = 0;
        };
        struct RelationshipMapEntry
        {
            u32 foreignID = 0;
            u32 recordIndex = 0;
        };
        struct RelationshipMap
        {
            u32 entriesCount = 0;
            u32 minID = 0;
            u32 maxID = 0;
            RelationshipMapEntry* entries = nullptr;
        };

        struct Section
        {
            u8* recordData = nullptr;
            u8* stringTableData = nullptr;
            u8* variableRecordData = nullptr;
            OffsetMapEntry* offsetMapData = nullptr;
            u32* idListData = nullptr;
            CopyTableEntry* copyTable = nullptr;

            RelationshipMap relationshipMap = { };
            u32* offsetMapList = nullptr;

            bool isEncrypted = false;
            robin_hood::unordered_map<u32, u32> recordIndexToForeignID = { };
        };

    public:
        Header header = { };

        std::vector<SectionHeader> sectionHeaders = { };
        std::vector<FieldStructure> fieldStructures = { };
        std::vector<FieldStorageInfo> fieldStorageInfos = { };
        std::vector<robin_hood::unordered_map<u32, u32>> perFieldCommonDataIDToValue = { };
        std::vector<std::vector<u32>> perFieldPalleteData = { };
        std::vector<Section> sections = { };
    };
}

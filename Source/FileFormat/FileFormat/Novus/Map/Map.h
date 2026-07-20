#pragma once
#include "FileFormat/Shared.h"
#include "FileFormat/Novus/FileHeader.h"

#include <Base/Types.h>
#include <Base/Platform.h>

namespace Map
{
    static const std::string HEADER_FILE_EXTENSION = ".map";

    struct MapHeader
    {
    public:
        static const u32 CURRENT_VERSION = 3;

        struct Flags
        {
            u32 UseMapObjectAsBase : 1;
        };

    public:
        FileHeader header = FileHeader(FileHeader::Type::MapHeader, CURRENT_VERSION);

        Flags flags = { };
        Terrain::Placement placement = { };
        std::vector<u64> chunkHashes;

    public:
        bool Save(std::shared_ptr<Bytebuffer>& buffer);
        static bool Read(std::shared_ptr<Bytebuffer>& buffer, MapHeader& out);
    };
}

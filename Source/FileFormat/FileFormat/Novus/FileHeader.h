#pragma once
#include <Base/Types.h>
#include <Base/Platform.h>

#include <limits>

struct FileHeader
{
public:
    enum class Type
    {
        Invalid = -1,
        ClientDB,
        MapHeader,
        MapChunk,
        MapObject,
        MapObjectGroup,
        ComplexModel
    };

public:
    FileHeader() { }
    FileHeader(Type inType, u32 inVersion)
    {
        type = inType;
        version = inVersion;
    }

    bool operator==(const FileHeader& other)
    {
        return type == other.type && version == other.version;
    }

    bool operator!=(const FileHeader& other)
    {
        return type != other.type || version != other.version;
    }

public:
    Type type = Type::Invalid;
    u32 version = 0;
};

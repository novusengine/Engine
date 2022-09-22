#pragma once
#include <Base/Types.h>

namespace DB::Client::Definitions
{
    struct Map
    {
        u32 id;
        u32 name;
        u32 internalName;
        u32 instanceType;
        u32 flags;
        u32 expansion;
        u32 maxPlayers;
    };
}
#pragma once
#include "Filesystem/Core/Mount.h"

#include <Base/Types.h>

namespace PACT
{
    struct Config
    {
    public:
        static constexpr uvec3 ROOT_VERSION = uvec3(0, 0, 2);
        static constexpr uvec3 MANIFEST_VERSION = uvec3(0, 0, 2);
        static constexpr u32 CDC_MIN_SIZE = 256u * 1024u;
        static constexpr u32 CDC_AVG_SIZE = 1u * 1024u * 1024u;
        static constexpr u32 CDC_MAX_SIZE = 4u * 1024u * 1024u;
        static constexpr u64 MAX_MANIFEST_DATA_SIZE = 1ull * 1024ull * 1024ull * 1024ull;
        static constexpr u64 LOCAL_MANIFEST_ID_START = 10'000;
        static constexpr u64 LOCAL_FILE_ID_START = 1'000'000;

        static inline std::string BASE_DIR = "data";
        static inline std::string MANIFEST_DIR = "manifests";
        static inline std::string DATA_DIR = "data";
        static inline std::string ROOT_FILE = "root.pact";

        static inline std::string MANIFEST_EXT = ".manifest";
        static inline std::string DATA_EXT = ".bin";
    };
}

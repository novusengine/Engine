#pragma once
#include "Filesystem/Core/Mount.h"

#include <Base/Types.h>

namespace PACT
{
    struct Config
    {
    public:
        static constexpr uvec3 ROOT_VERSION = uvec3(0, 0, 1);
        static constexpr uvec3 MANIFEST_VERSION = uvec3(0, 0, 1);
        static inline std::string BASE_DIR = "data";
        static inline std::string MANIFEST_DIR = "manifests";
        static inline std::string DATA_DIR = "data";
        static inline std::string ROOT_FILE = "root.pact";

        static inline std::string MANIFEST_EXT = ".manifest";
        static inline std::string DATA_EXT = ".bin";
    };
}
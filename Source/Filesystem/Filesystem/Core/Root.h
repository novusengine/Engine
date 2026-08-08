#pragma once

#include "Filesystem/Core/Chunk.h"
#include "Filesystem/Core/Origin.h"

#include <Base/Types.h>

#include <vector>

class Bytebuffer;

namespace PACT
{
    struct PactFeatureSet
    {
    public:
        u32 chunking : 1;       // 1 = content-defined chunking is enabled.
        u32 hashAlgo : 3;       // 0 = SHA-256.
        u32 cdcAlgo  : 3;       // 0 = PACT FastCDC v1.
        u32          : 25;

        u32 cdcMinSize;
        u32 cdcAvgSize;
        u32 cdcMaxSize;
    };

    struct PactManifestRef
    {
    public:
        PactDigest digest = {}; // SHA-256 of the serialized manifest blob.
        u64 manifestID = 0;     // Stable logical ID.
        u32 priority = 0;
        u32 storageMask = 0;    // Dev, Server, and Client pre-download mask.
        PactManifestOrigin origin = PactManifestOrigin::Remote;
    };

    enum class PactRootValidateResult
    {
        Success,
        InvalidMagic,
        MismatchVersion,
        InvalidFeatureSet,
        InvalidManifestRefs
    };

    struct PactRoot
    {
    public:
        static constexpr size_t SERIALIZED_HEADER_SIZE = 36;
        static constexpr size_t SERIALIZED_MANIFEST_REF_SIZE = 52;

        char magic[4] = { 'P', 'A', 'C', 'T' };
        uvec3 version = {};
        PactFeatureSet featureSet = {};
        std::vector<PactManifestRef> manifestRefs;

    public:
        size_t GetSerializedSize() const;
        bool Serialize(Bytebuffer* buffer) const;
        bool Deserialize(Bytebuffer* buffer);
        PactRootValidateResult Validate() const;
    };
}

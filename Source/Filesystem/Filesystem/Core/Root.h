#pragma once

#include <Base/Types.h>

class Bytebuffer;

namespace PACT
{
    struct PactFeatureSet
    {
        u64 : 64;
    };

    enum class PactRootValidateResult
    {
        Success,
        InvalidMagic,
        MismatchVersion
    };

    struct PactRoot
    {
    public:
        char magic[4] = { 'P', 'A', 'C', 'T' };
        uvec3 version;
        PactFeatureSet featureSet;

    public:
        bool Serialize(Bytebuffer* buffer) const;
        bool Deserialize(Bytebuffer* buffer);
        PactRootValidateResult Validate() const;
    };
}
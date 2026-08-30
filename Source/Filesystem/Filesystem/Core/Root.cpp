#include "Root.h"
#include "Filesystem/Config.h"
#include "Filesystem/Core/Serialization.h"

#include <Base/Memory/Bytebuffer.h>

#include <algorithm>
#include <limits>
#include <set>
#include <unordered_set>

namespace PACT
{
    size_t PactRoot::GetSerializedSize() const
    {
        return SERIALIZED_HEADER_SIZE + manifestRefs.size() * SERIALIZED_MANIFEST_REF_SIZE;
    }

    bool PactRoot::Serialize(Bytebuffer* buffer) const
    {
        if (Validate() != PactRootValidateResult::Success || manifestRefs.size() > std::numeric_limits<u32>::max())
            return false;

        const u32 featureFlags = featureSet.chunking |
            (featureSet.hashAlgo << 1) |
            (featureSet.cdcAlgo << 4);

        bool failed = false;
        failed |= !buffer->PutBytes(magic, sizeof(magic));
        failed |= !Serialization::WriteU32(buffer, version.x);
        failed |= !Serialization::WriteU32(buffer, version.y);
        failed |= !Serialization::WriteU32(buffer, version.z);
        failed |= !Serialization::WriteU32(buffer, featureFlags);
        failed |= !Serialization::WriteU32(buffer, featureSet.cdcMinSize);
        failed |= !Serialization::WriteU32(buffer, featureSet.cdcAvgSize);
        failed |= !Serialization::WriteU32(buffer, featureSet.cdcMaxSize);
        failed |= !Serialization::WriteU32(buffer, static_cast<u32>(manifestRefs.size()));

        for (const PactManifestRef& manifestRef : manifestRefs)
        {
            failed |= !buffer->PutBytes(manifestRef.digest.data(), manifestRef.digest.size());
            failed |= !Serialization::WriteU64(buffer, manifestRef.manifestID);
            failed |= !Serialization::WriteU32(buffer, manifestRef.priority);
            failed |= !Serialization::WriteU32(buffer, manifestRef.storageMask);
            failed |= !Serialization::WriteU32(buffer, static_cast<u32>(manifestRef.origin));
        }

        return !failed;
    }
    bool PactRoot::Deserialize(Bytebuffer* buffer)
    {
        manifestRefs.clear();

        u32 featureFlags = 0;
        PactFeatureSet tempFeatureSet = {};
        u32 manifestCount = 0;

        bool failed = false;
        failed |= !buffer->GetBytes(magic, sizeof(magic));
        failed |= !Serialization::ReadU32(buffer, version.x);
        failed |= !Serialization::ReadU32(buffer, version.y);
        failed |= !Serialization::ReadU32(buffer, version.z);
        failed |= !Serialization::ReadU32(buffer, featureFlags);
        failed |= !Serialization::ReadU32(buffer, tempFeatureSet.cdcMinSize);
        failed |= !Serialization::ReadU32(buffer, tempFeatureSet.cdcAvgSize);
        failed |= !Serialization::ReadU32(buffer, tempFeatureSet.cdcMaxSize);
        failed |= !Serialization::ReadU32(buffer, manifestCount);

        if (failed)
            return false;

        featureSet = tempFeatureSet;
        featureSet.chunking = featureFlags & 0x1u;
        featureSet.hashAlgo = (featureFlags >> 1) & 0x7u;
        featureSet.cdcAlgo = (featureFlags >> 4) & 0x7u;

        if ((featureFlags & ~0x7Fu) != 0 || manifestCount > buffer->GetActiveSize() / SERIALIZED_MANIFEST_REF_SIZE)
            return false;

        manifestRefs.resize(manifestCount);
        for (PactManifestRef& manifestRef : manifestRefs)
        {
            failed |= !buffer->GetBytes(manifestRef.digest.data(), manifestRef.digest.size());
            failed |= !Serialization::ReadU64(buffer, manifestRef.manifestID);
            failed |= !Serialization::ReadU32(buffer, manifestRef.priority);
            failed |= !Serialization::ReadU32(buffer, manifestRef.storageMask);

            u32 origin = 0;
            failed |= !Serialization::ReadU32(buffer, origin);
            if (origin > static_cast<u32>(PactManifestOrigin::Local))
                return false;

            manifestRef.origin = static_cast<PactManifestOrigin>(origin);
        }

        return !failed;
    }
    PactRootValidateResult PactRoot::Validate() const
    {
        const bool invalidMagic = magic[0] != 'P' || magic[1] != 'A' || magic[2] != 'C' || magic[3] != 'T';
        if (invalidMagic)
            return PactRootValidateResult::InvalidMagic;

        if (version != Config::ROOT_VERSION)
            return PactRootValidateResult::MismatchVersion;

        if (featureSet.hashAlgo != 0 || featureSet.cdcAlgo != 0)
            return PactRootValidateResult::InvalidFeatureSet;

        if (featureSet.chunking && (featureSet.cdcMinSize == 0 || featureSet.cdcMinSize > featureSet.cdcAvgSize || featureSet.cdcAvgSize > featureSet.cdcMaxSize))
            return PactRootValidateResult::InvalidFeatureSet;

        if (!featureSet.chunking && (featureSet.cdcMinSize != 0 || featureSet.cdcAvgSize != 0 || featureSet.cdcMaxSize != 0))
            return PactRootValidateResult::InvalidFeatureSet;

        std::unordered_set<u64> manifestIDs;
        std::set<PactDigest> manifestDigests;
        for (const PactManifestRef& manifestRef : manifestRefs)
        {
            const bool hasDigest = std::any_of(manifestRef.digest.begin(), manifestRef.digest.end(), [](u8 value)
            {
                return value != 0;
            });
            if (!hasDigest || manifestRef.manifestID == std::numeric_limits<u64>::max() || manifestRef.storageMask > 0x7u ||
                manifestRef.origin > PactManifestOrigin::Local ||
                !manifestIDs.insert(manifestRef.manifestID).second || !manifestDigests.insert(manifestRef.digest).second)
            {
                return PactRootValidateResult::InvalidManifestRefs;
            }
        }

        return PactRootValidateResult::Success;
    }
}

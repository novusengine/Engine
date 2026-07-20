#include "Manifest.h"
#include "Filesystem/Config.h"

#include <Base/Memory/Bytebuffer.h>

namespace PACT
{
    bool PactManifest::Serialize(Bytebuffer* buffer) const
    {
        bool failed = false;

        failed |= !buffer->Put(header);
        failed |= !buffer->PutBytes(entries.data(), entries.size() * sizeof(ManifestEntry));
        failed |= !buffer->Serialize(stringTable);

        return !failed;
    }
    bool PactManifest::Deserialize(Bytebuffer* buffer)
    {
        bool failed = false;

        failed |= !buffer->Get(header);

        entries.resize(header.entryCount);
        failed |= !buffer->GetBytes(entries.data(), entries.size() * sizeof(ManifestEntry));
        failed |= !buffer->Deserialize(stringTable);

        return !failed;
    }
    PactManifestValidateResult PactManifest::Validate() const
    {
        bool invalidMagic = false;
        invalidMagic |= header.magic[0] != 'P';
        invalidMagic |= header.magic[1] != 'A';
        invalidMagic |= header.magic[2] != 'M';
        invalidMagic |= header.magic[3] != 'F';

        if (invalidMagic)
            return PactManifestValidateResult::InvalidMagic;

        if (header.version != Config::MANIFEST_VERSION)
            return PactManifestValidateResult::MismatchVersion;

        return PactManifestValidateResult::Success;
    }

    PactManifest& PactManifest::operator=(PactManifest&& other) noexcept
    {
        std::swap(header, other.header);
        std::swap(entries, other.entries);
        std::swap(stringTable, other.stringTable);
        std::swap(dataPath, other.dataPath);

        return *this;
    }
}

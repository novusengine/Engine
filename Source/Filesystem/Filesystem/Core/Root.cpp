#include "Root.h"
#include "Filesystem/Config.h"

#include <Base/Memory/Bytebuffer.h>

namespace PACT
{
    bool PactRoot::Serialize(Bytebuffer* buffer) const
    {
        bool failed = false;

        failed |= !buffer->Put(*this);

        return !failed;
    }
    bool PactRoot::Deserialize(Bytebuffer* buffer)
    {
        bool failed = false;

        failed |= !buffer->Get(*this);

        return !failed;
    }
    PactRootValidateResult PactRoot::Validate() const
    {
        bool invalidMagic = false;
        invalidMagic |= magic[0] != 'P';
        invalidMagic |= magic[1] != 'A';
        invalidMagic |= magic[2] != 'C';
        invalidMagic |= magic[3] != 'T';

        if (invalidMagic)
            return PactRootValidateResult::InvalidMagic;

        if (version != Config::ROOT_VERSION)
            return PactRootValidateResult::MismatchVersion;

        return PactRootValidateResult::Success;
    }
}

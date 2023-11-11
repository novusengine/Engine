#include "Map.h"

namespace Map
{
    bool MapHeader::Read(std::shared_ptr<Bytebuffer>& buffer, MapHeader& out)
    {
        if (!buffer->Get(out))
            return false;

        if (out.header.type != FileHeader::Type::MapHeader || out.header.version != MapHeader::CURRENT_VERSION)
            return false;

        return true;
    }
}
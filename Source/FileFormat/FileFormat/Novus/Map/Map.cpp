#include "Map.h"

#include <Base/Types.h>
#include <Base/Util/DebugHandler.h>

#include <fstream>

namespace Map
{
    bool MapHeader::Save(const std::string& path)
	{
		std::ofstream output(path, std::ofstream::out | std::ofstream::binary);
		if (!output)
		{
			DebugHandler::PrintError("Failed to create Map Header file. Check admin permissions {0}", path);
			return false;
		}

        output.write(reinterpret_cast<char const*>(&header), sizeof(header));
        output.write(reinterpret_cast<char const*>(&flags), sizeof(flags));
        output.write(reinterpret_cast<char const*>(&placement), sizeof(placement));

        output.close();

        return true;
    }
    bool MapHeader::Read(std::shared_ptr<Bytebuffer>& buffer, MapHeader& out)
    {
        if (!buffer->Get(out))
            return false;

        if (out.header.type != FileHeader::Type::MapHeader || out.header.version != MapHeader::CURRENT_VERSION)
            return false;

        return true;
    }
}
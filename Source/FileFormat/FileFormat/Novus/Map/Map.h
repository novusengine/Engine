#pragma once
#include "FileFormat/Shared.h"
#include "FileFormat/Novus/FileHeader.h"

#include <Base/Types.h>
#include <Base/Platform.h>

namespace Map
{
PRAGMA_NO_PADDING_START;
	struct Layout
	{
	public:
		static const u32 CURRENT_VERSION = 1;

		struct Flags
		{
			u32 UseMapObjectAsBase : 1;
		};

	public:
		FileHeader header = FileHeader(FileHeader::Type::Map, CURRENT_VERSION);

		Flags flags = { };
		Terrain::Placement placement = { };
	};
PRAGMA_NO_PADDING_END;
}

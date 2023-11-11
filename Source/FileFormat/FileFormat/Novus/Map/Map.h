#pragma once
#include "FileFormat/Shared.h"
#include "FileFormat/Novus/FileHeader.h"

#include <Base/Types.h>
#include <Base/Platform.h>

namespace Map
{
	struct MapHeader
	{
	public:
		static const u32 CURRENT_VERSION = 2;

		struct Flags
		{
			u32 UseMapObjectAsBase : 1;
		};

	public:
		FileHeader header = FileHeader(FileHeader::Type::MapHeader, CURRENT_VERSION);

		Flags flags = { };
		Terrain::Placement placement = { };

	public:
		static bool Read(std::shared_ptr<Bytebuffer>& buffer, MapHeader& out);
	};
}

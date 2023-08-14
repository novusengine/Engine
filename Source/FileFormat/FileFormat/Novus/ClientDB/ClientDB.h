#pragma once
#include "FileFormat/Novus/FileHeader.h"

#include <Base/Types.h>
#include <Base/Platform.h>
#include <Base/Container/StringTable.h>

#include <robinhood/robinhood.h>
#include <vector>

class Bytebuffer;
namespace DB::Client
{
	PRAGMA_NO_PADDING_START;
	template <class T>
	struct ClientDB
	{
	public:
		static const u32 CURRENT_VERSION = 1;

		struct Flags
		{
			u32 unused : 32;
		};

    public:
        bool HasEntry(u32 entryID)
        {
            return idToIndexMap.contains(entryID);
        }
        const T& GetEntry(u32 entryID)
        {
            return data[idToIndexMap[entryID]];
        }

	public:
		FileHeader header = FileHeader(FileHeader::Type::ClientDB, CURRENT_VERSION);

		Flags flags = { };
		std::vector<T> data = { };
        StringTable stringTable = { };
        robin_hood::unordered_map<u32, u32> idToIndexMap = { };

	public:
		bool Save(std::string& path)
		{
            // Create a file
            std::ofstream output(path, std::ofstream::out | std::ofstream::binary);
            if (!output)
            {
                DebugHandler::PrintError("Failed to create Terrain Chunk file. Check admin permissions");
                return false;
            }

            // Write the Chunk to file
            output.write(reinterpret_cast<char const*>(&header), sizeof(FileHeader));
            output.write(reinterpret_cast<char const*>(&flags), sizeof(u32));

            u32 numElements = static_cast<u32>(data.size());
            output.write(reinterpret_cast<char const*>(&numElements), sizeof(u32)); // Write number of elements

            if (numElements > 0)
            {
                output.write(reinterpret_cast<char const*>(data.data()), data * sizeof(T)); // Write elements
            }

            output.close();

            return true;
		}

        size_t GetSerializedSize()
        {
            size_t result = 0;

            result += sizeof(FileHeader);
            result += sizeof(Flags);
            result += sizeof(u32) + (data.size() * sizeof(T));
            result += sizeof(u32) + stringTable.GetNumBytes();

            return result;
        }

		bool Write(std::shared_ptr<Bytebuffer>& buffer)
		{
            if (!buffer->Put(header))
                return false;

            if (!buffer->Put(flags))
                return false;

            // Read Elements
            {
                u32 numElements = static_cast<u32>(data.size());
                if (!buffer->PutU32(numElements))
                    return false;

                if (numElements)
                {
                    if (!buffer->PutBytes(reinterpret_cast<u8*>(&data[0]), numElements * sizeof(T)))
                        return false;
                }
            }

            if (!stringTable.Serialize(buffer.get()))
                return false;

            return true;
		}

		bool Read(std::shared_ptr<Bytebuffer>& buffer)
		{
			if (!buffer->Get(header))
				return false;

			if (!buffer->Get(flags))
				return false;

            // Read Elements
            {
                data.clear();

                u32 numElements = 0;
                if (!buffer->GetU32(numElements))
                    return false;

                if (numElements)
                {
                    data.resize(numElements);
                    if (!buffer->GetBytes(reinterpret_cast<u8*>(&data[0]), numElements * sizeof(T)))
                        return false;
                }
            }

            // Read Stringtable
            {
                stringTable.Clear();

                if (!stringTable.Deserialize(buffer.get()))
                    return false;
            }

            return true;
		}
	};
	PRAGMA_NO_PADDING_END;
}
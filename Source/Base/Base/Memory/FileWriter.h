#pragma once
#include "Base/Types.h"
#include "Base/Memory/Bytebuffer.h"
#include "Base/Util/DebugHandler.h"

#include <fstream>
#include <filesystem>

class Bytebuffer;
class FileWriter
{
public:
	FileWriter(std::filesystem::path path, std::shared_ptr<Bytebuffer>& buffer) : _path(path), _buffer(buffer) { }

	bool Write()
	{
		// Create a file
		std::ofstream output(_path, std::ofstream::out | std::ofstream::binary);
		if (!output)
		{
			DebugHandler::PrintError("[FileWriter] Failed to create file ({0}). Check admin permissions", _path.string().c_str());
			return false;
		}

		output.write(reinterpret_cast<char const*>(_buffer->GetDataPointer()), _buffer->writtenData);

		output.close();
		return true;
	}

	const std::filesystem::path& GetPath() { return _path; }

private:
	std::filesystem::path _path;
	std::shared_ptr<Bytebuffer> _buffer;
};
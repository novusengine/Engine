#pragma once
#include <Base/Types.h>

#include <robinhood/robinhood.h>

class Bytebuffer;
struct FileChunkHeader;

namespace M2
{
	struct Layout;
}

class M2Parser
{
public:
	enum class ParseType
	{
		Root,
		Skin
	};

	M2Parser() { }

	bool TryParse(const ParseType parseType, std::shared_ptr<Bytebuffer>& buffer, M2::Layout& out);

private:
	bool ParseBufferOrderIndependent(std::shared_ptr<Bytebuffer>& buffer, M2::Layout& out);
	bool ParseSkinBuffer(std::shared_ptr<Bytebuffer>& buffer, M2::Layout& out);

private:
	static bool ReadMD21(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, M2::Layout& layout);
	static bool ReadSFID(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, M2::Layout& layout);
	static bool ReadAFID(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, M2::Layout& layout);
	static bool ReadBFID(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, M2::Layout& layout);
	static bool ReadTXID(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, M2::Layout& layout);

private:
	static robin_hood::unordered_map<u32, std::function<bool(const FileChunkHeader&, std::shared_ptr<Bytebuffer>&, M2::Layout&)>> _m2FileChunkToFunction;
};
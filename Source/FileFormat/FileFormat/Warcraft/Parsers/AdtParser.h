#pragma once
#include <Base/Types.h>

#include <robinhood/robinhood.h>

class Bytebuffer;
struct FileChunkHeader;

namespace ADT
{
	struct WDT;
	struct Layout;
}

class AdtParser
{
public:
	enum class ParseType
	{
		Root,
		Texture,
		Object
	};

	AdtParser() { }

	bool TryParse(std::shared_ptr<Bytebuffer>& rootBuffer, std::shared_ptr<Bytebuffer>& textureBuffer, std::shared_ptr<Bytebuffer>& objectBuffer, const ADT::WDT& wdt, ADT::Layout& out);

private:
	struct ParseContext
	{
		i32 currentMCNKIndex = -1;
	};

	bool ParseBufferOrderIndependent(ParseContext& context, ParseType parseType, std::shared_ptr<Bytebuffer>& buffer, const ADT::WDT& wdt, ADT::Layout& adt);

private:
	static bool ReadMVER(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const ADT::WDT& wdt, ADT::Layout& adt);
	static bool ReadMHDR(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const ADT::WDT& wdt, ADT::Layout& adt);
	static bool ReadMDID(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const ADT::WDT& wdt, ADT::Layout& adt);
	static bool ReadMHID(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const ADT::WDT& wdt, ADT::Layout& adt);
	static bool ReadMDDF(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const ADT::WDT& wdt, ADT::Layout& adt);
	static bool ReadMODF(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const ADT::WDT& wdt, ADT::Layout& adt);
	static bool ReadMFBO(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const ADT::WDT& wdt, ADT::Layout& adt);
	static bool ReadMTXF(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const ADT::WDT& wdt, ADT::Layout& adt);
	static bool ReadMH2O(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const ADT::WDT& wdt, ADT::Layout& adt);

	static bool ReadMCNK(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const ADT::WDT& wdt, ADT::Layout& adt);
	static bool ReadMCVT(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const ADT::WDT& wdt, ADT::Layout& adt);
	static bool ReadMCLY(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const ADT::WDT& wdt, ADT::Layout& adt);
	static bool ReadMCAL(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const ADT::WDT& wdt, ADT::Layout& adt);
	static bool ReadMCNR(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const ADT::WDT& wdt, ADT::Layout& adt);
	static bool ReadMCCV(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const ADT::WDT& wdt, ADT::Layout& adt);

private:
	static robin_hood::unordered_map<u32, std::function<bool(ParseContext& context, const AdtParser::ParseType parseType, const FileChunkHeader&, std::shared_ptr<Bytebuffer>&, const ADT::WDT&, ADT::Layout&)>> _adtFileChunkToFunction;
};
#pragma once
#include <Base/Types.h>

#include <robinhood/robinhood.h>

class Bytebuffer;
struct FileChunkHeader;

namespace Adt
{
	struct Wdt;
	struct Layout;

	class Parser
	{
	public:
		enum class ParseType
		{
			Root,
			Texture,
			Object
		};

		Parser() { }

		bool TryParse(std::shared_ptr<Bytebuffer>& rootBuffer, std::shared_ptr<Bytebuffer>& textureBuffer, std::shared_ptr<Bytebuffer>& objectBuffer, const Adt::Wdt& wdt, Adt::Layout& out);

	private:
		struct ParseContext
		{
			i32 currentMCNKIndex = -1;
		};

		bool ParseBufferOrderIndependent(ParseContext& context, ParseType parseType, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);

	private:
		static bool ReadMVER(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMHDR(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMDID(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMHID(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMDDF(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMODF(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMFBO(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMTXF(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMH2O(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);

		static bool ReadMCNK(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMCVT(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMCLY(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMCAL(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMCNR(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMCCV(ParseContext& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);

	private:
		static robin_hood::unordered_map<u32, std::function<bool(ParseContext& context, const Parser::ParseType parseType, const FileChunkHeader&, std::shared_ptr<Bytebuffer>&, const Adt::Wdt&, Adt::Layout&)>> _adtFileChunkToFunction;
	};
}
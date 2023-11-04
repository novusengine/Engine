#pragma once
#include "FileFormat/Novus/ClientDB/ClientDB.h"
#include "FileFormat/Novus/ClientDB/Definitions.h"

#include <Base/Types.h>

#include <robinhood/robinhood.h>

using namespace ClientDB;

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

		struct Context
		{
			i32 currentMCNKIndex = -1;

			Storage<Definitions::LiquidObject>* liquidObjects = nullptr;
			Storage<Definitions::LiquidType>* liquidTypes = nullptr;
			Storage<Definitions::LiquidMaterial>* liquidMaterials = nullptr;
		};

		Parser() { }

		bool TryParse(Context& parseContext, std::shared_ptr<Bytebuffer>& rootBuffer, std::shared_ptr<Bytebuffer>& textureBuffer, std::shared_ptr<Bytebuffer>& objectBuffer, const Adt::Wdt& wdt, Adt::Layout& out);

	private:
		bool ParseBufferOrderIndependent(Context& context, ParseType parseType, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);

	private:
		static bool ReadMVER(Context& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMHDR(Context& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMDID(Context& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMHID(Context& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMDDF(Context& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMODF(Context& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMFBO(Context& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMTXF(Context& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMH2O(Context& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);

		static bool ReadMCNK(Context& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMCVT(Context& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMCLY(Context& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMCAL(Context& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMCNR(Context& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);
		static bool ReadMCCV(Context& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Adt::Wdt& wdt, Adt::Layout& adt);

	private:
		static robin_hood::unordered_map<u32, std::function<bool(Context& context, const Parser::ParseType parseType, const FileChunkHeader&, std::shared_ptr<Bytebuffer>&, const Adt::Wdt&, Adt::Layout&)>> _adtFileChunkToFunction;
	};
}
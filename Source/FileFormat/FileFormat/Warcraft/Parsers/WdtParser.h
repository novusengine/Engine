#pragma once
#include <Base/Types.h>

#include <robinhood/robinhood.h>

class Bytebuffer;
struct FileChunkHeader;

namespace ADT
{
	struct WDT;
}

class WdtParser
{
public:
	WdtParser() { }

	bool TryParse(std::shared_ptr<Bytebuffer>& wdtBuffer, ADT::WDT& out);

public:
	static bool ReadMVER(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, ADT::WDT& wdt);
	static bool ReadMPHD(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, ADT::WDT& wdt);
	static bool ReadMAIN(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, ADT::WDT& wdt);
	static bool ReadMAID(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, ADT::WDT& wdt);
	static bool ReadMODF(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, ADT::WDT& wdt);

private:
	bool ParseBufferOrderIndependent(std::shared_ptr<Bytebuffer>& buffer, ADT::WDT& out);
	static robin_hood::unordered_map<u32, std::function<bool(const FileChunkHeader&, std::shared_ptr<Bytebuffer>&, ADT::WDT&)>> wdtFileChunkToFunction;
};
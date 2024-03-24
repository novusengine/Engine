#pragma once
#include <Base/Types.h>

#include <robinhood/robinhood.h>

class Bytebuffer;
struct FileChunkHeader;

namespace Adt
{
    struct Wdt;

    class WdtParser
    {
    public:
        WdtParser() { }

        bool TryParse(std::shared_ptr<Bytebuffer>& wdtBuffer, Adt::Wdt& out);

    public:
        static bool ReadMVER(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Adt::Wdt& wdt);
        static bool ReadMPHD(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Adt::Wdt& wdt);
        static bool ReadMAIN(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Adt::Wdt& wdt);
        static bool ReadMAID(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Adt::Wdt& wdt);
        static bool ReadMODF(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Adt::Wdt& wdt);

    private:
        bool ParseBufferOrderIndependent(std::shared_ptr<Bytebuffer>& buffer, Adt::Wdt& out);
        static robin_hood::unordered_map<u32, std::function<bool(const FileChunkHeader&, std::shared_ptr<Bytebuffer>&, Adt::Wdt&)>> wdtFileChunkToFunction;
    };
}
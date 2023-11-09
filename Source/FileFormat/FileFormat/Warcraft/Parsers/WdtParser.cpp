#include "WdtParser.h"
#include "FileFormat/Shared.h"
#include "FileFormat/Warcraft/ADT/Adt.h"

#include <Base/Util/DebugHandler.h>

using namespace Adt;

robin_hood::unordered_map<u32, std::function<bool(const FileChunkHeader&, std::shared_ptr<Bytebuffer>&, Wdt&)>> WdtParser::wdtFileChunkToFunction =
{
    { *reinterpret_cast<const u32*>("MVER"), WdtParser::ReadMVER },
    { *reinterpret_cast<const u32*>("MPHD"), WdtParser::ReadMPHD },
    { *reinterpret_cast<const u32*>("MAIN"), WdtParser::ReadMAIN },
    { *reinterpret_cast<const u32*>("MAID"), WdtParser::ReadMAID },
    { *reinterpret_cast<const u32*>("MODF"), WdtParser::ReadMODF },

    { *reinterpret_cast<const u32*>("MWMO"), nullptr },
    { *reinterpret_cast<const u32*>("MANM"), nullptr },
    { *reinterpret_cast<const u32*>("MAOI"), nullptr },
    { *reinterpret_cast<const u32*>("MAOH"), nullptr },
    { *reinterpret_cast<const u32*>("MPL2"), nullptr },
    { *reinterpret_cast<const u32*>("MPL3"), nullptr },
    { *reinterpret_cast<const u32*>("MSLT"), nullptr },
    { *reinterpret_cast<const u32*>("MTEX"), nullptr },
    { *reinterpret_cast<const u32*>("MLTA"), nullptr },
    { *reinterpret_cast<const u32*>("VFOG"), nullptr },
    { *reinterpret_cast<const u32*>("PVPD"), nullptr },
    { *reinterpret_cast<const u32*>("PVMI"), nullptr },
    { *reinterpret_cast<const u32*>("PVBD"), nullptr }
};

bool WdtParser::TryParse(std::shared_ptr<Bytebuffer>& wdtBuffer, Wdt& out)
{
    // Parse Wdt Buffer
    if (!ParseBufferOrderIndependent(wdtBuffer, out))
        return false;

    return true;
}

bool WdtParser::ParseBufferOrderIndependent(std::shared_ptr<Bytebuffer>& buffer, Wdt& wdt)
{
    FileChunkHeader header;

    do
    {
        if (!buffer->Get<FileChunkHeader>(header))
            return false;

        auto itr = wdtFileChunkToFunction.find(header.token);
        if (itr == wdtFileChunkToFunction.end())
        {
            const char* bytes = reinterpret_cast<const char*>(&header.token);

            std::string_view sv(bytes, 4);

            DebugHandler::PrintError("[WdtParser : Encountered unexpected Chunk {0}", sv);

            return false;
        }

        if (itr->second)
        {
            if (!itr->second(header, buffer, wdt))
                return false;
        }
        else if (header.size > 0)
        {
            buffer->SkipRead(header.size);
        }

    } while (buffer->GetActiveSize());

    return true;
}
bool WdtParser::ReadMVER(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Wdt& wdt)
{
    if (!buffer->Get(wdt.mver))
        return false;

    return true;
}
bool WdtParser::ReadMPHD(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Wdt& wdt)
{
    if (!buffer->Get(wdt.mphd))
        return false;

    return true;
}

bool WdtParser::ReadMAIN(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Wdt& wdt)
{
    if (!buffer->Get(wdt.main))
        return false;

    return true;
}

bool WdtParser::ReadMAID(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Wdt& wdt)
{
    if (!buffer->Get(wdt.maid))
        return false;

    return true;
}

bool WdtParser::ReadMODF(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Wdt& wdt)
{
    if (!LoadArrayOfStructs(buffer, header.size, wdt.modf.data))
        return false;

    return true;
}

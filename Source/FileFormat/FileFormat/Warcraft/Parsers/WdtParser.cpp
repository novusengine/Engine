#include "WdtParser.h"
#include "FileFormat/Warcraft/Shared.h"
#include "FileFormat/Warcraft/ADT/Adt.h"

#include <Base/Util/DebugHandler.h>

using namespace ADT;

robin_hood::unordered_map<u32, std::function<bool(const FileChunkHeader&, std::shared_ptr<Bytebuffer>&, WDT&)>> WdtParser::wdtFileChunkToFunction =
{
    { static_cast<u32>('MVER'), WdtParser::ReadMVER },
    { static_cast<u32>('MPHD'), WdtParser::ReadMPHD },
    { static_cast<u32>('MAIN'), WdtParser::ReadMAIN },
    { static_cast<u32>('MAID'), WdtParser::ReadMAID },
    { static_cast<u32>('MODF'), WdtParser::ReadMODF },

    { static_cast<u32>('MWMO'), nullptr },
    { static_cast<u32>('MANM'), nullptr },
    { static_cast<u32>('MAOI'), nullptr },
    { static_cast<u32>('MAOH'), nullptr },
    { static_cast<u32>('MPL2'), nullptr },
    { static_cast<u32>('MPL3'), nullptr },
    { static_cast<u32>('MSLT'), nullptr },
    { static_cast<u32>('MTEX'), nullptr },
    { static_cast<u32>('MLTA'), nullptr },
    { static_cast<u32>('VFOG'), nullptr },
    { static_cast<u32>('PVPD'), nullptr },
    { static_cast<u32>('PVMI'), nullptr },
    { static_cast<u32>('PVBD'), nullptr }
};

bool WdtParser::TryParse(std::shared_ptr<Bytebuffer>& wdtBuffer, WDT& out)
{
    // Parse Wdt Buffer
    if (!ParseBufferOrderIndependent(wdtBuffer, out))
        return false;

    return true;
}

bool WdtParser::ParseBufferOrderIndependent(std::shared_ptr<Bytebuffer>& buffer, WDT& wdt)
{
    FileChunkHeader header;

    do
    {
        if (!buffer->Get<FileChunkHeader>(header))
            return false;

        auto itr = wdtFileChunkToFunction.find(header.token);
        if (itr == wdtFileChunkToFunction.end())
        {
            DebugHandler::PrintError("[WdtParser : Encountered unexpected Chunk (%.*s)", 4, reinterpret_cast<char*>(&header.token));
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
bool WdtParser::ReadMVER(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, WDT& wdt)
{
    if (!buffer->Get(wdt.mver))
        return false;

    return true;
}
bool WdtParser::ReadMPHD(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, WDT& wdt)
{
    if (!buffer->Get(wdt.mphd))
        return false;

    return true;
}

bool WdtParser::ReadMAIN(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, WDT& wdt)
{
    if (!buffer->Get(wdt.main))
        return false;

    return true;
}

bool WdtParser::ReadMAID(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, WDT& wdt)
{
    if (!buffer->Get(wdt.maid))
        return false;

    return true;
}

bool WdtParser::ReadMODF(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, WDT& wdt)
{
    if (!LoadArrayOfStructs(buffer, header.size, wdt.modf.data))
        return false;

    return true;
}
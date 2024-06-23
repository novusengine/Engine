#include "WmoParser.h"
#include "FileFormat/Shared.h"
#include "FileFormat/Warcraft/WMO/Wmo.h"

#include <Base/Util/DebugHandler.h>

using namespace Wmo;

robin_hood::unordered_map<u32, std::function<bool(const Parser::ParseType parseType, const FileChunkHeader&, std::shared_ptr<Bytebuffer>&, Layout& layout)>> Parser::_wmoFileChunkToFunction =
{
    // WMO Root Chunks
    { FileChunkToken("MVER"), Parser::ReadMVER },
    { FileChunkToken("MOHD"), Parser::ReadMOHD },
    { FileChunkToken("MOMT"), Parser::ReadMOMT },
    { FileChunkToken("MOUV"), nullptr },
    { FileChunkToken("MOGN"), Parser::ReadMOGN },
    { FileChunkToken("MOGI"), Parser::ReadMOGI },
    { FileChunkToken("MOSI"), Parser::ReadMOSI },
    { FileChunkToken("MOPV"), Parser::ReadMOPV },
    { FileChunkToken("MOPT"), Parser::ReadMOPT },
    { FileChunkToken("MOPR"), Parser::ReadMOPR },
    { FileChunkToken("MOVV"), nullptr },
    { FileChunkToken("MOVB"), nullptr },
    { FileChunkToken("MOLT"), Parser::ReadMOLT },
    { FileChunkToken("MOLV"), nullptr },
    { FileChunkToken("MODS"), Parser::ReadMODS },
    { FileChunkToken("MODI"), Parser::ReadMODI },
    { FileChunkToken("MODD"), Parser::ReadMODD },
    { FileChunkToken("MFOG"), nullptr }, // Read these when we have FOG support.
    { FileChunkToken("MCVP"), nullptr },
    { FileChunkToken("GFID"), Parser::ReadGFID },
    { FileChunkToken("MDDI"), nullptr },
    { FileChunkToken("MPVD"), nullptr },
    { FileChunkToken("MAVG"), nullptr },
    { FileChunkToken("MAVD"), nullptr },
    { FileChunkToken("MBVD"), nullptr },
    { FileChunkToken("MFED"), nullptr },
    { FileChunkToken("MGI2"), nullptr },
    { FileChunkToken("MNLD"), nullptr },
    { FileChunkToken("MDDL"), nullptr },

    // WMO Group Chunks
    { FileChunkToken("MOGP"), Parser::ReadMOGP },
    { FileChunkToken("MOGX"), nullptr },
    { FileChunkToken("MOPY"), Parser::ReadMOPY },
    { FileChunkToken("MPY2"), nullptr },
    { FileChunkToken("MOVI"), Parser::ReadMOVI },
    { FileChunkToken("MOVX"), nullptr },
    { FileChunkToken("MOVT"), Parser::ReadMOVT },
    { FileChunkToken("MONR"), Parser::ReadMONR },
    { FileChunkToken("MOTV"), Parser::ReadMOTV },
    { FileChunkToken("MOBA"), Parser::ReadMOBA },
    { FileChunkToken("MOLR"), nullptr },
    { FileChunkToken("MODR"), nullptr },
    { FileChunkToken("MOBN"), nullptr },
    { FileChunkToken("MOBR"), nullptr },
    { FileChunkToken("MOCV"), Parser::ReadMOCV },
    { FileChunkToken("MLIQ"), nullptr },
    { FileChunkToken("MORI"), nullptr },
    { FileChunkToken("MORB"), nullptr },
    { FileChunkToken("MOTA"), nullptr },
    { FileChunkToken("MOBS"), nullptr },
    { FileChunkToken("MDAL"), nullptr },
    { FileChunkToken("MOPL"), nullptr },
    { FileChunkToken("MOPB"), nullptr },
    { FileChunkToken("MOLS"), nullptr },
    { FileChunkToken("MOLP"), nullptr },
    { FileChunkToken("MLSK"), nullptr },
    { FileChunkToken("MOS2"), nullptr },
    { FileChunkToken("MOP2"), nullptr },
    { FileChunkToken("MPVR"), nullptr },
    { FileChunkToken("MAVR"), nullptr },
    { FileChunkToken("MBVR"), nullptr },
    { FileChunkToken("MFVR"), nullptr },
    { FileChunkToken("MNLR"), nullptr },
    { FileChunkToken("MNLR"), nullptr },

    // Unused
    { FileChunkToken("MOSB"), nullptr },
    { FileChunkToken("MODN"), nullptr },
    { FileChunkToken("MLSS"), nullptr },
    { FileChunkToken("MLSP"), nullptr },
    { FileChunkToken("MLSO"), nullptr },
    { FileChunkToken("MPBV"), nullptr },
    { FileChunkToken("MPBP"), nullptr },
    { FileChunkToken("MPBI"), nullptr },
    { FileChunkToken("MPBG"), nullptr },
};

bool Parser::TryParse(const ParseType parseType, std::shared_ptr<Bytebuffer>& buffer, Layout& out)
{
    if (!buffer)
        return false;

    if (parseType == ParseType::Group)
        out.groups.emplace_back();

    if (!ParseBufferOrderIndependent(parseType, buffer, out))
        return false;

    return true;
}

bool Parser::ParseBufferOrderIndependent(ParseType parseType, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    FileChunkHeader header;

    do
    {
        if (!buffer->Get<FileChunkHeader>(header))
            return false;

        auto itr = _wmoFileChunkToFunction.find(header.token);
        if (itr == _wmoFileChunkToFunction.end())
        {
            const char* bytes = reinterpret_cast<const char*>(&header.token);

            std::string_view sv(bytes, 4);

            NC_LOG_ERROR("WmoParser : Encountered unexpected Chunk {0}", sv);

            return false;
        }

        if (itr->second)
        {
            if (!itr->second(parseType, header, buffer, layout))
                return false;
        }
        else if (header.size > 0)
        {
            buffer->SkipRead(header.size);
        }

    } while (buffer->GetActiveSize());

    return true;
}

bool Parser::ReadMVER(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    switch (parseType)
    {
    case ParseType::Root:
    {
        if (!buffer->Get(layout.mver))
            return false;

        break;
    }
    case ParseType::Group:
    {
        WMOGroup& group = layout.groups.back();

        if (!buffer->Get(group.mver))
            return false;

        break;
    }

    default:
        return false;
    }

    return true;
}
bool Parser::ReadMOHD(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!buffer->Get(layout.mohd))
        return false;

    return true;
}
bool Parser::ReadMOMT(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.momt.data))
        return false;

    return true;
}
bool Parser::ReadMOGN(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    layout.mogn.numBytes = header.size;
    layout.mogn.groupNames = buffer->GetReadPointer();
    buffer->SkipRead(layout.mogn.numBytes);

    return true;
}
bool Parser::ReadMOGI(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.mogi.data))
        return false;

    return true;
}
bool Parser::ReadMOSI(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!buffer->Get(layout.mosi))
        return false;

    return true;
}
bool Parser::ReadMOPV(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.mopv.data))
        return false;

    return true;
}
bool Parser::ReadMOPT(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.mopt.data))
        return false;

    return true;
}
bool Parser::ReadMOPR(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.mopr.data))
        return false;

    return true;
}
bool Parser::ReadMOLT(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.molt.data))
        return false;

    return true;
}
bool Parser::ReadMODS(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.mods.data))
        return false;

    return true;
}
bool Parser::ReadMODI(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.modi.data))
        return false;

    return true;
}
bool Parser::ReadMODD(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.modd.data))
        return false;

    return true;
}
bool Parser::ReadGFID(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.gfid.data))
        return false;

    return true;
}

bool Parser::ReadMOGP(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    WMOGroup& group = layout.groups.back();

    if (!buffer->Get(group.mogp))
        return false;

    return true;
}
bool Parser::ReadMOPY(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    WMOGroup& group = layout.groups.back();

    if (!LoadArrayOfStructs(buffer, header.size, group.mopy.data))
        return false;

    return true;
}
bool Parser::ReadMOVI(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    WMOGroup& group = layout.groups.back();

    if (!LoadArrayOfStructs(buffer, header.size, group.movi.data))
        return false;

    return true;
}
bool Parser::ReadMOVT(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    WMOGroup& group = layout.groups.back();

    if (!LoadArrayOfStructs(buffer, header.size, group.movt.data))
        return false;

    return true;
}
bool Parser::ReadMONR(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    WMOGroup& group = layout.groups.back();

    if (!LoadArrayOfStructs(buffer, header.size, group.monr.data))
        return false;

    return true;
}
bool Parser::ReadMOTV(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    WMOGroup& group = layout.groups.back();

    MOTV& motv = group.motvs.emplace_back();
    if (!LoadArrayOfStructs(buffer, header.size, motv.data))
        return false;

    return true;
}
bool Parser::ReadMOBA(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    WMOGroup& group = layout.groups.back();

    if (!LoadArrayOfStructs(buffer, header.size, group.moba.data))
        return false;

    return true;
}
bool Parser::ReadMOCV(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    WMOGroup& group = layout.groups.back();

    MOCV& mocv = group.mocvs.emplace_back();
    if (!LoadArrayOfStructs(buffer, header.size, mocv.data))
        return false;

    return true;
}

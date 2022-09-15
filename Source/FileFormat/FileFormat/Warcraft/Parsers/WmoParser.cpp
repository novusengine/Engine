#include "WmoParser.h"
#include "FileFormat/Warcraft/Shared.h"
#include "FileFormat/Warcraft/WMO/Wmo.h"

#include <Base/Util/DebugHandler.h>

using namespace WMO;

robin_hood::unordered_map<u32, std::function<bool(const WmoParser::ParseType parseType, const FileChunkHeader&, std::shared_ptr<Bytebuffer>&, Layout& layout)>> WmoParser::_wmoFileChunkToFunction =
{
    // WMO Root Chunks
    { FileChunkToken("MVER"), WmoParser::ReadMVER },
    { FileChunkToken("MOHD"), WmoParser::ReadMOHD },
    { FileChunkToken("MOMT"), WmoParser::ReadMOMT },
    { FileChunkToken("MOUV"), nullptr },
    { FileChunkToken("MOGN"), WmoParser::ReadMOGN },
    { FileChunkToken("MOGI"), WmoParser::ReadMOGI },
    { FileChunkToken("MOSI"), WmoParser::ReadMOSI },
    { FileChunkToken("MOPV"), WmoParser::ReadMOPV },
    { FileChunkToken("MOPT"), WmoParser::ReadMOPT },
    { FileChunkToken("MOPR"), WmoParser::ReadMOPR },
    { FileChunkToken("MOVV"), nullptr },
    { FileChunkToken("MOVB"), nullptr },
    { FileChunkToken("MOLT"), WmoParser::ReadMOLT },
    { FileChunkToken("MOLV"), nullptr },
    { FileChunkToken("MODS"), WmoParser::ReadMODS },
    { FileChunkToken("MODI"), WmoParser::ReadMODI },
    { FileChunkToken("MODD"), WmoParser::ReadMODD },
    { FileChunkToken("MFOG"), nullptr }, // Read these when we have FOG support.
    { FileChunkToken("MCVP"), nullptr },
    { FileChunkToken("GFID"), WmoParser::ReadGFID },
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
    { FileChunkToken("MOGP"), WmoParser::ReadMOGP },
    { FileChunkToken("MOPY"), WmoParser::ReadMOPY },
    { FileChunkToken("MOVI"), WmoParser::ReadMOVI },
    { FileChunkToken("MOVX"), nullptr },
    { FileChunkToken("MOVT"), WmoParser::ReadMOVT },
    { FileChunkToken("MONR"), WmoParser::ReadMONR },
    { FileChunkToken("MOTV"), WmoParser::ReadMOTV },
    { FileChunkToken("MOBA"), WmoParser::ReadMOBA },
    { FileChunkToken("MOLR"), nullptr },
    { FileChunkToken("MODR"), nullptr },
    { FileChunkToken("MOBN"), nullptr },
    { FileChunkToken("MOBR"), nullptr },
    { FileChunkToken("MOCV"), WmoParser::ReadMOCV },
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

bool WmoParser::TryParse(const ParseType parseType, std::shared_ptr<Bytebuffer>& buffer, Layout& out)
{
    if (!buffer)
        return false;

    if (parseType == ParseType::Group)
        out.groups.emplace_back();

    if (!ParseBufferOrderIndependent(parseType, buffer, out))
        return false;

    return true;
}

bool WmoParser::ParseBufferOrderIndependent(ParseType parseType, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    FileChunkHeader header;

    do
    {
        if (!buffer->Get<FileChunkHeader>(header))
            return false;

        auto itr = _wmoFileChunkToFunction.find(header.token);
        if (itr == _wmoFileChunkToFunction.end())
        {
            DebugHandler::PrintError("[WmoParser : Encountered unexpected Chunk (%.*s)", 4, reinterpret_cast<char*>(&header.token));
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

bool WmoParser::ReadMVER(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
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
bool WmoParser::ReadMOHD(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!buffer->Get(layout.mohd))
        return false;

    return true;
}
bool WmoParser::ReadMOMT(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.momt.data))
        return false;

    return true;
}
bool WmoParser::ReadMOGN(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    layout.mogn.numBytes = header.size;
    layout.mogn.groupNames = buffer->GetReadPointer();
    buffer->SkipRead(layout.mogn.numBytes);

    return true;
}
bool WmoParser::ReadMOGI(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.mogi.data))
        return false;

    return true;
}
bool WmoParser::ReadMOSI(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!buffer->Get(layout.mosi))
        return false;

    return true;
}
bool WmoParser::ReadMOPV(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.mopv.data))
        return false;

    return true;
}
bool WmoParser::ReadMOPT(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.mopt.data))
        return false;

    return true;
}
bool WmoParser::ReadMOPR(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.mopr.data))
        return false;

    return true;
}
bool WmoParser::ReadMOLT(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.molt.data))
        return false;

    return true;
}
bool WmoParser::ReadMODS(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.mods.data))
        return false;

    return true;
}
bool WmoParser::ReadMODI(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.modi.data))
        return false;

    return true;
}
bool WmoParser::ReadMODD(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.modd.data))
        return false;

    return true;
}
bool WmoParser::ReadGFID(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.gfid.data))
        return false;

    return true;
}

bool WmoParser::ReadMOGP(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    WMOGroup& group = layout.groups.back();

    if (!buffer->Get(group.mogp))
        return false;

    return true;
}
bool WmoParser::ReadMOPY(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    WMOGroup& group = layout.groups.back();

    if (!LoadArrayOfStructs(buffer, header.size, group.mopy.data))
        return false;

    return true;
}
bool WmoParser::ReadMOVI(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    WMOGroup& group = layout.groups.back();

    if (!LoadArrayOfStructs(buffer, header.size, group.movi.data))
        return false;

    return true;
}
bool WmoParser::ReadMOVT(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    WMOGroup& group = layout.groups.back();

    if (!LoadArrayOfStructs(buffer, header.size, group.movt.data))
        return false;

    return true;
}
bool WmoParser::ReadMONR(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    WMOGroup& group = layout.groups.back();

    if (!LoadArrayOfStructs(buffer, header.size, group.monr.data))
        return false;

    return true;
}
bool WmoParser::ReadMOTV(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    WMOGroup& group = layout.groups.back();

    MOTV& motv = group.motvs.emplace_back();
    if (!LoadArrayOfStructs(buffer, header.size, motv.data))
        return false;

    return true;
}
bool WmoParser::ReadMOBA(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    WMOGroup& group = layout.groups.back();

    if (!LoadArrayOfStructs(buffer, header.size, group.moba.data))
        return false;

    return true;
}
bool WmoParser::ReadMOCV(const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    WMOGroup& group = layout.groups.back();

    MOCV& mocv = group.mocvs.emplace_back();
    if (!LoadArrayOfStructs(buffer, header.size, mocv.data))
        return false;

    return true;
}

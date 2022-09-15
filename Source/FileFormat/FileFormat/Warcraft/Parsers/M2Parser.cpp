#include "M2Parser.h"
#include "FileFormat/Warcraft/Shared.h"
#include "FileFormat/Warcraft/M2/M2.h"

#include <Base/Util/DebugHandler.h>

using namespace M2;

robin_hood::unordered_map<u32, std::function<bool(const FileChunkHeader&, std::shared_ptr<Bytebuffer>&, Layout&)>> M2Parser::_m2FileChunkToFunction =
{
    { FileChunkToken("MD21", true), M2Parser::ReadMD21},
    { FileChunkToken("SFID", true), M2Parser::ReadSFID},
    { FileChunkToken("AFID", true), M2Parser::ReadAFID},
    { FileChunkToken("BFID", true), M2Parser::ReadBFID},
    { FileChunkToken("TXID", true), M2Parser::ReadTXID},

    { FileChunkToken("PFID", true), nullptr},
    { FileChunkToken("TXAC", true), nullptr},
    { FileChunkToken("EXPT", true), nullptr},
    { FileChunkToken("EXP2", true), nullptr},
    { FileChunkToken("PABC", true), nullptr},
    { FileChunkToken("PADC", true), nullptr},
    { FileChunkToken("PSBC", true), nullptr},
    { FileChunkToken("PEDC", true), nullptr},
    { FileChunkToken("SKID", true), nullptr},
    { FileChunkToken("LDV1", true), nullptr},
    { FileChunkToken("RPID", true), nullptr},
    { FileChunkToken("GPID", true), nullptr},
    { FileChunkToken("WFV1", true), nullptr},
    { FileChunkToken("WFV2", true), nullptr},
    { FileChunkToken("PGD1", true), nullptr},
    { FileChunkToken("WFV3", true), nullptr},
    { FileChunkToken("PFDC", true), nullptr},
    { FileChunkToken("EDGF", true), nullptr},
    { FileChunkToken("NERF", true), nullptr},
    { FileChunkToken("DETL", true), nullptr},
    { FileChunkToken("DBOC", true), nullptr}
};

bool M2Parser::TryParse(const ParseType parseType, std::shared_ptr<Bytebuffer>& buffer, Layout& out)
{
    if (!buffer)
        return false;

    if (parseType == ParseType::Root)
    {
        if (!ParseBufferOrderIndependent(buffer, out))
            return false;
    }
    else if (parseType == ParseType::Skin)
    {
        if (!ParseSkinBuffer(buffer, out))
            return false;
    }
    else
    {
        return false;
    }

    return true;
}

bool M2Parser::ParseBufferOrderIndependent(std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    FileChunkHeader header;

    do
    {
        if (!buffer->Get<FileChunkHeader>(header))
            return false;

        auto itr = _m2FileChunkToFunction.find(header.token);
        if (itr == _m2FileChunkToFunction.end())
        {
            // There appears to be M2s with just zeros written.
            if (header.token == 0 && header.size == 0 && buffer->readData == sizeof(FileChunkHeader))
                return false;

            DebugHandler::PrintError("[M2Parser : Encountered unexpected Chunk (%.*s)", 4, reinterpret_cast<char*>(&header.token));
            return false;
        }

        if (itr->second)
        {
            if (!itr->second(header, buffer, layout))
                return false;
        }
        else if (header.size > 0)
        {
            buffer->SkipRead(header.size);
        }

    } while (buffer->GetActiveSize());

    return true;
}

bool M2Parser::ParseSkinBuffer(std::shared_ptr<Bytebuffer>& buffer, Layout& out)
{
    if (!buffer->Get(out.skin))
        return false;

    return true;
}

bool M2Parser::ReadMD21(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    u32 md21Offset = static_cast<u32>(buffer->readData);
    u32 md21Size = sizeof(MD21) - sizeof(layout.md21.textureCombinerCombos);

    // MD21 contains an optional M2Array at the end, so we read all bytes up until there, and then read it afterwards if needed.
    if (!buffer->GetBytes(reinterpret_cast<u8*>(&layout.md21), md21Size))
        return false;

    if (layout.md21.flags.UseTextureCombinerCombos)
    {
        if (!buffer->Get(layout.md21.textureCombinerCombos))
            return false;

        md21Size += sizeof(layout.md21.textureCombinerCombos);
    }

    // Make offsets absolute
    {
        layout.md21.uniqueName.offset += md21Offset * (layout.md21.uniqueName.offset > 0);
        layout.md21.loopingAnimationTimestamps.offset += md21Offset * (layout.md21.loopingAnimationTimestamps.offset > 0);
        layout.md21.sequences.offset += md21Offset * (layout.md21.sequences.offset > 0);
        layout.md21.sequenceIDToAnimationID.offset += md21Offset * (layout.md21.sequenceIDToAnimationID.offset > 0);
        layout.md21.bones.offset += md21Offset * (layout.md21.bones.offset > 0);
        layout.md21.keyBoneToBoneIndexList.offset += md21Offset * (layout.md21.keyBoneToBoneIndexList.offset > 0);
        layout.md21.vertices.offset += md21Offset * (layout.md21.vertices.offset > 0);
        layout.md21.colors.offset += md21Offset * (layout.md21.colors.offset > 0);
        layout.md21.textures.offset += md21Offset * (layout.md21.textures.offset > 0);
        layout.md21.textureWeights.offset += md21Offset * (layout.md21.textureWeights.offset > 0);
        layout.md21.textureTransforms.offset += md21Offset * (layout.md21.textureTransforms.offset > 0);
        layout.md21.textureSwapTable.offset += md21Offset * (layout.md21.textureSwapTable.offset > 0);
        layout.md21.materials.offset += md21Offset * (layout.md21.materials.offset > 0);
        layout.md21.boneCombinationList.offset += md21Offset * (layout.md21.boneCombinationList.offset > 0);
        layout.md21.textureCombinationList.offset += md21Offset * (layout.md21.textureCombinationList.offset > 0);
        layout.md21.textureUnitLookupList.offset += md21Offset * (layout.md21.textureUnitLookupList.offset > 0);
        layout.md21.textureTransparencyLookupList.offset += md21Offset * (layout.md21.textureTransparencyLookupList.offset > 0);
        layout.md21.textureUVAnimationLookupList.offset += md21Offset * (layout.md21.textureUVAnimationLookupList.offset > 0);
        layout.md21.collisionIndices.offset += md21Offset * (layout.md21.collisionIndices.offset > 0);
        layout.md21.collisionVertices.offset += md21Offset * (layout.md21.collisionVertices.offset > 0);
        layout.md21.collisionNormals.offset += md21Offset * (layout.md21.collisionNormals.offset > 0);
        layout.md21.attachments.offset += md21Offset * (layout.md21.attachments.offset > 0);
        layout.md21.attachmentLooukpList.offset += md21Offset * (layout.md21.attachmentLooukpList.offset > 0);
        layout.md21.events.offset += md21Offset * (layout.md21.events.offset > 0);
        layout.md21.lights.offset += md21Offset * (layout.md21.lights.offset > 0);
        layout.md21.cameras.offset += md21Offset * (layout.md21.cameras.offset > 0);
        layout.md21.cameraLookupList.offset += md21Offset * (layout.md21.cameraLookupList.offset > 0);
        layout.md21.ribbonEmitters.offset += md21Offset * (layout.md21.ribbonEmitters.offset > 0);
        layout.md21.particleEmitters.offset += md21Offset * (layout.md21.particleEmitters.offset > 0);
        layout.md21.textureCombinerCombos.offset += md21Offset * (layout.md21.textureCombinerCombos.offset > 0);
    }

    size_t bytesToSkip = header.size - md21Size;
    buffer->SkipRead(bytesToSkip);

    return true;
}
bool M2Parser::ReadSFID(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    u32 bytesToRead = layout.md21.numSkinProfiles * sizeof(u32);
    if (bytesToRead)
    {
        layout.sfid.skinFileIDs.resize(layout.md21.numSkinProfiles);
        if (!buffer->GetBytes(reinterpret_cast<u8*>(layout.sfid.skinFileIDs.data()), bytesToRead))
            return false;
    }

    u32 numLodBytesToRead = header.size - bytesToRead;
    if (numLodBytesToRead)
    {
        u32 numLods = numLodBytesToRead / sizeof(u32);

        layout.sfid.skinLodFileIDs.resize(numLods);
        if (!buffer->GetBytes(reinterpret_cast<u8*>(layout.sfid.skinLodFileIDs.data()), numLodBytesToRead))
            return false;
    }

    return true;
}
bool M2Parser::ReadAFID(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.afid.data))
        return false;

    return true;
}
bool M2Parser::ReadBFID(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.bfid.boneFileIDs))
        return false;

    return true;
}
bool M2Parser::ReadTXID(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.txid.textureFileIDs))
        return false;

    return true;
}
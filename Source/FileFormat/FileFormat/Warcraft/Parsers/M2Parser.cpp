#include "M2Parser.h"
#include "FileFormat/Shared.h"
#include "FileFormat/Warcraft/M2/M2.h"

#include <Base/Util/DebugHandler.h>

using namespace M2;

robin_hood::unordered_map<u32, std::function<bool(const FileChunkHeader&, std::shared_ptr<Bytebuffer>&, Layout&)>> Parser::_m2FileChunkToFunction =
{
    { FileChunkToken("MD21", true), Parser::ReadMD21},
    { FileChunkToken("SFID", true), Parser::ReadSFID},
    { FileChunkToken("AFID", true), Parser::ReadAFID},
    { FileChunkToken("BFID", true), Parser::ReadBFID},
    { FileChunkToken("TXID", true), Parser::ReadTXID},

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

bool Parser::TryParse(const ParseType parseType, std::shared_ptr<Bytebuffer>& buffer, Layout& out)
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

bool Parser::ParseBufferOrderIndependent(std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
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

            const char* bytes = reinterpret_cast<const char*>(&header.token);

            std::string_view sv(bytes, 4);

            NC_LOG_ERROR("M2Parser : Encountered unexpected Chunk {0}", sv);

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

bool Parser::ParseSkinBuffer(std::shared_ptr<Bytebuffer>& buffer, Layout& out)
{
    if (!buffer->Get(out.skin))
        return false;

    return true;
}

template <typename T>
static void InitAnimationArray(std::shared_ptr<Bytebuffer>& buffer, M2Track<T>& track, u32 md21Offset)
{
    track.timestamps.Init(md21Offset);
    track.values.Init(md21Offset);

    for (u32 j = 0; j < track.timestamps.size; j++)
    {
        M2Array<u32>* timestamps = track.timestamps.GetElement(buffer, j);
        timestamps->Init(md21Offset);
    }

    for (u32 j = 0; j < track.values.size; j++)
    {
        M2Array<T>* values = track.values.GetElement(buffer, j);
        values->Init(md21Offset);
    }
}

bool Parser::ReadMD21(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
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
        layout.md21.uniqueName.Init(md21Offset);
        layout.md21.globalLoops.Init(md21Offset);
        layout.md21.sequences.Init(md21Offset);
        layout.md21.sequenceIDToAnimationID.Init(md21Offset);
        layout.md21.bones.Init(md21Offset);
        layout.md21.keyBoneToBoneIndexList.Init(md21Offset);
        layout.md21.vertices.Init(md21Offset);
        layout.md21.colors.Init(md21Offset);
        layout.md21.textures.Init(md21Offset);
        layout.md21.textureWeights.Init(md21Offset);
        layout.md21.textureTransforms.Init(md21Offset);
        layout.md21.textureSwapTable.Init(md21Offset);
        layout.md21.materials.Init(md21Offset);
        layout.md21.boneCombinationList.Init(md21Offset);
        layout.md21.textureCombinationList.Init(md21Offset);
        layout.md21.textureUnitLookupList.Init(md21Offset);
        layout.md21.textureTransparencyLookupList.Init(md21Offset);
        layout.md21.textureUVAnimationLookupList.Init(md21Offset);
        layout.md21.collisionIndices.Init(md21Offset);
        layout.md21.collisionVertices.Init(md21Offset);
        layout.md21.collisionNormals.Init(md21Offset);
        layout.md21.attachments.Init(md21Offset);
        layout.md21.attachmentLooukpList.Init(md21Offset);
        layout.md21.events.Init(md21Offset);
        layout.md21.lights.Init(md21Offset);
        layout.md21.cameras.Init(md21Offset);
        layout.md21.cameraLookupList.Init(md21Offset);
        layout.md21.ribbonEmitters.Init(md21Offset);
        layout.md21.particleEmitters.Init(md21Offset);
        layout.md21.textureCombinerCombos.Init(md21Offset);

        for (u32 i = 0; i < layout.md21.bones.size; i++)
        {
            M2CompBone* bone = layout.md21.bones.GetElement(buffer, i);

            InitAnimationArray(buffer, bone->translation, md21Offset);
            InitAnimationArray(buffer, bone->rotation, md21Offset);
            InitAnimationArray(buffer, bone->scale, md21Offset);
        }

        for (u32 i = 0; i < layout.md21.textureTransforms.size; i++)
        {
            M2TextureTransform* textureTransform = layout.md21.textureTransforms.GetElement(buffer, i);

            InitAnimationArray(buffer, textureTransform->translation, md21Offset);
            InitAnimationArray(buffer, textureTransform->rotation, md21Offset);
            InitAnimationArray(buffer, textureTransform->scale, md21Offset);
        }

        for (u32 i = 0; i < layout.md21.cameras.size; i++)
        {
            M2Camera* camera = layout.md21.cameras.GetElement(buffer, i);

            InitAnimationArray(buffer, camera->positions, md21Offset);
            InitAnimationArray(buffer, camera->targetPosition, md21Offset);
            InitAnimationArray(buffer, camera->roll, md21Offset);
            InitAnimationArray(buffer, camera->fov, md21Offset);
        }
    }

    size_t bytesToSkip = header.size - md21Size;
    buffer->SkipRead(bytesToSkip);

    return true;
}

bool Parser::ReadSFID(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
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
bool Parser::ReadAFID(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.afid.data))
        return false;

    return true;
}
bool Parser::ReadBFID(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.bfid.boneFileIDs))
        return false;

    return true;
}
bool Parser::ReadTXID(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.txid.textureFileIDs))
        return false;

    return true;
}
bool Parser::ReadTVFS(const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.txid.textureFileIDs))
        return false;

    return true;
}

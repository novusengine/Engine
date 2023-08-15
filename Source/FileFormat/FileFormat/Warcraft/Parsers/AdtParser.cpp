#include "AdtParser.h"
#include "FileFormat/Shared.h"
#include "FileFormat/Warcraft/ADT/Adt.h"

#include <Base/Util/DebugHandler.h>

using namespace Adt;

robin_hood::unordered_map<u32, std::function<bool(Parser::Context& context, const Parser::ParseType parseType, const FileChunkHeader&, std::shared_ptr<Bytebuffer>&, const Wdt&, Layout&)>> Parser::_adtFileChunkToFunction =
{
    { FileChunkToken("MVER"), Parser::ReadMVER },
    { FileChunkToken("MHDR"), Parser::ReadMHDR },
    { FileChunkToken("MDID"), Parser::ReadMDID },
    { FileChunkToken("MHID"), Parser::ReadMHID },
    { FileChunkToken("MDDF"), Parser::ReadMDDF },
    { FileChunkToken("MODF"), Parser::ReadMODF },
    { FileChunkToken("MFBO"), Parser::ReadMFBO },
    { FileChunkToken("MTXF"), Parser::ReadMTXF },
    { FileChunkToken("MAMP"), nullptr             },
    { FileChunkToken("MH2O"), Parser::ReadMH2O },
    { FileChunkToken("MTXP"), nullptr             },
    { FileChunkToken("MCBB"), nullptr             },
    { FileChunkToken("MBMH"), nullptr             },
    { FileChunkToken("MBMI"), nullptr             },
    { FileChunkToken("MBNV"), nullptr             },
    { FileChunkToken("MBBB"), nullptr             },
    { FileChunkToken("MCMT"), nullptr             },
    { FileChunkToken("MWDR"), nullptr             },
    { FileChunkToken("MWDS"), nullptr             },
    { FileChunkToken("MLMB"), nullptr             },
    { FileChunkToken("MTCG"), nullptr             },

    // MCNK and it's sub chunks
    { FileChunkToken("MCNK"), Parser::ReadMCNK },
    { FileChunkToken("MCVT"), Parser::ReadMCVT },
    { FileChunkToken("MCLV"), nullptr },
    { FileChunkToken("MCCV"), Parser::ReadMCCV },
    { FileChunkToken("MCLY"), Parser::ReadMCLY },
    { FileChunkToken("MCAL"), Parser::ReadMCAL },
    { FileChunkToken("MCSH"), nullptr },
    { FileChunkToken("MCNR"), Parser::ReadMCNR },
    { FileChunkToken("MCRD"), nullptr },
    { FileChunkToken("MCRW"), nullptr },
    { FileChunkToken("MCLQ"), nullptr },
    { FileChunkToken("MCSE"), nullptr },

    // Old chunks that no longer exist (Could possibly be in the files as trash data)
    { FileChunkToken("MCIN"), nullptr },
    { FileChunkToken("MTEX"), nullptr },
    { FileChunkToken("MMDX"), nullptr },
    { FileChunkToken("MMID"), nullptr },
    { FileChunkToken("MWMO"), nullptr },
    { FileChunkToken("MWID"), nullptr }
};

bool Parser::TryParse(Context& context, std::shared_ptr<Bytebuffer>& rootBuffer, std::shared_ptr<Bytebuffer>& textureBuffer, std::shared_ptr<Bytebuffer>& objectBuffer, const Wdt& wdt, Layout& out)
{
    if (!rootBuffer)
        return false;

    // Parse Root Buffer
    out.cellInfos.resize(Terrain::CHUNK_NUM_CELLS);

    if (!ParseBufferOrderIndependent(context, ParseType::Root, rootBuffer, wdt, out))
        return false;

    // Parse Texture Buffer
    if (textureBuffer)
    {
        context.currentMCNKIndex = -1;
        if (!ParseBufferOrderIndependent(context, ParseType::Texture, textureBuffer, wdt, out))
            return false;
    }

    // Parse Object Buffer
    if (objectBuffer)
    {
        context.currentMCNKIndex = -1;
        if (!ParseBufferOrderIndependent(context, ParseType::Object, objectBuffer, wdt, out))
            return false;
    }

    return true;
}

bool Parser::ParseBufferOrderIndependent(Context& context, ParseType parseType, std::shared_ptr<Bytebuffer>& buffer, const Wdt& wdt, Layout& layout)
{
    FileChunkHeader header;

    do
    {
        if (!buffer->Get<FileChunkHeader>(header))
            return false;

        auto itr = _adtFileChunkToFunction.find(header.token);
        if (itr == _adtFileChunkToFunction.end())
        {
            DebugHandler::PrintError("[Parser : Encountered unexpected Chunk (%.*s)", 4, reinterpret_cast<char*>(&header.token));
            return false;
        }

        if (itr->second)
        {
            if (!itr->second(context, parseType, header, buffer, wdt, layout))
                return false;
        }
        else if (header.size > 0)
        {
            buffer->SkipRead(header.size);
        }

    } while (buffer->GetActiveSize());

    return true;
}

bool Parser::ReadMVER(Context& context, const Parser::ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Wdt& wdt, Layout& layout)
{
    if (!buffer->Get(layout.mver))
        return false;

    return true;
}
bool Parser::ReadMHDR(Context& context, const Parser::ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Wdt& wdt, Layout& layout)
{
    u32 mhdrOffset = static_cast<u32>(buffer->readData);

    if (!buffer->Get(layout.mhdr))
        return false;

    // Convert offsets from A(Relative to mhdrOffset) to B(Absolute)
    {
        layout.mhdr.offsets.mfbo += mhdrOffset * (layout.mhdr.offsets.mfbo != 0);
        layout.mhdr.offsets.mh2o += mhdrOffset * (layout.mhdr.offsets.mh2o != 0);
    }

    return true;
}
bool Parser::ReadMDID(Context& context, const Parser::ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Wdt& wdt, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.mdid.data))
        return false;

    return true;
}
bool Parser::ReadMHID(Context& context, const Parser::ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Wdt& wdt, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.mhid.data))
        return false;

    return true;
}
bool Parser::ReadMDDF(Context& context, const Parser::ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Wdt& wdt, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.mddf.data))
        return false;

    return true;
}
bool Parser::ReadMODF(Context& context, const Parser::ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Wdt& wdt, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.modf.data))
        return false;

    return true;
}
bool Parser::ReadMFBO(Context& context, const Parser::ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Wdt& wdt, Layout& layout)
{
    if (!buffer->Get(layout.mfbo))
        return false;

    return true;
}
bool Parser::ReadMTXF(Context& context, const Parser::ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Wdt& wdt, Layout& layout)
{
    if (!LoadArrayOfStructs(buffer, header.size, layout.mtxf.data))
        return false;

    return true;
}
bool Parser::ReadMH2O(Context& context, const Parser::ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Wdt& wdt, Layout& layout)
{
    size_t bufferStartRead = buffer->readData;

    std::vector<u8> dataBytes(header.size);
    std::memcpy(dataBytes.data(), buffer->GetReadPointer(), header.size);

    constexpr u32 LIQUID_HEADER_SIZE = (16 * 16) * sizeof(MH2O::LiquidHeader);
    if (!LoadArrayOfStructs(buffer, LIQUID_HEADER_SIZE, layout.mh2o.headers))
        return false;

    u32 numHeaders = static_cast<u32>(layout.mh2o.headers.size());
    u32 numInstances = 0;
    u32 numAttributes = 0;

    u32 firstInstanceOffset = std::numeric_limits<u32>().max();

    // Prepass [Gather data]
    {
        for (u32 i = 0; i < numHeaders; i++)
        {
            const MH2O::LiquidHeader& liquidHeader = layout.mh2o.headers[i];

            if (liquidHeader.layerCount == 0)
                continue;

            if (liquidHeader.instanceOffset < firstInstanceOffset)
                firstInstanceOffset = liquidHeader.instanceOffset;

            if (liquidHeader.attributeOffset > 0)
                numAttributes++;

            numInstances += liquidHeader.layerCount;
        }

        layout.mh2o.instances.resize(numInstances);
        std::memcpy(layout.mh2o.instances.data(), &dataBytes[firstInstanceOffset], numInstances * sizeof(MH2O::LiquidInstance));
    }

    // Process [Read & Convert]
    {
        assert(numHeaders > 0);
        assert(numInstances > 0);

        u32 instanceIndex = 0;
        u32 attributeIndex = 0;

        layout.mh2o.attributes.resize(numAttributes);

        for (u32 i = 0; i < numHeaders; i++)
        {
            const MH2O::LiquidHeader& liquidHeader = layout.mh2o.headers[i];

            bool hasAttributes = liquidHeader.attributeOffset > 0;
            if (hasAttributes)
            {
                const MH2O::LiquidAttribute* liquidAttributePtr = reinterpret_cast<MH2O::LiquidAttribute*>(&dataBytes[liquidHeader.attributeOffset]);
                assert(liquidAttributePtr != nullptr);

                MH2O::LiquidAttribute& liquidAttribute = layout.mh2o.attributes[attributeIndex++];
                liquidAttribute = *liquidAttributePtr;
            }

            if (liquidHeader.layerCount == 0)
                continue;

            for (u32 j = 0; j < liquidHeader.layerCount; j++)
            {
                const MH2O::LiquidInstance& liquidInstance = layout.mh2o.instances[instanceIndex + j];

                u16 liquidVertexFormat = liquidInstance.liquidVertexFormat;
                if (liquidInstance.liquidType == 2)
                    liquidVertexFormat = 2;

                if (liquidVertexFormat >= 42)
                {
                    i16 liquidTypeID = -1;
                    i32 materialID = -1;

                    if (context.liquidObjects->HasEntry(liquidVertexFormat))
                    {
                        liquidTypeID = context.liquidObjects->GetEntry(liquidVertexFormat).liquidTypeID;
                    }
                    else
                    {
                        liquidTypeID = liquidInstance.liquidType;
                    }

                    if (context.liquidTypes->HasEntry(liquidTypeID))
                    {
                        materialID = context.liquidTypes->GetEntry(liquidTypeID).materialID;

                        if (context.liquidMaterials->HasEntry(materialID))
                        {
                            liquidVertexFormat = context.liquidMaterials->GetEntry(materialID).liquidVertexFormat;
                        }
                    }
                }
                assert(liquidVertexFormat >= 0 && liquidVertexFormat <= 3);

                bool hasBitmapData = liquidInstance.bitmapDataOffset > 0;
                bool hasVertexData = liquidInstance.vertexDataOffset > 0 && liquidVertexFormat != 2 && liquidVertexFormat != 42;

                if (hasBitmapData)
                {
                    u32 bitmapDataBeforeAdd = static_cast<u32>(layout.mh2o.bitmapData.size());
                    u32 bitmapDataToAdd = (liquidInstance.width * liquidInstance.height + 7) / 8;
                    assert(bitmapDataToAdd > 0);

                    layout.mh2o.bitmapData.resize(bitmapDataBeforeAdd + bitmapDataToAdd);
                    memcpy(&layout.mh2o.bitmapData[bitmapDataBeforeAdd], &dataBytes[liquidInstance.bitmapDataOffset], bitmapDataToAdd);
                }

                if (hasVertexData)
                {
                    u32 numVertices = (liquidInstance.width + 1) * (liquidInstance.height + 1);
                    u32 formatSize = ((liquidVertexFormat == 0) * sizeof(MH2O::LiquidVertexFormat_Height)) + ((liquidVertexFormat == 1 || liquidVertexFormat == 3) * sizeof(MH2O::LiquidVertexFormat_Height_UV));
                    assert(numVertices > 0 && formatSize > 0);

                    u32 vertexDataBeforeAdd = static_cast<u32>(layout.mh2o.vertexData.size());
                    u32 vertexDataToAdd = numVertices * formatSize;

                    layout.mh2o.vertexData.resize(vertexDataBeforeAdd + vertexDataToAdd);
                    memcpy(&layout.mh2o.vertexData[vertexDataBeforeAdd], &dataBytes[liquidInstance.vertexDataOffset], vertexDataToAdd);
                }
            }

            instanceIndex += liquidHeader.layerCount;
        }
    }

    size_t bufferEndRead = buffer->readData;
    size_t bufferBytesToSkip = header.size - (bufferEndRead - bufferStartRead);

    buffer->SkipRead(bufferBytesToSkip);

    return true;
}
bool Parser::ReadMCNK(Context& context, const Parser::ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Wdt& wdt, Layout& layout)
{
    context.currentMCNKIndex++;

    if (parseType == Parser::ParseType::Texture ||
        parseType == Parser::ParseType::Object)
        return true;

    CellInfo& cellInfo = layout.cellInfos[context.currentMCNKIndex];

    if (!buffer->Get(cellInfo.mcnk))
        return false;

    return true;
}
bool Parser::ReadMCVT(Context& context, const Parser::ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Wdt& wdt, Layout& layout)
{
    CellInfo& cellInfo = layout.cellInfos[context.currentMCNKIndex];

    if (!buffer->Get(cellInfo.mcvt))
        return false;

    return true;
}
bool Parser::ReadMCLY(Context& context, const Parser::ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Wdt& wdt, Layout& layout)
{
    CellInfo& cellInfo = layout.cellInfos[context.currentMCNKIndex];

    if (!LoadArrayOfStructs(buffer, header.size, cellInfo.mcly.data))
        return false;

    return true;
}
bool Parser::ReadMCAL(Context& context, const Parser::ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Wdt& wdt, Layout& layout)
{
    CellInfo& cellInfo = layout.cellInfos[context.currentMCNKIndex];

    size_t numLayers = cellInfo.mcly.data.size();
    if (numLayers <= 1)
        return true;

    bool usingBigAlpha = wdt.mphd.flags.UseBigAlpha || wdt.mphd.flags.UseHeightTexturing;

    for (u32 i = 1; i < numLayers; i++)
    {
        const MCLY::LayerDefinition& layerDefinition = cellInfo.mcly.data[i];
        MCAL::AlphaMap& mcal = cellInfo.mcal.data.emplace_back();

        if (usingBigAlpha)
        {
            if (layerDefinition.flags.CompressedAlphaMap)
            {
                u8* alphaMap = &buffer->GetReadPointer()[layerDefinition.offsetInMCAL];
                u32 offsetIn = 0;
                u32 offsetOut = 0;

                while (offsetOut < 4096 && offsetIn < 4096)
                {
                    bool fill = alphaMap[offsetIn] & 0x80;
                    u8 count = alphaMap[offsetIn++] & 0x7F;

                    for (u8 j = 0; j < count; j++)
                    {
                        // According to Wowdev.wiki some compressed alphamaps are corrupt, and can generate more than
                        // 4096 bytes of data, and therefore we must manually check for it here.
                        if (offsetOut == 4096 || offsetIn == 4096)
                            break;

                        mcal.alphaMap[offsetOut++] = alphaMap[offsetIn];

                        // Copy Mode, in this case we can to contiously copy the next sequence of bytes
                        if (!fill)
                            offsetIn += 1;
                    }

                    // Fill Mode, in this case we want to copy the first byte into next "count" of bytes
                    if (fill)
                        offsetIn += 1;
                }
            }
            else
            {
                if (!buffer->GetBytes(&mcal.alphaMap[0], 4096, buffer->readData + layerDefinition.offsetInMCAL))
                    return false;
            }
        }
        else
        {
            u8 alphaMap[2048];
            if (!buffer->GetBytes(&alphaMap[0], 2048, buffer->readData + layerDefinition.offsetInMCAL))
            {
                assert(false);
                return false;
            }

            u16 offsetOut = 0;
            for (u16 j = 0; j < 2048; j++)
            {
                u8 alpha = alphaMap[j];

                f32 firstVal = static_cast<f32>(alpha & 0xF);
                f32 secondVal = static_cast<f32>(alpha >> 4);

                firstVal = (firstVal / 15.0f) * 255.0f;
                secondVal = (secondVal / 15.0f) * 255.0f;

                mcal.alphaMap[offsetOut++] = static_cast<u8>(firstVal);
                mcal.alphaMap[offsetOut++] = static_cast<u8>(secondVal);
            }

            // Check if we have to fix the alpha map
            if (!cellInfo.mcnk.flags.DoNotFixAlphaMap)
            {
                // Fix last column for every row but the last
                for (u16 j = 0; j < 4032; j += 64)
                {
                    u8 byte = mcal.alphaMap[j + 62];
                    mcal.alphaMap[j + 63] = byte;
                }

                // Fix last row
                for (u16 j = 4032; j < 4096; j++)
                {
                    u8 byte = mcal.alphaMap[j - 64];
                    mcal.alphaMap[j] = byte;
                }
            }
        }
    }

    buffer->SkipRead(header.size);
    return true;
}
bool Parser::ReadMCNR(Context& context, const Parser::ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Wdt& wdt, Layout& layout)
{
    CellInfo& cellInfo = layout.cellInfos[context.currentMCNKIndex];

    size_t startBufferIndex = buffer->readData;
    if (!buffer->Get(cellInfo.mcnr))
        return false;

    size_t deltaBufferIndex = header.size - (buffer->readData - startBufferIndex);
    if (deltaBufferIndex > 0)
        buffer->SkipRead(deltaBufferIndex);

    return true;
}
bool Parser::ReadMCCV(Context& context, const ParseType parseType, const FileChunkHeader& header, std::shared_ptr<Bytebuffer>& buffer, const Wdt& wdt, Layout& layout)
{
    CellInfo& cellInfo = layout.cellInfos[context.currentMCNKIndex];

    if (!buffer->Get(cellInfo.mccv))
        return false;

    return true;
}
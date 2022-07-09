#include "Renderer.h"
#include "RenderGraph.h"
#include "RenderSettings.h"

#include <Base/Memory/Allocator.h>

namespace Renderer
{
    Renderer::~Renderer()
    {
    }

    RenderGraph& Renderer::CreateRenderGraph(RenderGraphDesc& desc)
    {
        RenderGraph* renderGraph = Memory::Allocator::New<RenderGraph>(desc.allocator, desc.allocator, this);
        renderGraph->Init(desc);

        return *renderGraph;
    }

    BufferID Renderer::CreateBuffer(BufferID bufferID, BufferDesc& desc)
    {
        if (bufferID != BufferID::Invalid())
        {
            QueueDestroyBuffer(bufferID);
        }

        return CreateBuffer(desc);
    }

    BufferID Renderer::CreateAndFillBuffer(BufferID bufferID, BufferDesc desc, void* data, size_t dataSize)
    {
        if (bufferID != BufferID::Invalid())
        {
            QueueDestroyBuffer(bufferID);
        }

        return CreateAndFillBuffer(desc, data, dataSize);
    }

    BufferID Renderer::CreateAndFillBuffer(BufferDesc desc, void* data, size_t dataSize)
    {
        // Create actual buffer
        desc.usage |= BufferUsage::TRANSFER_DESTINATION; // If we're supposed to stage into it, we have to make sure it's a transfer destination
        BufferID bufferID = CreateBuffer(desc);

        if (dataSize > 0)
        {
            // If the size is big enough to upload in one go
            if (dataSize < Settings::STAGING_BUFFER_SIZE)
            {
                auto uploadBuffer = CreateUploadBuffer(bufferID, 0, dataSize);
                memcpy(uploadBuffer->mappedMemory, data, dataSize);
            }
            else // Else if we need to chunk it up
            {
                size_t dataUploaded = 0;
                while (dataUploaded < dataSize)
                {
                    size_t remainingData = dataSize - dataUploaded;
                    size_t dataChunkSize = Math::Min(remainingData, Settings::STAGING_BUFFER_SIZE);

                    auto uploadBuffer = CreateUploadBuffer(bufferID, dataUploaded, dataChunkSize);

                    memcpy(uploadBuffer->mappedMemory, &static_cast<u8*>(data)[dataUploaded], dataChunkSize);

                    dataUploaded += dataChunkSize;
                }
            }
        }

        return bufferID;
    }

    BufferID Renderer::CreateAndFillBuffer(BufferID bufferID, BufferDesc desc, const std::function<void(void*, size_t)>& callback)
    {
        if (bufferID != BufferID::Invalid())
        {
            QueueDestroyBuffer(bufferID);
        }
        
        return CreateAndFillBuffer(desc, callback);
    }

    BufferID Renderer::CreateAndFillBuffer(BufferDesc desc, const std::function<void(void*, size_t)>& callback)
    {
        // Create actual buffer
        desc.usage |= BufferUsage::TRANSFER_DESTINATION; // If we're supposed to stage into it, we have to make sure it's a transfer destination
        BufferID bufferID = CreateBuffer(desc);

        u64 descSize = Math::Max(desc.size, 1ul);

        auto uploadBuffer = CreateUploadBuffer(bufferID, 0, descSize);
        callback(uploadBuffer->mappedMemory, descSize);

        return bufferID;
    }

    void Renderer::UploadToBuffer(BufferID dstBuffer, u64 dstOffset, void* srcData, u64 srcOffset, u64 srcSize)
    {
        // If the size is big enough to upload in one go
        if (srcSize < Settings::STAGING_BUFFER_SIZE)
        {
            auto uploadBuffer = CreateUploadBuffer(dstBuffer, dstOffset, srcSize);
            memcpy(uploadBuffer->mappedMemory, &static_cast<u8*>(srcData)[srcOffset], srcSize);
        }
        else // Else if we need to chunk it up
        {
            size_t dataUploaded = 0;
            while (dataUploaded < srcSize)
            {
                size_t remainingData = srcSize - dataUploaded;
                size_t dataChunkSize = Math::Min(remainingData, Settings::STAGING_BUFFER_SIZE);

                auto uploadBuffer = CreateUploadBuffer(dstBuffer, dstOffset + dataUploaded, dataChunkSize);

                memcpy(uploadBuffer->mappedMemory, &static_cast<u8*>(srcData)[srcOffset + dataUploaded], dataChunkSize);

                dataUploaded += dataChunkSize;
            }
        }
    }
}
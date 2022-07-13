#include "ModelUtils.h"
#include "Model.h"

#include "FileFormat/ChunkHeader.h"
#include "FileFormat/ChunkPointer.h"

#include <Base/Memory/Bytebuffer.h>

namespace ModelUtils
{
	size_t GetModelSerializedSize(const Model& model)
	{
		size_t size = sizeof(Model::Header);

		// 1x ChunkPointer per mesh.
		size += model.meshes.size() * sizeof(ChunkPointer);

		// Vertices
		size += model.vertices.size() * sizeof(Model::Vertex);

		// Indices
		size += model.indices.size() * sizeof(u32);

		// Meshes
		{
			for (u32 i = 0; i < model.meshes.size(); i++)
			{
				const Model::Mesh& mesh = model.meshes[i];

				size += mesh.meshlets.size() * sizeof(Model::Meshlet);
			}
		}

		return size;
	}

	bool Serialize(const Model& model, std::shared_ptr<Bytebuffer>& buffer)
	{
		size_t bufferStartWrittenData = buffer->writtenData;

		if (!buffer->Put<ChunkHeader>(model.header))
			return false;

		ChunkHeader* chunkHeader = reinterpret_cast<ChunkHeader*>(&buffer->GetDataPointer()[bufferStartWrittenData]);
		ChunkPointer emptyChunkPointer;
		size_t bufferChunkPointersOffset = buffer->writtenData;

		{
			// Vertices
			if (!buffer->Put<ChunkPointer>(emptyChunkPointer))
				return false;

			// Indices
			if (!buffer->Put<ChunkPointer>(emptyChunkPointer))
				return false;

			// Meshes
			if (!buffer->Put<ChunkPointer>(emptyChunkPointer))
				return false;

			for (u32 i = 0; i < model.meshes.size(); i++)
			{
				// Meshlets
				if (!buffer->Put<ChunkPointer>(emptyChunkPointer))
					return false;
			}
		}

		ChunkPointer* chunkPointer = reinterpret_cast<ChunkPointer*>(&buffer->GetDataPointer()[bufferChunkPointersOffset]);

		// Write Vertices
		{
			chunkPointer->bufferOffset = buffer->writtenData;
			chunkPointer->numElements = model.vertices.size();

			if (!buffer->PutBytes(model.vertices.data(), chunkPointer->numElements * sizeof(Model::Vertex)))
				return false;

			chunkPointer++;
		}

		// Write Indices
		{
			chunkPointer->bufferOffset = buffer->writtenData;
			chunkPointer->numElements = model.indices.size();

			if (!buffer->PutBytes(model.indices.data(), chunkPointer->numElements * sizeof(u32)))
				return false;

			chunkPointer++;
		}

		// Write Meshes
		{
			chunkPointer->bufferOffset = buffer->writtenData;
			chunkPointer->numElements = model.meshes.size();

			for (u32 i = 0; i < model.meshes.size(); i++)
			{
				const Model::Mesh& mesh = model.meshes[i];

				chunkPointer++;
				chunkPointer->bufferOffset = buffer->writtenData;
				chunkPointer->numElements = mesh.meshlets.size();

				if (!buffer->PutBytes(mesh.meshlets.data(), chunkPointer->numElements * sizeof(Model::Meshlet)))
					return false;
			}
		}

		// Size of this chunk is equal to the total bytes written minus the header itself
		chunkHeader->size = (buffer->writtenData - bufferStartWrittenData) - sizeof(ChunkHeader);

		return true;
	}
}
#pragma once
#include "FileFormat/ChunkHeader.h"
#include "FileFormat/ChunkPointer.h"

#include <Base/Types.h>
#include <Base/Platform.h>

#include <vector>

class Bytebuffer;

PRAGMA_NO_PADDING_START;
struct Model
{
public:
	struct Meshlet
	{
	public:
		u32 indexStart = 0;
		u16 indexCount = 0;
	};

	struct Mesh
	{
	public:
		std::vector<Meshlet> meshlets;
	};

	struct Vertex
	{
	public:
		vec3 position;
		vec3 normal;
		vec2 uv;
	};

	struct Header
	{
	public:
		ChunkHeader chunkHeader;

		ChunkPointer vertices;
		ChunkPointer indices;
		ChunkPointer meshes;
	};

public:
	Model();

public:
	ChunkHeader header;

	std::vector<Vertex> vertices;
	std::vector<u32> indices;
	std::vector<Mesh> meshes;
};
PRAGMA_NO_PADDING_END;
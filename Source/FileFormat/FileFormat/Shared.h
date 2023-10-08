#pragma once
#include <Base/Types.h>
#include <Base/Memory/Bytebuffer.h>

namespace Terrain
{
    constexpr u32 TEXTURE_ID_INVALID = std::numeric_limits<u32>().max();

    /*
        - Map     is made up of chunks
        - Chunks are made up of cells
        - Cells  are made up of patches    */
    constexpr u16 CHUNK_INVALID_ID                  = std::numeric_limits<u16>().max();
    constexpr f32 CHUNK_SIZE                        = 533.3333f;
    constexpr f32 CHUNK_HALF_SIZE                   = CHUNK_SIZE / 2.0f;
    constexpr u32 CHUNK_NUM_PER_MAP_STRIDE          = 64;
    constexpr u32 CHUNK_NUM_PER_MAP                 = CHUNK_NUM_PER_MAP_STRIDE * CHUNK_NUM_PER_MAP_STRIDE;
    constexpr u32 CHUNK_NUM_CELLS_PER_STRIDE        = 16;
    constexpr u32 CHUNK_NUM_CELLS                   = CHUNK_NUM_CELLS_PER_STRIDE * CHUNK_NUM_CELLS_PER_STRIDE;
    constexpr u32 CHUNK_ALPHAMAP_CELL_RESOLUTION    = 64 * 64;
    constexpr u32 CHUNK_ALPHAMAP_CELL_NUM_CHANNELS  = 4;
    constexpr u32 CHUNK_ALPHAMAP_TOTAL_BYTE_SIZE    = CHUNK_NUM_CELLS * CHUNK_ALPHAMAP_CELL_RESOLUTION * CHUNK_ALPHAMAP_CELL_NUM_CHANNELS;

    constexpr f32 CELL_SIZE                     = CHUNK_SIZE / CHUNK_NUM_CELLS_PER_STRIDE;
    constexpr f32 CELL_HALF_SIZE                = CELL_SIZE / 2.0f;
    constexpr u16 CELL_INNER_GRID_STRIDE        = 8;
    constexpr u16 CELL_INNER_GRID_SIZE          = CELL_INNER_GRID_STRIDE * CELL_INNER_GRID_STRIDE;
    constexpr u16 CELL_OUTER_GRID_STRIDE        = 9;
    constexpr u16 CELL_OUTER_GRID_SIZE          = CELL_OUTER_GRID_STRIDE * CELL_OUTER_GRID_STRIDE;
    constexpr u16 CELL_GRID_ROW_SIZE            = CELL_INNER_GRID_STRIDE + CELL_OUTER_GRID_STRIDE;
    constexpr u16 CELL_TOTAL_GRID_SIZE          = CELL_INNER_GRID_SIZE + CELL_OUTER_GRID_SIZE;
    constexpr u32 CELL_NUM_PATCHES_PER_STRIDE   = 8;
    constexpr u32 CELL_NUM_PATCHES              = CELL_NUM_PATCHES_PER_STRIDE * CELL_NUM_PATCHES_PER_STRIDE;
    constexpr u32 CELL_NUM_INDICES              = 768;
    constexpr u32 CELL_NUM_TRIANGLES            = CELL_NUM_INDICES / 3;

    constexpr f32 PATCH_SIZE                    = CELL_SIZE / CELL_NUM_PATCHES_PER_STRIDE;
    constexpr f32 PATCH_HALF_SIZE               = PATCH_SIZE / 2.0f;

    constexpr f32 MAP_SIZE                      = CHUNK_SIZE * CHUNK_NUM_PER_MAP_STRIDE; // yards
    constexpr f32 MAP_HALF_SIZE                 = MAP_SIZE / 2.0f; // yards

    struct Placement
    {
    public:
        u32 uniqueID = std::numeric_limits<u32>().max();
        u32 nameHash = std::numeric_limits<u32>().max();
        u16 doodadSet = std::numeric_limits<u16>().max();
        vec3 position = vec3(0.0f, 0.0f, 0.0f);
        quat rotation = quat(1, 0, 0, 0);
        u16 scale = 0;
    };
}
#pragma pack(push, 1)
struct FileChunkHeader
{
    u32 token;
    u32 size;
};

struct IntColor
{
    union
    {
        u32 bgra = 0;
        struct {
            u8 b;
            u8 g;
            u8 r;
            u8 a;
        };
    };
};

template <typename T>
struct SplineKey
{
    T value = { };
    T tanIn = { };
    T tanOut = { };
};

struct FileChunkToken
{
public:
    constexpr FileChunkToken(const char* name, bool reversed = false) noexcept : _value(0)
    {
        const u8 bytes[4] = { static_cast<u8>(name[0]), static_cast<u8>(name[1]), static_cast<u8>(name[2]), static_cast<u8>(name[3]) };

        if (reversed)
        {
            _value = static_cast<u32>(bytes[0]) | (static_cast<u32>(bytes[1]) << 8) | (static_cast<u32>(bytes[2]) << 16) | (static_cast<u32>(bytes[3]) << 24);
        }
        else
        {
            _value = static_cast<u32>(bytes[3]) | (static_cast<u32>(bytes[2]) << 8) | (static_cast<u32>(bytes[1]) << 16) | (static_cast<u32>(bytes[0]) << 24);
        }
    }
    FileChunkToken(const FileChunkToken& other) = default;

    constexpr operator u32() noexcept { return _value; }

private:
    u32 _value;
};

struct MVER // MVER provides us with the version of this file.
{
public:
    u32 version = 0;
};
#pragma pack(pop)

template <typename T>
inline bool LoadArrayOfStructs(std::shared_ptr<Bytebuffer>& buffer, u32 dataSize, std::vector<T>& container)
{
    if (dataSize == 0)
        return true;

    size_t elementSize = sizeof(T);
    size_t elementsNum = dataSize / elementSize;

    if (elementsNum == 0)
        return false;

    container.resize(elementsNum);
    if (!buffer->GetBytes(reinterpret_cast<u8*>(&container[0]), elementsNum * elementSize))
        return false;

    return true;
}

template <typename T>
inline bool ReadVectorFromBuffer(std::shared_ptr<Bytebuffer>& buffer, std::vector<T>& container)
{
    u32 numElements = 0;
    if (!buffer->GetU32(numElements))
        return false;

    if (numElements)
    {
        container.resize(numElements);
        if (!buffer->GetBytes(reinterpret_cast<u8*>(&container[0]), numElements * sizeof(T)))
            return false;
    }

    return true;
}

inline vec2 OctNormalWrap(vec2 v)
{
    vec2 wrap;
    wrap.x = (1.0f - glm::abs(v.y)) * (v.x >= 0.0f ? 1.0f : -1.0f);
    wrap.y = (1.0f - glm::abs(v.x)) * (v.y >= 0.0f ? 1.0f : -1.0f);
    return wrap;
}

inline vec2 OctNormalEncode(vec3 n)
{
    n /= (glm::abs(n.x) + glm::abs(n.y) + glm::abs(n.z));

    vec2 wrapped = OctNormalWrap(n);

    vec2 result;
    result.x = n.z >= 0.0f ? n.x : wrapped.x;
    result.y = n.z >= 0.0f ? n.y : wrapped.y;

    result.x = result.x * 0.5f + 0.5f;
    result.y = result.y * 0.5f + 0.5f;

    return result;
}

inline vec3 OctNormalDecode(vec2 encN)
{
    encN = encN * 2.0f - 1.0f;

    // https://twitter.com/Stubbesaurus/status/937994790553227264
    vec3 n = vec3(encN.x, encN.y, 1.0f - abs(encN.x) - abs(encN.y));
    float t = glm::clamp(-n.z, 0.0f, 1.0f);

    n.x += n.x >= 0.0f ? -t : t;
    n.y += n.y >= 0.0f ? -t : t;

    return normalize(n);
}

namespace CoordinateSpaces
{
    // Novus Coordinate space matches what Unity has
    // LEFT HANDED
    // X = East
    // Y = Up
    // Z = North

    // Placement Coordinate space
    // RIGHT HANDED
    // X = West
    // Y = Up
    // Z = North
    inline vec3 PlacementPosToNovus(const vec3& pos)
    {
        return vec3(-pos.x, pos.y, pos.z);
    }

    inline vec3 PlacementRotToNovus(const vec3& rot)
    {
        // When going from Right Handed to Left Handed we need to negate the rotations
        return vec3(-rot.x, -rot.y , rot.z);
    }

    inline quat PlacementRotToNovus(const quat& rot)
    {
        vec3 euler = glm::eulerAngles(rot);
        euler = PlacementRotToNovus(euler);

        return quat(euler);
    }

    // Model Coordinate space (M2 and WMO)
    // RIGHT HANDED
    // X = South
    // Y = East
    // Z = UP
    inline vec3 ModelPosToNovus(const vec3& pos)
    {
        return vec3(pos.y, pos.z, -pos.x);
    }

    // Model Coordinate space (M2 and WMO)
    // RIGHT HANDED
    // X = South
    // Y = East
    // Z = UP
    inline vec3 DecorationRotToNovus(const vec3& pos)
    {
        return vec3(-pos.y,- pos.z, pos.x);
    }

    // Cinematic Camera Coordinate Space
    // RIGHT HANDED
    // X = North
    // Y = West
    // Z = UP
    inline vec3 CinematicCameraPosToNovus(const vec3& pos)
    {
        return vec3(-pos.y, pos.z, pos.x);
    }

    inline glm::quat ModelRotToNovus(const glm::quat& input)
    {
        // Convert input quaternion to its equivalent rotation matrix
        glm::mat3 rotMat = glm::mat3(input);

        // Apply the same transformation as for position and scale
        glm::mat3 transformation(0, 0, -1,
            1, 0, 0,
            0, 1, 0);

        glm::mat3 convertedMat = transformation * rotMat * glm::transpose(transformation);
        // Convert the transformed matrix back to a quaternion
        glm::quat converted = glm::quat(convertedMat);
        return converted;
    }
}


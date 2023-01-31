#pragma once
#include <Base/Types.h>

namespace Geometry
{
    struct Triangle
    {
    public:
        vec3 vert1;
        vec3 vert2;
        vec3 vert3;

        const vec3& GetVert(u32 index) const;

        vec3 GetNormal() const;
        f32 GetSteepnessAngle() const;

        vec3 GetCollisionNormal() const;
        f32 GetCollisionSteepnessAngle() const;
    };

    struct AABoundingBox
    {
        vec3 center;
        vec3 extents;
    };

    struct OrientedBoundingBox
    {
        vec3 center;
        vec3 rotation;
        vec3 extents;
    };

#pragma pack(push, 1)
    struct Box
    {
        Box()
            : top(0)
            , right(0)
            , bottom(0)
            , left(0)
        {

        }

        Box(u32 inTop, u32 inRight, u32 inBottom, u32 inLeft)
            : top(inTop)
            , right(inRight)
            , bottom(inBottom)
            , left(inLeft)
        {

        }

        u32 top = 0;
        u32 right = 0;
        u32 bottom = 0;
        u32 left = 0;
    };
    struct FBox
    {
        FBox()
            : top(0.0f)
            , right(0.0f)
            , bottom(0.0f)
            , left(0.0f)
        {

        }

        FBox(f32 inTop, f32 inRight, f32 inBottom, f32 inLeft)
            : top(inTop)
            , right(inRight)
            , bottom(inBottom)
            , left(inLeft)
        {

        }

        f32 top = 0.0f;
        f32 right = 0.0f;
        f32 bottom = 0.0f;
        f32 left = 0.0f;
    };
    struct HBox
    {
        HBox()
            : top(f16(0.0f))
            , right(f16(0.0f))
            , bottom(f16(0.0f))
            , left(f16(0.0f))
        {

        }

        HBox(f16 inTop, f16 inRight, f16 inBottom, f16 inLeft)
            : top(inTop)
            , right(inRight)
            , bottom(inBottom)
            , left(inLeft)
        {

        }

        f16 top = f16(0.0f);
        f16 right = f16(0.0f);
        f16 bottom = f16(0.0f);
        f16 left = f16(0.0f);
    };
#pragma pack(pop)
}
#pragma once
#include <Base/Types.h>

namespace ClientDB::Definitions
{
    struct Base
    {
    public:
        u32 id;
    };

    struct Map : public Base
    {
    public:
        u32 name;
        u32 internalName;
        u32 instanceType;
        u32 flags;
        u32 expansion;
        u32 maxPlayers;
    };

    struct LiquidObject : public Base
    {
    public:
        u16 liquidTypeID;
        f32 flowDirection;
        f32 flowSpeed;
        u8 fishable;
        u8 reflection;
    };

    struct LiquidType : public Base
    {
    public:
        u32 name;
        u32 textures[6];
        u16 flags;
        u8 soundBank;
        u32 soundID;
        u32 spellID;
        f32 maxDarkenDepth;
        f32 fogDarkenIntensity;
        f32 ambDarkenIntensity;
        f32 dirDarkenIntensity;
        u16 lightID;
        f32 particleScale;
        u8 particleMovement;
        u8 particleTexSlots;
        u8 materialID;
        u32 minimapStaticCol;
        u32 frameCountTextures[6];
        u32 colors[2];
        f32 unkFloats[16];
        u32 unkInts[4];
        u32 coefficients[4];
    };

    struct LiquidMaterial : public Base
    {
    public:
        u8 flags;
        u8 liquidVertexFormat;
    };

    struct CinematicCamera : public Base
    {
    public:
        vec3 endPosition;
        u32 soundID;
        f32 rotation;
        u32 cameraPath;
    };

    struct CinematicSequence : public Base
    {
    public:
        u32 soundID;
        u32 cameraIDs[8];
    };

    // Custom
    struct CameraSave : public Base
    {
    public:
        u32 name;
        u32 code;
    };
    struct Cursor : public Base
    {
    public:
        u32 name;
        u32 texturePath;
    };
}
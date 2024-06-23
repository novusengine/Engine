#pragma once
#include "ClientDB.h"

#include <Base/Types.h>
#include <Base/Container/StringTable.h>
#include <Base/Memory/Bytebuffer.h>

#include <limits>

namespace ClientDB::Definitions
{
    struct Base
    {
    public:
        virtual bool Read(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) = 0;

        virtual bool Write(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) = 0;
        virtual bool WriteStringTable(Novus::Container::StringTableUnsafe& stringTable) = 0;

    public:
        u32 id = std::numeric_limits<u32>().max();
    };

    struct Empty : public Base
    {
    public:
        u32 dummy;

        bool Read(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) override
        {
            return true;
        }
        bool Write(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) override
        {
            return true;
        }
        bool WriteStringTable(Novus::Container::StringTableUnsafe& stringTable) override
        {
            return true;
        }
    };

    struct Map : public Base
    {
    public:
        std::string name;
        std::string internalName;
        u32 instanceType;
        u32 flags;
        u32 expansion;
        u32 maxPlayers;

    public:
        bool Read(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) override
        {
            if (!buffer->GetU32(id))
                return false;

            u32 nameIndex = 0;
            u32 internalNameIndex = 0;

            if (!buffer->GetU32(nameIndex))
                return false;

            if (!buffer->GetU32(internalNameIndex))
                return false;

            name = stringTable.GetString(nameIndex);
            internalName = stringTable.GetString(internalNameIndex);

            if (!buffer->GetU32(instanceType))
                return false;

            if (!buffer->GetU32(flags))
                return false;

            if (!buffer->GetU32(expansion))
                return false;

            if (!buffer->GetU32(maxPlayers))
                return false;

            return true;
        }
        bool Write(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) override
        {
            if (!buffer->PutU32(id))
                return false;

            u32 nameIndex = std::numeric_limits<u32>().max();
            if (!stringTable.TryFindString(name, nameIndex))
                return false;
            
            if (!buffer->PutU32(nameIndex))
                return false;

            u32 internalNameIndex = std::numeric_limits<u32>().max();
            if (!stringTable.TryFindString(internalName, internalNameIndex))
                return false;

            if (!buffer->PutU32(internalNameIndex))
                return false;

            if (!buffer->PutU32(instanceType))
                return false;

            if (!buffer->PutU32(flags))
                return false;

            if (!buffer->PutU32(expansion))
                return false;

            if (!buffer->PutU32(maxPlayers))
                return false;

            return true;
        }

        bool WriteStringTable(Novus::Container::StringTableUnsafe& stringTable) override
        {
            stringTable.AddString(name);
            stringTable.AddString(internalName);

            return true;
        }
    };

    struct LiquidObject : public Base
    {
    public:
        u16 liquidTypeID;
        f32 flowDirection;
        f32 flowSpeed;
        u8 fishable;
        u8 reflection;

    public:
        bool Read(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) override
        {
            if (!buffer->GetU32(id))
                return false;

            if (!buffer->GetU16(liquidTypeID))
                return false;

            if (!buffer->GetF32(flowDirection))
                return false;

            if (!buffer->GetF32(flowSpeed))
                return false;

            if (!buffer->GetU8(fishable))
                return false;

            if (!buffer->GetU8(reflection))
                return false;

            return true;
        }
        bool Write(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) override
        {
            if (!buffer->PutU32(id))
                return false;

            if (!buffer->PutU16(liquidTypeID))
                return false;

            if (!buffer->PutF32(flowDirection))
                return false;

            if (!buffer->PutF32(flowSpeed))
                return false;

            if (!buffer->PutU8(fishable))
                return false;

            if (!buffer->PutU8(reflection))
                return false;

            return true;
        }
        bool WriteStringTable(Novus::Container::StringTableUnsafe& stringTable) override
        {
            return true;
        }
    };

    struct LiquidType : public Base
    {
    public:
        std::string name;
        std::string textures[6];
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

        bool Read(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) override
        {
            if (!buffer->GetU32(id))
                return false;

            u32 nameIndex = 0;
            if (!buffer->GetU32(nameIndex))
                return false;

            name = stringTable.GetString(nameIndex);

            for (u32 i = 0; i < 6; i++)
            {
                u32 textureIndex = 0;
                if (!buffer->GetU32(textureIndex))
                    return false;

                textures[i] = stringTable.GetString(textureIndex);
            }

            if (!buffer->GetU16(flags))
                return false;

            if (!buffer->GetU8(soundBank))
                return false;

            if (!buffer->GetU32(soundID))
                return false;

            if (!buffer->GetF32(maxDarkenDepth))
                return false;

            if (!buffer->GetF32(fogDarkenIntensity))
                return false;

            if (!buffer->GetF32(ambDarkenIntensity))
                return false;

            if (!buffer->GetF32(dirDarkenIntensity))
                return false;

            if (!buffer->GetU16(lightID))
                return false;

            if (!buffer->GetF32(particleScale))
                return false;

            if (!buffer->GetU8(particleMovement))
                return false;

            if (!buffer->GetU8(particleTexSlots))
                return false;

            if (!buffer->GetU8(materialID))
                return false;

            if (!buffer->GetU32(minimapStaticCol))
                return false;

            for (u32 i = 0; i < 6; i++)
            {
                if (!buffer->GetU32(frameCountTextures[i]))
                    return false;
            }

            for (u32 i = 0; i < 2; i++)
            {
                if (!buffer->GetU32(colors[i]))
                    return false;
            }

            for (u32 i = 0; i < 16; i++)
            {
                if (!buffer->GetF32(unkFloats[i]))
                    return false;
            }

            for (u32 i = 0; i < 4; i++)
            {
                if (!buffer->GetU32(unkInts[i]))
                    return false;
            }

            for (u32 i = 0; i < 4; i++)
            {
                if (!buffer->GetU32(coefficients[i]))
                    return false;
            }

            return true;
        }

        bool Write(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) override
        {
            if (!buffer->PutU32(id))
                return false;

            u32 nameIndex = std::numeric_limits<u32>().max();
            if (!stringTable.TryFindString(name, nameIndex))
                return false;
            
            if (!buffer->PutU32(nameIndex))
                return false;

            for (u32 i = 0; i < 6; i++)
            {
                u32 textureIndex = std::numeric_limits<u32>().max();
                if (!stringTable.TryFindString(textures[i], textureIndex))
                    return false;
                
                if (!buffer->PutU32(textureIndex))
                    return false;
            }

            if (!buffer->PutU16(flags))
                return false;

            if (!buffer->PutU8(soundBank))
                return false;

            if (!buffer->PutU32(soundID))
                return false;

            if (!buffer->PutF32(maxDarkenDepth))
                return false;

            if (!buffer->PutF32(fogDarkenIntensity))
                return false;

            if (!buffer->PutF32(ambDarkenIntensity))
                return false;

            if (!buffer->PutF32(dirDarkenIntensity))
                return false;

            if (!buffer->PutU16(lightID))
                return false;

            if (!buffer->PutF32(particleScale))
                return false;

            if (!buffer->PutU8(particleMovement))
                return false;

            if (!buffer->PutU8(particleTexSlots))
                return false;

            if (!buffer->PutU8(materialID))
                return false;

            if (!buffer->PutU32(minimapStaticCol))
                return false;

            for (u32 i = 0; i < 6; i++)
            {
                if (!buffer->PutU32(frameCountTextures[i]))
                    return false;
            }

            for (u32 i = 0; i < 2; i++)
            {
                if (!buffer->PutU32(colors[i]))
                    return false;
            }

            for (u32 i = 0; i < 16; i++)
            {
                if (!buffer->PutF32(unkFloats[i]))
                    return false;
            }

            for (u32 i = 0; i < 4; i++)
            {
                if (!buffer->PutU32(unkInts[i]))
                    return false;
            }

            for (u32 i = 0; i < 4; i++)
            {
                if (!buffer->PutU32(coefficients[i]))
                    return false;
            }

            return true;
        }

        bool WriteStringTable(Novus::Container::StringTableUnsafe& stringTable) override
        {
            stringTable.AddString(name);

            for (u32 i = 0; i < 6; i++)
            {
                stringTable.AddString(textures[i]);
            }

            return true;
        }
    };

    struct LiquidMaterial : public Base
    {
    public:
        u8 flags;
        u8 liquidVertexFormat;

    public:
        bool Read(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) override
        {
            if (!buffer->GetU32(id))
                return false;

            if (!buffer->GetU8(flags))
                return false;

            if (!buffer->GetU8(liquidVertexFormat))
                return false;

            return true;
        }
        bool Write(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) override
        {
            if (!buffer->PutU32(id))
                return false;

            if (!buffer->PutU8(flags))
                return false;

            if (!buffer->PutU8(liquidVertexFormat))
                return false;

            return true;
        }
        bool WriteStringTable(Novus::Container::StringTableUnsafe& stringTable) override
        {
            return true;
        }
    };

    struct CinematicCamera : public Base
    {
    public:
        vec3 endPosition;
        u32 soundID;
        f32 rotation;
        u32 cameraPath;

    public:
        bool Read(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) override
        {
            if (!buffer->GetU32(id))
                return false;

            if (!buffer->Get(endPosition))
                return false;

            if (!buffer->GetU32(soundID))
                return false;

            if (!buffer->GetU32(cameraPath))
                return false;

            return true;
        }

        bool Write(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) override
        {
            if (!buffer->PutU32(id))
                return false;

            if (!buffer->Put(endPosition))
                return false;

            if (!buffer->PutU32(soundID))
                return false;

            if (!buffer->PutU32(cameraPath))
                return false;

            return true;
        }

        bool WriteStringTable(Novus::Container::StringTableUnsafe& stringTable) override
        {
            return true;
        }
    };

    struct CinematicSequence : public Base
    {
    public:
        u32 soundID;
        u32 cameraIDs[8];

    public:
        bool Read(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) override
        {
            if (!buffer->GetU32(id))
                return false;

            if (!buffer->GetU32(soundID))
                return false;

            for (u32 i = 0; i < 8; i++)
            {
                if (!buffer->GetU32(cameraIDs[i]))
                    return false;
            }

            return true;
        }
        bool Write(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) override
        {
            if (!buffer->PutU32(id))
                return false;

            if (!buffer->PutU32(soundID))
                return false;

            for (u32 i = 0; i < 8; i++)
            {
                if (!buffer->PutU32(cameraIDs[i]))
                    return false;
            }

            return true;
        }
        bool WriteStringTable(Novus::Container::StringTableUnsafe& stringTable) override
        {
            return true;
        }
    };

    // Custom
    struct CameraSave : public Base
    {
    public:
        std::string name;
        std::string code;

    public:
        bool Read(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) override
        {
            if (!buffer->GetU32(id))
                return false;

            u32 nameIndex = 0;
            u32 codeIndex = 0;

            if (!buffer->GetU32(nameIndex))
                return false;

            if (!buffer->GetU32(codeIndex))
                return false;

            name = stringTable.GetString(nameIndex);
            code = stringTable.GetString(codeIndex);

            return true;
        }

        bool Write(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) override
        {
            if (!buffer->PutU32(id))
                return false;

            u32 nameIndex = std::numeric_limits<u32>().max();
            if (!stringTable.TryFindString(name, nameIndex))
                return false;

            if (!buffer->PutU32(nameIndex))
                return false;

            u32 codeIndex = std::numeric_limits<u32>().max();
            if (!stringTable.TryFindString(code, codeIndex))
                return false;

            if (!buffer->PutU32(codeIndex))
                return false;

            return true;
        }

        bool WriteStringTable(Novus::Container::StringTableUnsafe& stringTable) override
        {
            stringTable.AddString(name);
            stringTable.AddString(code);

            return true;
        }
    };
    struct Cursor : public Base
    {
    public:
        std::string name;
        std::string texturePath;

    public:
        bool Read(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) override
        {
            if (!buffer->GetU32(id))
                return false;

            u32 nameIndex = 0;
            u32 texturePathIndex = 0;

            if (!buffer->GetU32(nameIndex))
                return false;

            if (!buffer->GetU32(texturePathIndex))
                return false;

            name = stringTable.GetString(nameIndex);
            texturePath = stringTable.GetString(texturePathIndex);

            return true;
        }

        bool Write(std::shared_ptr<Bytebuffer>& buffer, const Novus::Container::StringTableUnsafe& stringTable) override
        {
            if (!buffer->PutU32(id))
                return false;

            u32 nameIndex = std::numeric_limits<u32>().max();
            if (!stringTable.TryFindString(name, nameIndex))
                return false;

            if (!buffer->PutU32(nameIndex))
                return false;

            u32 texturePathIndex = std::numeric_limits<u32>().max();
            if (!stringTable.TryFindString(texturePath, texturePathIndex))
                return false;

            if (!buffer->PutU32(texturePathIndex))
                return false;

            return true;
        }

        bool WriteStringTable(Novus::Container::StringTableUnsafe& stringTable) override
        {
            stringTable.AddString(name);
            stringTable.AddString(texturePath);

            return true;
        }
    };
}
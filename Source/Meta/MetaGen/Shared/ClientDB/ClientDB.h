#pragma once

#include <Base/Types.h>
#include <Base/Memory/Bytebuffer.h>
#include <FileFormat/Novus/ClientDB/ClientDB.h>

namespace MetaGen::Shared::ClientDB
{
    struct TextureFileDataRecord
    {
    public:
        StringRef texture;
        u32 materialResourcesID;

    public:
        static inline std::string NAME = "TextureFileDataRecord";
        static constexpr u32 NAME_HASH = 2259103733;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "texture", ::ClientDB::FieldType::StringRef }, { "materialResourcesID", ::ClientDB::FieldType::u32 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(texture);
            failed |= !buffer->PutU32(materialResourcesID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(texture);
            failed |= !buffer->GetU32(materialResourcesID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // texture
            result += 4; // materialResourcesID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "texture : " + std::to_string(texture) + ", ";
            tmp += "materialResourcesID : " + std::to_string(materialResourcesID);

            return tmp;
        }
    };

    struct ModelFileDataRecord
    {
    public:
        StringRef model;
        u32 modelResourcesID;
        u8 flags;

    public:
        static inline std::string NAME = "ModelFileDataRecord";
        static constexpr u32 NAME_HASH = 734844835;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "model", ::ClientDB::FieldType::StringRef }, { "modelResourcesID", ::ClientDB::FieldType::u32 }, { "flags", ::ClientDB::FieldType::u8 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(model);
            failed |= !buffer->PutU32(modelResourcesID);
            failed |= !buffer->PutU8(flags);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(model);
            failed |= !buffer->GetU32(modelResourcesID);
            failed |= !buffer->GetU8(flags);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // model
            result += 4; // modelResourcesID
            result += 1; // flags

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "model : " + std::to_string(model) + ", ";
            tmp += "modelResourcesID : " + std::to_string(modelResourcesID) + ", ";
            tmp += "flags : " + std::to_string(flags);

            return tmp;
        }
    };

    struct AnimationDataRecord
    {
    public:
        u64 flags;
        u16 fallback;
        u16 behaviorID;
        u8 behaviorTier;

    public:
        static inline std::string NAME = "AnimationDataRecord";
        static constexpr u32 NAME_HASH = 1014337468;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "flags", ::ClientDB::FieldType::u64 }, { "fallback", ::ClientDB::FieldType::u16 }, { "behaviorID", ::ClientDB::FieldType::u16 }, { "behaviorTier", ::ClientDB::FieldType::u8 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU64(flags);
            failed |= !buffer->PutU16(fallback);
            failed |= !buffer->PutU16(behaviorID);
            failed |= !buffer->PutU8(behaviorTier);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU64(flags);
            failed |= !buffer->GetU16(fallback);
            failed |= !buffer->GetU16(behaviorID);
            failed |= !buffer->GetU8(behaviorTier);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 8; // flags
            result += 2; // fallback
            result += 2; // behaviorID
            result += 1; // behaviorTier

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "flags : " + std::to_string(flags) + ", ";
            tmp += "fallback : " + std::to_string(fallback) + ", ";
            tmp += "behaviorID : " + std::to_string(behaviorID) + ", ";
            tmp += "behaviorTier : " + std::to_string(behaviorTier);

            return tmp;
        }
    };

    struct CameraSaveRecord
    {
    public:
        StringRef name;
        StringRef code;

    public:
        static inline std::string NAME = "CameraSaveRecord";
        static constexpr u32 NAME_HASH = 2713319678;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "name", ::ClientDB::FieldType::StringRef }, { "code", ::ClientDB::FieldType::StringRef } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(name);
            failed |= !buffer->PutU32(code);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(name);
            failed |= !buffer->GetU32(code);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // name
            result += 4; // code

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "name : " + std::to_string(name) + ", ";
            tmp += "code : " + std::to_string(code);

            return tmp;
        }
    };

    struct CinematicCameraRecord
    {
    public:
        u32 soundID;
        vec3 endPosition;
        f32 rotation;
        StringRef model;

    public:
        static inline std::string NAME = "CinematicCameraRecord";
        static constexpr u32 NAME_HASH = 3141969404;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "soundID", ::ClientDB::FieldType::u32 }, { "endPosition", ::ClientDB::FieldType::vec3 }, { "rotation", ::ClientDB::FieldType::f32 }, { "model", ::ClientDB::FieldType::StringRef } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(soundID);
            failed |= !buffer->Put(endPosition);
            failed |= !buffer->PutF32(rotation);
            failed |= !buffer->PutU32(model);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(soundID);
            failed |= !buffer->Get(endPosition);
            failed |= !buffer->GetF32(rotation);
            failed |= !buffer->GetU32(model);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // soundID
            result += 12; // endPosition
            result += 4; // rotation
            result += 4; // model

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "soundID : " + std::to_string(soundID) + ", ";
            tmp += std::string("endPosition : ") + "(X : " + std::to_string(endPosition.x) + ", Y : " + std::to_string(endPosition.y) + ", Z : " + std::to_string(endPosition.z) + ")" + ", ";
            tmp += "rotation : " + std::to_string(rotation) + ", ";
            tmp += "model : " + std::to_string(model);

            return tmp;
        }
    };

    struct CinematicSequenceRecord
    {
    public:
        u16 cameraID;

    public:
        static inline std::string NAME = "CinematicSequenceRecord";
        static constexpr u32 NAME_HASH = 3725098338;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "cameraID", ::ClientDB::FieldType::u16 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU16(cameraID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU16(cameraID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 2; // cameraID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "cameraID : " + std::to_string(cameraID);

            return tmp;
        }
    };

    struct CreatureModelDataRecord
    {
    public:
        StringRef model;
        u32 flags;
        vec3 boxMin;
        vec3 boxMax;
        u16 soundID;
        u8 sizeClass;
        u8 bloodID;
        u8 footprintTextureID;
        u8 footprintTextureLength;
        u8 footprintTextureWidth;
        u8 footprintParticleScale;
        u16 footstepCameraEffectID;
        u16 deathThudCameraEffectID;
        vec2 collisionBox;
        f32 mountHeight;

    public:
        static inline std::string NAME = "CreatureModelDataRecord";
        static constexpr u32 NAME_HASH = 665118788;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "model", ::ClientDB::FieldType::StringRef }, { "flags", ::ClientDB::FieldType::u32 }, { "boxMin", ::ClientDB::FieldType::vec3 }, { "boxMax", ::ClientDB::FieldType::vec3 }, { "soundID", ::ClientDB::FieldType::u16 }, { "sizeClass", ::ClientDB::FieldType::u8 }, { "bloodID", ::ClientDB::FieldType::u8 }, { "footprintTextureID", ::ClientDB::FieldType::u8 }, { "footprintTextureLength", ::ClientDB::FieldType::u8 }, { "footprintTextureWidth", ::ClientDB::FieldType::u8 }, { "footprintParticleScale", ::ClientDB::FieldType::u8 }, { "footstepCameraEffectID", ::ClientDB::FieldType::u16 }, { "deathThudCameraEffectID", ::ClientDB::FieldType::u16 }, { "collisionBox", ::ClientDB::FieldType::vec2 }, { "mountHeight", ::ClientDB::FieldType::f32 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(model);
            failed |= !buffer->PutU32(flags);
            failed |= !buffer->Put(boxMin);
            failed |= !buffer->Put(boxMax);
            failed |= !buffer->PutU16(soundID);
            failed |= !buffer->PutU8(sizeClass);
            failed |= !buffer->PutU8(bloodID);
            failed |= !buffer->PutU8(footprintTextureID);
            failed |= !buffer->PutU8(footprintTextureLength);
            failed |= !buffer->PutU8(footprintTextureWidth);
            failed |= !buffer->PutU8(footprintParticleScale);
            failed |= !buffer->PutU16(footstepCameraEffectID);
            failed |= !buffer->PutU16(deathThudCameraEffectID);
            failed |= !buffer->Put(collisionBox);
            failed |= !buffer->PutF32(mountHeight);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(model);
            failed |= !buffer->GetU32(flags);
            failed |= !buffer->Get(boxMin);
            failed |= !buffer->Get(boxMax);
            failed |= !buffer->GetU16(soundID);
            failed |= !buffer->GetU8(sizeClass);
            failed |= !buffer->GetU8(bloodID);
            failed |= !buffer->GetU8(footprintTextureID);
            failed |= !buffer->GetU8(footprintTextureLength);
            failed |= !buffer->GetU8(footprintTextureWidth);
            failed |= !buffer->GetU8(footprintParticleScale);
            failed |= !buffer->GetU16(footstepCameraEffectID);
            failed |= !buffer->GetU16(deathThudCameraEffectID);
            failed |= !buffer->Get(collisionBox);
            failed |= !buffer->GetF32(mountHeight);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // model
            result += 4; // flags
            result += 12; // boxMin
            result += 12; // boxMax
            result += 2; // soundID
            result += 1; // sizeClass
            result += 1; // bloodID
            result += 1; // footprintTextureID
            result += 1; // footprintTextureLength
            result += 1; // footprintTextureWidth
            result += 1; // footprintParticleScale
            result += 2; // footstepCameraEffectID
            result += 2; // deathThudCameraEffectID
            result += 8; // collisionBox
            result += 4; // mountHeight

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "model : " + std::to_string(model) + ", ";
            tmp += "flags : " + std::to_string(flags) + ", ";
            tmp += std::string("boxMin : ") + "(X : " + std::to_string(boxMin.x) + ", Y : " + std::to_string(boxMin.y) + ", Z : " + std::to_string(boxMin.z) + ")" + ", ";
            tmp += std::string("boxMax : ") + "(X : " + std::to_string(boxMax.x) + ", Y : " + std::to_string(boxMax.y) + ", Z : " + std::to_string(boxMax.z) + ")" + ", ";
            tmp += "soundID : " + std::to_string(soundID) + ", ";
            tmp += "sizeClass : " + std::to_string(sizeClass) + ", ";
            tmp += "bloodID : " + std::to_string(bloodID) + ", ";
            tmp += "footprintTextureID : " + std::to_string(footprintTextureID) + ", ";
            tmp += "footprintTextureLength : " + std::to_string(footprintTextureLength) + ", ";
            tmp += "footprintTextureWidth : " + std::to_string(footprintTextureWidth) + ", ";
            tmp += "footprintParticleScale : " + std::to_string(footprintParticleScale) + ", ";
            tmp += "footstepCameraEffectID : " + std::to_string(footstepCameraEffectID) + ", ";
            tmp += "deathThudCameraEffectID : " + std::to_string(deathThudCameraEffectID) + ", ";
            tmp += std::string("collisionBox : ") + "(X : " + std::to_string(collisionBox.x) + ", Y : " + std::to_string(collisionBox.y) + ")" + ", ";
            tmp += "mountHeight : " + std::to_string(mountHeight);

            return tmp;
        }
    };

    struct CreatureDisplayInfoRecord
    {
    public:
        u32 modelID;
        u32 extendedDisplayInfoID;
        u16 soundID;
        u16 npcSoundID;
        u8 flags;
        u8 gender;
        i8 sizeClass;
        u8 bloodID;
        u8 unarmedWeaponType;
        u8 creatureModelAlpha;
        f32 creatureModelScale;
        f32 creaturePetScale;
        std::array<StringRef, 4> textureVariations;

    public:
        static inline std::string NAME = "CreatureDisplayInfoRecord";
        static constexpr u32 NAME_HASH = 2627811369;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "modelID", ::ClientDB::FieldType::u32 }, { "extendedDisplayInfoID", ::ClientDB::FieldType::u32 }, { "soundID", ::ClientDB::FieldType::u16 }, { "npcSoundID", ::ClientDB::FieldType::u16 }, { "flags", ::ClientDB::FieldType::u8 }, { "gender", ::ClientDB::FieldType::u8 }, { "sizeClass", ::ClientDB::FieldType::i8 }, { "bloodID", ::ClientDB::FieldType::u8 }, { "unarmedWeaponType", ::ClientDB::FieldType::u8 }, { "creatureModelAlpha", ::ClientDB::FieldType::u8 }, { "creatureModelScale", ::ClientDB::FieldType::f32 }, { "creaturePetScale", ::ClientDB::FieldType::f32 }, { "textureVariations", ::ClientDB::FieldType::StringRef, 4 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(modelID);
            failed |= !buffer->PutU32(extendedDisplayInfoID);
            failed |= !buffer->PutU16(soundID);
            failed |= !buffer->PutU16(npcSoundID);
            failed |= !buffer->PutU8(flags);
            failed |= !buffer->PutU8(gender);
            failed |= !buffer->PutI8(sizeClass);
            failed |= !buffer->PutU8(bloodID);
            failed |= !buffer->PutU8(unarmedWeaponType);
            failed |= !buffer->PutU8(creatureModelAlpha);
            failed |= !buffer->PutF32(creatureModelScale);
            failed |= !buffer->PutF32(creaturePetScale);
            failed |= !buffer->PutBytes(textureVariations.data(), textureVariations.size());

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(modelID);
            failed |= !buffer->GetU32(extendedDisplayInfoID);
            failed |= !buffer->GetU16(soundID);
            failed |= !buffer->GetU16(npcSoundID);
            failed |= !buffer->GetU8(flags);
            failed |= !buffer->GetU8(gender);
            failed |= !buffer->GetI8(sizeClass);
            failed |= !buffer->GetU8(bloodID);
            failed |= !buffer->GetU8(unarmedWeaponType);
            failed |= !buffer->GetU8(creatureModelAlpha);
            failed |= !buffer->GetF32(creatureModelScale);
            failed |= !buffer->GetF32(creaturePetScale);
            failed |= !buffer->GetBytes(textureVariations.data(), 4 * sizeof(StringRef));

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // modelID
            result += 4; // extendedDisplayInfoID
            result += 2; // soundID
            result += 2; // npcSoundID
            result += 1; // flags
            result += 1; // gender
            result += 1; // sizeClass
            result += 1; // bloodID
            result += 1; // unarmedWeaponType
            result += 1; // creatureModelAlpha
            result += 4; // creatureModelScale
            result += 4; // creaturePetScale
            result += 4 * sizeof(StringRef); // textureVariations

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "modelID : " + std::to_string(modelID) + ", ";
            tmp += "extendedDisplayInfoID : " + std::to_string(extendedDisplayInfoID) + ", ";
            tmp += "soundID : " + std::to_string(soundID) + ", ";
            tmp += "npcSoundID : " + std::to_string(npcSoundID) + ", ";
            tmp += "flags : " + std::to_string(flags) + ", ";
            tmp += "gender : " + std::to_string(gender) + ", ";
            tmp += "sizeClass : " + std::to_string(sizeClass) + ", ";
            tmp += "bloodID : " + std::to_string(bloodID) + ", ";
            tmp += "unarmedWeaponType : " + std::to_string(unarmedWeaponType) + ", ";
            tmp += "creatureModelAlpha : " + std::to_string(creatureModelAlpha) + ", ";
            tmp += "creatureModelScale : " + std::to_string(creatureModelScale) + ", ";
            tmp += "creaturePetScale : " + std::to_string(creaturePetScale) + ", ";
            tmp += std::string("textureVariations : Arrays are unsupported");

            return tmp;
        }
    };

    struct CreatureDisplayInfoExtraRecord
    {
    public:
        u8 flags;
        u8 raceID;
        u8 gender;
        u8 classID;
        u8 skinID;
        u8 faceID;
        u8 hairStyleID;
        u8 hairColorID;
        u8 facialHairID;
        StringRef bakedTexture;

    public:
        static inline std::string NAME = "CreatureDisplayInfoExtraRecord";
        static constexpr u32 NAME_HASH = 465351691;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "flags", ::ClientDB::FieldType::u8 }, { "raceID", ::ClientDB::FieldType::u8 }, { "gender", ::ClientDB::FieldType::u8 }, { "classID", ::ClientDB::FieldType::u8 }, { "skinID", ::ClientDB::FieldType::u8 }, { "faceID", ::ClientDB::FieldType::u8 }, { "hairStyleID", ::ClientDB::FieldType::u8 }, { "hairColorID", ::ClientDB::FieldType::u8 }, { "facialHairID", ::ClientDB::FieldType::u8 }, { "bakedTexture", ::ClientDB::FieldType::StringRef } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU8(flags);
            failed |= !buffer->PutU8(raceID);
            failed |= !buffer->PutU8(gender);
            failed |= !buffer->PutU8(classID);
            failed |= !buffer->PutU8(skinID);
            failed |= !buffer->PutU8(faceID);
            failed |= !buffer->PutU8(hairStyleID);
            failed |= !buffer->PutU8(hairColorID);
            failed |= !buffer->PutU8(facialHairID);
            failed |= !buffer->PutU32(bakedTexture);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU8(flags);
            failed |= !buffer->GetU8(raceID);
            failed |= !buffer->GetU8(gender);
            failed |= !buffer->GetU8(classID);
            failed |= !buffer->GetU8(skinID);
            failed |= !buffer->GetU8(faceID);
            failed |= !buffer->GetU8(hairStyleID);
            failed |= !buffer->GetU8(hairColorID);
            failed |= !buffer->GetU8(facialHairID);
            failed |= !buffer->GetU32(bakedTexture);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 1; // flags
            result += 1; // raceID
            result += 1; // gender
            result += 1; // classID
            result += 1; // skinID
            result += 1; // faceID
            result += 1; // hairStyleID
            result += 1; // hairColorID
            result += 1; // facialHairID
            result += 4; // bakedTexture

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "flags : " + std::to_string(flags) + ", ";
            tmp += "raceID : " + std::to_string(raceID) + ", ";
            tmp += "gender : " + std::to_string(gender) + ", ";
            tmp += "classID : " + std::to_string(classID) + ", ";
            tmp += "skinID : " + std::to_string(skinID) + ", ";
            tmp += "faceID : " + std::to_string(faceID) + ", ";
            tmp += "hairStyleID : " + std::to_string(hairStyleID) + ", ";
            tmp += "hairColorID : " + std::to_string(hairColorID) + ", ";
            tmp += "facialHairID : " + std::to_string(facialHairID) + ", ";
            tmp += "bakedTexture : " + std::to_string(bakedTexture);

            return tmp;
        }
    };

    struct CursorRecord
    {
    public:
        StringRef name;
        StringRef texture;

    public:
        static inline std::string NAME = "CursorRecord";
        static constexpr u32 NAME_HASH = 1086526162;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "name", ::ClientDB::FieldType::StringRef }, { "texture", ::ClientDB::FieldType::StringRef } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(name);
            failed |= !buffer->PutU32(texture);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(name);
            failed |= !buffer->GetU32(texture);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // name
            result += 4; // texture

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "name : " + std::to_string(name) + ", ";
            tmp += "texture : " + std::to_string(texture);

            return tmp;
        }
    };

    struct IconRecord
    {
    public:
        StringRef texture;

    public:
        static inline std::string NAME = "IconRecord";
        static constexpr u32 NAME_HASH = 937822319;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "texture", ::ClientDB::FieldType::StringRef } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(texture);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(texture);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // texture

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "texture : " + std::to_string(texture);

            return tmp;
        }
    };

    struct ItemDisplayInfoRecord
    {
    public:
        u16 flags;
        u16 itemRangedDisplayInfoID;
        std::array<u32, 2> modelResourcesID;
        std::array<u32, 2> modelMaterialResourcesID;
        std::array<u8, 4> modelGeosetGroups;
        std::array<u16, 2> modelGeosetVisIDs;

    public:
        static inline std::string NAME = "ItemDisplayInfoRecord";
        static constexpr u32 NAME_HASH = 2897499367;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "flags", ::ClientDB::FieldType::u16 }, { "itemRangedDisplayInfoID", ::ClientDB::FieldType::u16 }, { "modelResourcesID", ::ClientDB::FieldType::u32, 2 }, { "modelMaterialResourcesID", ::ClientDB::FieldType::u32, 2 }, { "modelGeosetGroups", ::ClientDB::FieldType::u8, 4 }, { "modelGeosetVisIDs", ::ClientDB::FieldType::u16, 2 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU16(flags);
            failed |= !buffer->PutU16(itemRangedDisplayInfoID);
            failed |= !buffer->PutBytes(modelResourcesID.data(), modelResourcesID.size());
            failed |= !buffer->PutBytes(modelMaterialResourcesID.data(), modelMaterialResourcesID.size());
            failed |= !buffer->PutBytes(modelGeosetGroups.data(), modelGeosetGroups.size());
            failed |= !buffer->PutBytes(modelGeosetVisIDs.data(), modelGeosetVisIDs.size());

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU16(flags);
            failed |= !buffer->GetU16(itemRangedDisplayInfoID);
            failed |= !buffer->GetBytes(modelResourcesID.data(), 2 * sizeof(u32));
            failed |= !buffer->GetBytes(modelMaterialResourcesID.data(), 2 * sizeof(u32));
            failed |= !buffer->GetBytes(modelGeosetGroups.data(), 4 * sizeof(u8));
            failed |= !buffer->GetBytes(modelGeosetVisIDs.data(), 2 * sizeof(u16));

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 2; // flags
            result += 2; // itemRangedDisplayInfoID
            result += 2 * sizeof(u32); // modelResourcesID
            result += 2 * sizeof(u32); // modelMaterialResourcesID
            result += 4 * sizeof(u8); // modelGeosetGroups
            result += 2 * sizeof(u16); // modelGeosetVisIDs

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "flags : " + std::to_string(flags) + ", ";
            tmp += "itemRangedDisplayInfoID : " + std::to_string(itemRangedDisplayInfoID) + ", ";
            tmp += std::string("modelResourcesID : Arrays are unsupported") + ", ";
            tmp += std::string("modelMaterialResourcesID : Arrays are unsupported") + ", ";
            tmp += std::string("modelGeosetGroups : Arrays are unsupported") + ", ";
            tmp += std::string("modelGeosetVisIDs : Arrays are unsupported");

            return tmp;
        }
    };

    struct ItemDisplayInfoMaterialResourceRecord
    {
    public:
        u32 displayInfoID;
        u8 componentSection;
        u32 materialResourcesID;

    public:
        static inline std::string NAME = "ItemDisplayInfoMaterialResourceRecord";
        static constexpr u32 NAME_HASH = 61432464;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "displayInfoID", ::ClientDB::FieldType::u32 }, { "componentSection", ::ClientDB::FieldType::u8 }, { "materialResourcesID", ::ClientDB::FieldType::u32 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(displayInfoID);
            failed |= !buffer->PutU8(componentSection);
            failed |= !buffer->PutU32(materialResourcesID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(displayInfoID);
            failed |= !buffer->GetU8(componentSection);
            failed |= !buffer->GetU32(materialResourcesID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // displayInfoID
            result += 1; // componentSection
            result += 4; // materialResourcesID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "displayInfoID : " + std::to_string(displayInfoID) + ", ";
            tmp += "componentSection : " + std::to_string(componentSection) + ", ";
            tmp += "materialResourcesID : " + std::to_string(materialResourcesID);

            return tmp;
        }
    };

    struct ItemDisplayInfoModelMaterialResourceRecord
    {
    public:
        u32 displayInfoID;
        u8 modelIndex;
        u8 textureType;
        u32 materialResourcesID;

    public:
        static inline std::string NAME = "ItemDisplayInfoModelMaterialResourceRecord";
        static constexpr u32 NAME_HASH = 137796419;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "displayInfoID", ::ClientDB::FieldType::u32 }, { "modelIndex", ::ClientDB::FieldType::u8 }, { "textureType", ::ClientDB::FieldType::u8 }, { "materialResourcesID", ::ClientDB::FieldType::u32 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(displayInfoID);
            failed |= !buffer->PutU8(modelIndex);
            failed |= !buffer->PutU8(textureType);
            failed |= !buffer->PutU32(materialResourcesID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(displayInfoID);
            failed |= !buffer->GetU8(modelIndex);
            failed |= !buffer->GetU8(textureType);
            failed |= !buffer->GetU32(materialResourcesID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // displayInfoID
            result += 1; // modelIndex
            result += 1; // textureType
            result += 4; // materialResourcesID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "displayInfoID : " + std::to_string(displayInfoID) + ", ";
            tmp += "modelIndex : " + std::to_string(modelIndex) + ", ";
            tmp += "textureType : " + std::to_string(textureType) + ", ";
            tmp += "materialResourcesID : " + std::to_string(materialResourcesID);

            return tmp;
        }
    };

    struct ItemRecord
    {
    public:
        StringRef name;
        StringRef description;
        u32 iconID;
        u32 displayID;
        u8 bind;
        u8 rarity;
        u8 category;
        u8 categoryType;
        u16 virtualLevel;
        u16 requiredLevel;
        u32 durability;
        u32 armor;
        u32 statTemplateID;
        u32 armorTemplateID;
        u32 weaponTemplateID;
        u32 shieldTemplateID;

    public:
        static inline std::string NAME = "ItemRecord";
        static constexpr u32 NAME_HASH = 464316057;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "name", ::ClientDB::FieldType::StringRef }, { "description", ::ClientDB::FieldType::StringRef }, { "iconID", ::ClientDB::FieldType::u32 }, { "displayID", ::ClientDB::FieldType::u32 }, { "bind", ::ClientDB::FieldType::u8 }, { "rarity", ::ClientDB::FieldType::u8 }, { "category", ::ClientDB::FieldType::u8 }, { "categoryType", ::ClientDB::FieldType::u8 }, { "virtualLevel", ::ClientDB::FieldType::u16 }, { "requiredLevel", ::ClientDB::FieldType::u16 }, { "durability", ::ClientDB::FieldType::u32 }, { "armor", ::ClientDB::FieldType::u32 }, { "statTemplateID", ::ClientDB::FieldType::u32 }, { "armorTemplateID", ::ClientDB::FieldType::u32 }, { "weaponTemplateID", ::ClientDB::FieldType::u32 }, { "shieldTemplateID", ::ClientDB::FieldType::u32 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(name);
            failed |= !buffer->PutU32(description);
            failed |= !buffer->PutU32(iconID);
            failed |= !buffer->PutU32(displayID);
            failed |= !buffer->PutU8(bind);
            failed |= !buffer->PutU8(rarity);
            failed |= !buffer->PutU8(category);
            failed |= !buffer->PutU8(categoryType);
            failed |= !buffer->PutU16(virtualLevel);
            failed |= !buffer->PutU16(requiredLevel);
            failed |= !buffer->PutU32(durability);
            failed |= !buffer->PutU32(armor);
            failed |= !buffer->PutU32(statTemplateID);
            failed |= !buffer->PutU32(armorTemplateID);
            failed |= !buffer->PutU32(weaponTemplateID);
            failed |= !buffer->PutU32(shieldTemplateID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(name);
            failed |= !buffer->GetU32(description);
            failed |= !buffer->GetU32(iconID);
            failed |= !buffer->GetU32(displayID);
            failed |= !buffer->GetU8(bind);
            failed |= !buffer->GetU8(rarity);
            failed |= !buffer->GetU8(category);
            failed |= !buffer->GetU8(categoryType);
            failed |= !buffer->GetU16(virtualLevel);
            failed |= !buffer->GetU16(requiredLevel);
            failed |= !buffer->GetU32(durability);
            failed |= !buffer->GetU32(armor);
            failed |= !buffer->GetU32(statTemplateID);
            failed |= !buffer->GetU32(armorTemplateID);
            failed |= !buffer->GetU32(weaponTemplateID);
            failed |= !buffer->GetU32(shieldTemplateID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // name
            result += 4; // description
            result += 4; // iconID
            result += 4; // displayID
            result += 1; // bind
            result += 1; // rarity
            result += 1; // category
            result += 1; // categoryType
            result += 2; // virtualLevel
            result += 2; // requiredLevel
            result += 4; // durability
            result += 4; // armor
            result += 4; // statTemplateID
            result += 4; // armorTemplateID
            result += 4; // weaponTemplateID
            result += 4; // shieldTemplateID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "name : " + std::to_string(name) + ", ";
            tmp += "description : " + std::to_string(description) + ", ";
            tmp += "iconID : " + std::to_string(iconID) + ", ";
            tmp += "displayID : " + std::to_string(displayID) + ", ";
            tmp += "bind : " + std::to_string(bind) + ", ";
            tmp += "rarity : " + std::to_string(rarity) + ", ";
            tmp += "category : " + std::to_string(category) + ", ";
            tmp += "categoryType : " + std::to_string(categoryType) + ", ";
            tmp += "virtualLevel : " + std::to_string(virtualLevel) + ", ";
            tmp += "requiredLevel : " + std::to_string(requiredLevel) + ", ";
            tmp += "durability : " + std::to_string(durability) + ", ";
            tmp += "armor : " + std::to_string(armor) + ", ";
            tmp += "statTemplateID : " + std::to_string(statTemplateID) + ", ";
            tmp += "armorTemplateID : " + std::to_string(armorTemplateID) + ", ";
            tmp += "weaponTemplateID : " + std::to_string(weaponTemplateID) + ", ";
            tmp += "shieldTemplateID : " + std::to_string(shieldTemplateID);

            return tmp;
        }
    };

    struct ItemStatTypeRecord
    {
    public:
        StringRef name;
        StringRef description;

    public:
        static inline std::string NAME = "ItemStatTypeRecord";
        static constexpr u32 NAME_HASH = 1177124275;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "name", ::ClientDB::FieldType::StringRef }, { "description", ::ClientDB::FieldType::StringRef } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(name);
            failed |= !buffer->PutU32(description);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(name);
            failed |= !buffer->GetU32(description);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // name
            result += 4; // description

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "name : " + std::to_string(name) + ", ";
            tmp += "description : " + std::to_string(description);

            return tmp;
        }
    };

    struct ItemStatTemplateRecord
    {
    public:
        std::array<u8, 8> statTypeID;
        std::array<i32, 8> value;

    public:
        static inline std::string NAME = "ItemStatTemplateRecord";
        static constexpr u32 NAME_HASH = 2717286265;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "statTypeID", ::ClientDB::FieldType::u8, 8 }, { "value", ::ClientDB::FieldType::i32, 8 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutBytes(statTypeID.data(), statTypeID.size());
            failed |= !buffer->PutBytes(value.data(), value.size());

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetBytes(statTypeID.data(), 8 * sizeof(u8));
            failed |= !buffer->GetBytes(value.data(), 8 * sizeof(i32));

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 8 * sizeof(u8); // statTypeID
            result += 8 * sizeof(i32); // value

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += std::string("statTypeID : Arrays are unsupported") + ", ";
            tmp += std::string("value : Arrays are unsupported");

            return tmp;
        }
    };

    struct ItemArmorTemplateRecord
    {
    public:
        u8 equipType;
        u32 bonusArmor;

    public:
        static inline std::string NAME = "ItemArmorTemplateRecord";
        static constexpr u32 NAME_HASH = 3102936142;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "equipType", ::ClientDB::FieldType::u8 }, { "bonusArmor", ::ClientDB::FieldType::u32 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU8(equipType);
            failed |= !buffer->PutU32(bonusArmor);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU8(equipType);
            failed |= !buffer->GetU32(bonusArmor);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 1; // equipType
            result += 4; // bonusArmor

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "equipType : " + std::to_string(equipType) + ", ";
            tmp += "bonusArmor : " + std::to_string(bonusArmor);

            return tmp;
        }
    };

    struct ItemShieldTemplateRecord
    {
    public:
        u32 bonusArmor;
        u32 block;

    public:
        static inline std::string NAME = "ItemShieldTemplateRecord";
        static constexpr u32 NAME_HASH = 3976556312;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "bonusArmor", ::ClientDB::FieldType::u32 }, { "block", ::ClientDB::FieldType::u32 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(bonusArmor);
            failed |= !buffer->PutU32(block);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(bonusArmor);
            failed |= !buffer->GetU32(block);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // bonusArmor
            result += 4; // block

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "bonusArmor : " + std::to_string(bonusArmor) + ", ";
            tmp += "block : " + std::to_string(block);

            return tmp;
        }
    };

    struct ItemWeaponTemplateRecord
    {
    public:
        u8 weaponStyle;
        uvec2 damageRange;
        f32 speed;

    public:
        static inline std::string NAME = "ItemWeaponTemplateRecord";
        static constexpr u32 NAME_HASH = 4142802919;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "weaponStyle", ::ClientDB::FieldType::u8 }, { "damageRange", ::ClientDB::FieldType::uvec2 }, { "speed", ::ClientDB::FieldType::f32 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU8(weaponStyle);
            failed |= !buffer->Put(damageRange);
            failed |= !buffer->PutF32(speed);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU8(weaponStyle);
            failed |= !buffer->Get(damageRange);
            failed |= !buffer->GetF32(speed);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 1; // weaponStyle
            result += 8; // damageRange
            result += 4; // speed

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "weaponStyle : " + std::to_string(weaponStyle) + ", ";
            tmp += std::string("damageRange : ") + "(X : " + std::to_string(damageRange.x) + ", Y : " + std::to_string(damageRange.y) + ")" + ", ";
            tmp += "speed : " + std::to_string(speed);

            return tmp;
        }
    };

    struct ItemEffectRecord
    {
    public:
        u32 itemID;
        u8 effectSlot;
        u8 effectType;
        u32 effectSpellID;

    public:
        static inline std::string NAME = "ItemEffectRecord";
        static constexpr u32 NAME_HASH = 3893237422;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "itemID", ::ClientDB::FieldType::u32 }, { "effectSlot", ::ClientDB::FieldType::u8 }, { "effectType", ::ClientDB::FieldType::u8 }, { "effectSpellID", ::ClientDB::FieldType::u32 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(itemID);
            failed |= !buffer->PutU8(effectSlot);
            failed |= !buffer->PutU8(effectType);
            failed |= !buffer->PutU32(effectSpellID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(itemID);
            failed |= !buffer->GetU8(effectSlot);
            failed |= !buffer->GetU8(effectType);
            failed |= !buffer->GetU32(effectSpellID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // itemID
            result += 1; // effectSlot
            result += 1; // effectType
            result += 4; // effectSpellID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "itemID : " + std::to_string(itemID) + ", ";
            tmp += "effectSlot : " + std::to_string(effectSlot) + ", ";
            tmp += "effectType : " + std::to_string(effectType) + ", ";
            tmp += "effectSpellID : " + std::to_string(effectSpellID);

            return tmp;
        }
    };

    struct LightRecord
    {
    public:
        u16 mapID;
        vec3 position;
        vec2 fallOff;
        std::array<u16, 8> paramIDs;

    public:
        static inline std::string NAME = "LightRecord";
        static constexpr u32 NAME_HASH = 1265551218;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "mapID", ::ClientDB::FieldType::u16 }, { "position", ::ClientDB::FieldType::vec3 }, { "fallOff", ::ClientDB::FieldType::vec2 }, { "paramIDs", ::ClientDB::FieldType::u16, 8 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU16(mapID);
            failed |= !buffer->Put(position);
            failed |= !buffer->Put(fallOff);
            failed |= !buffer->PutBytes(paramIDs.data(), paramIDs.size());

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU16(mapID);
            failed |= !buffer->Get(position);
            failed |= !buffer->Get(fallOff);
            failed |= !buffer->GetBytes(paramIDs.data(), 8 * sizeof(u16));

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 2; // mapID
            result += 12; // position
            result += 8; // fallOff
            result += 8 * sizeof(u16); // paramIDs

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "mapID : " + std::to_string(mapID) + ", ";
            tmp += std::string("position : ") + "(X : " + std::to_string(position.x) + ", Y : " + std::to_string(position.y) + ", Z : " + std::to_string(position.z) + ")" + ", ";
            tmp += std::string("fallOff : ") + "(X : " + std::to_string(fallOff.x) + ", Y : " + std::to_string(fallOff.y) + ")" + ", ";
            tmp += std::string("paramIDs : Arrays are unsupported");

            return tmp;
        }
    };

    struct LightDataRecord
    {
    public:
        u16 lightParamID;
        u32 timestamp;
        u32 diffuseColor;
        u32 ambientColor;
        std::array<u32, 6> skyColors;
        u32 sunColor;
        u32 sunFogColor;
        f32 sunFogStrength;
        f32 sunFogAngle;
        std::array<u32, 4> cloudColors;
        f32 cloudDensity;
        std::array<u32, 2> oceanColors;
        std::array<u32, 2> riverColors;
        u32 shadowColor;
        f32 fogEnd;
        u32 fogEndColor;
        u32 fogEndHeightColor;
        u32 fogHeightColor;
        f32 fogScaler;
        u8 fogDensity;

    public:
        static inline std::string NAME = "LightDataRecord";
        static constexpr u32 NAME_HASH = 1560500346;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "lightParamID", ::ClientDB::FieldType::u16 }, { "timestamp", ::ClientDB::FieldType::u32 }, { "diffuseColor", ::ClientDB::FieldType::u32 }, { "ambientColor", ::ClientDB::FieldType::u32 }, { "skyColors", ::ClientDB::FieldType::u32, 6 }, { "sunColor", ::ClientDB::FieldType::u32 }, { "sunFogColor", ::ClientDB::FieldType::u32 }, { "sunFogStrength", ::ClientDB::FieldType::f32 }, { "sunFogAngle", ::ClientDB::FieldType::f32 }, { "cloudColors", ::ClientDB::FieldType::u32, 4 }, { "cloudDensity", ::ClientDB::FieldType::f32 }, { "oceanColors", ::ClientDB::FieldType::u32, 2 }, { "riverColors", ::ClientDB::FieldType::u32, 2 }, { "shadowColor", ::ClientDB::FieldType::u32 }, { "fogEnd", ::ClientDB::FieldType::f32 }, { "fogEndColor", ::ClientDB::FieldType::u32 }, { "fogEndHeightColor", ::ClientDB::FieldType::u32 }, { "fogHeightColor", ::ClientDB::FieldType::u32 }, { "fogScaler", ::ClientDB::FieldType::f32 }, { "fogDensity", ::ClientDB::FieldType::u8 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU16(lightParamID);
            failed |= !buffer->PutU32(timestamp);
            failed |= !buffer->PutU32(diffuseColor);
            failed |= !buffer->PutU32(ambientColor);
            failed |= !buffer->PutBytes(skyColors.data(), skyColors.size());
            failed |= !buffer->PutU32(sunColor);
            failed |= !buffer->PutU32(sunFogColor);
            failed |= !buffer->PutF32(sunFogStrength);
            failed |= !buffer->PutF32(sunFogAngle);
            failed |= !buffer->PutBytes(cloudColors.data(), cloudColors.size());
            failed |= !buffer->PutF32(cloudDensity);
            failed |= !buffer->PutBytes(oceanColors.data(), oceanColors.size());
            failed |= !buffer->PutBytes(riverColors.data(), riverColors.size());
            failed |= !buffer->PutU32(shadowColor);
            failed |= !buffer->PutF32(fogEnd);
            failed |= !buffer->PutU32(fogEndColor);
            failed |= !buffer->PutU32(fogEndHeightColor);
            failed |= !buffer->PutU32(fogHeightColor);
            failed |= !buffer->PutF32(fogScaler);
            failed |= !buffer->PutU8(fogDensity);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU16(lightParamID);
            failed |= !buffer->GetU32(timestamp);
            failed |= !buffer->GetU32(diffuseColor);
            failed |= !buffer->GetU32(ambientColor);
            failed |= !buffer->GetBytes(skyColors.data(), 6 * sizeof(u32));
            failed |= !buffer->GetU32(sunColor);
            failed |= !buffer->GetU32(sunFogColor);
            failed |= !buffer->GetF32(sunFogStrength);
            failed |= !buffer->GetF32(sunFogAngle);
            failed |= !buffer->GetBytes(cloudColors.data(), 4 * sizeof(u32));
            failed |= !buffer->GetF32(cloudDensity);
            failed |= !buffer->GetBytes(oceanColors.data(), 2 * sizeof(u32));
            failed |= !buffer->GetBytes(riverColors.data(), 2 * sizeof(u32));
            failed |= !buffer->GetU32(shadowColor);
            failed |= !buffer->GetF32(fogEnd);
            failed |= !buffer->GetU32(fogEndColor);
            failed |= !buffer->GetU32(fogEndHeightColor);
            failed |= !buffer->GetU32(fogHeightColor);
            failed |= !buffer->GetF32(fogScaler);
            failed |= !buffer->GetU8(fogDensity);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 2; // lightParamID
            result += 4; // timestamp
            result += 4; // diffuseColor
            result += 4; // ambientColor
            result += 6 * sizeof(u32); // skyColors
            result += 4; // sunColor
            result += 4; // sunFogColor
            result += 4; // sunFogStrength
            result += 4; // sunFogAngle
            result += 4 * sizeof(u32); // cloudColors
            result += 4; // cloudDensity
            result += 2 * sizeof(u32); // oceanColors
            result += 2 * sizeof(u32); // riverColors
            result += 4; // shadowColor
            result += 4; // fogEnd
            result += 4; // fogEndColor
            result += 4; // fogEndHeightColor
            result += 4; // fogHeightColor
            result += 4; // fogScaler
            result += 1; // fogDensity

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "lightParamID : " + std::to_string(lightParamID) + ", ";
            tmp += "timestamp : " + std::to_string(timestamp) + ", ";
            tmp += "diffuseColor : " + std::to_string(diffuseColor) + ", ";
            tmp += "ambientColor : " + std::to_string(ambientColor) + ", ";
            tmp += std::string("skyColors : Arrays are unsupported") + ", ";
            tmp += "sunColor : " + std::to_string(sunColor) + ", ";
            tmp += "sunFogColor : " + std::to_string(sunFogColor) + ", ";
            tmp += "sunFogStrength : " + std::to_string(sunFogStrength) + ", ";
            tmp += "sunFogAngle : " + std::to_string(sunFogAngle) + ", ";
            tmp += std::string("cloudColors : Arrays are unsupported") + ", ";
            tmp += "cloudDensity : " + std::to_string(cloudDensity) + ", ";
            tmp += std::string("oceanColors : Arrays are unsupported") + ", ";
            tmp += std::string("riverColors : Arrays are unsupported") + ", ";
            tmp += "shadowColor : " + std::to_string(shadowColor) + ", ";
            tmp += "fogEnd : " + std::to_string(fogEnd) + ", ";
            tmp += "fogEndColor : " + std::to_string(fogEndColor) + ", ";
            tmp += "fogEndHeightColor : " + std::to_string(fogEndHeightColor) + ", ";
            tmp += "fogHeightColor : " + std::to_string(fogHeightColor) + ", ";
            tmp += "fogScaler : " + std::to_string(fogScaler) + ", ";
            tmp += "fogDensity : " + std::to_string(fogDensity);

            return tmp;
        }
    };

    struct LightParamRecord
    {
    public:
        u8 flags;
        u16 lightSkyboxID;
        f32 glow;
        std::array<f32, 2> oceanAlphas;
        std::array<f32, 2> riverAlphas;

    public:
        static inline std::string NAME = "LightParamRecord";
        static constexpr u32 NAME_HASH = 824812251;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "flags", ::ClientDB::FieldType::u8 }, { "lightSkyboxID", ::ClientDB::FieldType::u16 }, { "glow", ::ClientDB::FieldType::f32 }, { "oceanAlphas", ::ClientDB::FieldType::f32, 2 }, { "riverAlphas", ::ClientDB::FieldType::f32, 2 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU8(flags);
            failed |= !buffer->PutU16(lightSkyboxID);
            failed |= !buffer->PutF32(glow);
            failed |= !buffer->PutBytes(oceanAlphas.data(), oceanAlphas.size());
            failed |= !buffer->PutBytes(riverAlphas.data(), riverAlphas.size());

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU8(flags);
            failed |= !buffer->GetU16(lightSkyboxID);
            failed |= !buffer->GetF32(glow);
            failed |= !buffer->GetBytes(oceanAlphas.data(), 2 * sizeof(f32));
            failed |= !buffer->GetBytes(riverAlphas.data(), 2 * sizeof(f32));

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 1; // flags
            result += 2; // lightSkyboxID
            result += 4; // glow
            result += 2 * sizeof(f32); // oceanAlphas
            result += 2 * sizeof(f32); // riverAlphas

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "flags : " + std::to_string(flags) + ", ";
            tmp += "lightSkyboxID : " + std::to_string(lightSkyboxID) + ", ";
            tmp += "glow : " + std::to_string(glow) + ", ";
            tmp += std::string("oceanAlphas : Arrays are unsupported") + ", ";
            tmp += std::string("riverAlphas : Arrays are unsupported");

            return tmp;
        }
    };

    struct LightSkyboxRecord
    {
    public:
        StringRef name;
        StringRef model;

    public:
        static inline std::string NAME = "LightSkyboxRecord";
        static constexpr u32 NAME_HASH = 575876980;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "name", ::ClientDB::FieldType::StringRef }, { "model", ::ClientDB::FieldType::StringRef } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(name);
            failed |= !buffer->PutU32(model);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(name);
            failed |= !buffer->GetU32(model);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // name
            result += 4; // model

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "name : " + std::to_string(name) + ", ";
            tmp += "model : " + std::to_string(model);

            return tmp;
        }
    };

    struct LiquidTypeRecord
    {
    public:
        StringRef name;
        u16 flags;
        u16 lightID;
        u8 soundBank;
        u16 soundID;
        u8 materialID;
        u8 particleScale;
        u8 particleMovement;
        u8 particleTextureSlot;
        u32 minimapColor;
        u32 maxDarkenDepth;
        f32 fogDarkenIntensity;
        f32 ambDarkenIntensity;
        f32 dirDarkenIntensity;
        std::array<StringRef, 6> textures;
        std::array<u8, 6> frameCounts;
        std::array<f32, 16> unkFloats;
        std::array<i32, 4> unkInts;

    public:
        static inline std::string NAME = "LiquidTypeRecord";
        static constexpr u32 NAME_HASH = 758317908;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "name", ::ClientDB::FieldType::StringRef }, { "flags", ::ClientDB::FieldType::u16 }, { "lightID", ::ClientDB::FieldType::u16 }, { "soundBank", ::ClientDB::FieldType::u8 }, { "soundID", ::ClientDB::FieldType::u16 }, { "materialID", ::ClientDB::FieldType::u8 }, { "particleScale", ::ClientDB::FieldType::u8 }, { "particleMovement", ::ClientDB::FieldType::u8 }, { "particleTextureSlot", ::ClientDB::FieldType::u8 }, { "minimapColor", ::ClientDB::FieldType::u32 }, { "maxDarkenDepth", ::ClientDB::FieldType::u32 }, { "fogDarkenIntensity", ::ClientDB::FieldType::f32 }, { "ambDarkenIntensity", ::ClientDB::FieldType::f32 }, { "dirDarkenIntensity", ::ClientDB::FieldType::f32 }, { "textures", ::ClientDB::FieldType::StringRef, 6 }, { "frameCounts", ::ClientDB::FieldType::u8, 6 }, { "unkFloats", ::ClientDB::FieldType::f32, 16 }, { "unkInts", ::ClientDB::FieldType::i32, 4 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(name);
            failed |= !buffer->PutU16(flags);
            failed |= !buffer->PutU16(lightID);
            failed |= !buffer->PutU8(soundBank);
            failed |= !buffer->PutU16(soundID);
            failed |= !buffer->PutU8(materialID);
            failed |= !buffer->PutU8(particleScale);
            failed |= !buffer->PutU8(particleMovement);
            failed |= !buffer->PutU8(particleTextureSlot);
            failed |= !buffer->PutU32(minimapColor);
            failed |= !buffer->PutU32(maxDarkenDepth);
            failed |= !buffer->PutF32(fogDarkenIntensity);
            failed |= !buffer->PutF32(ambDarkenIntensity);
            failed |= !buffer->PutF32(dirDarkenIntensity);
            failed |= !buffer->PutBytes(textures.data(), textures.size());
            failed |= !buffer->PutBytes(frameCounts.data(), frameCounts.size());
            failed |= !buffer->PutBytes(unkFloats.data(), unkFloats.size());
            failed |= !buffer->PutBytes(unkInts.data(), unkInts.size());

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(name);
            failed |= !buffer->GetU16(flags);
            failed |= !buffer->GetU16(lightID);
            failed |= !buffer->GetU8(soundBank);
            failed |= !buffer->GetU16(soundID);
            failed |= !buffer->GetU8(materialID);
            failed |= !buffer->GetU8(particleScale);
            failed |= !buffer->GetU8(particleMovement);
            failed |= !buffer->GetU8(particleTextureSlot);
            failed |= !buffer->GetU32(minimapColor);
            failed |= !buffer->GetU32(maxDarkenDepth);
            failed |= !buffer->GetF32(fogDarkenIntensity);
            failed |= !buffer->GetF32(ambDarkenIntensity);
            failed |= !buffer->GetF32(dirDarkenIntensity);
            failed |= !buffer->GetBytes(textures.data(), 6 * sizeof(StringRef));
            failed |= !buffer->GetBytes(frameCounts.data(), 6 * sizeof(u8));
            failed |= !buffer->GetBytes(unkFloats.data(), 16 * sizeof(f32));
            failed |= !buffer->GetBytes(unkInts.data(), 4 * sizeof(i32));

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // name
            result += 2; // flags
            result += 2; // lightID
            result += 1; // soundBank
            result += 2; // soundID
            result += 1; // materialID
            result += 1; // particleScale
            result += 1; // particleMovement
            result += 1; // particleTextureSlot
            result += 4; // minimapColor
            result += 4; // maxDarkenDepth
            result += 4; // fogDarkenIntensity
            result += 4; // ambDarkenIntensity
            result += 4; // dirDarkenIntensity
            result += 6 * sizeof(StringRef); // textures
            result += 6 * sizeof(u8); // frameCounts
            result += 16 * sizeof(f32); // unkFloats
            result += 4 * sizeof(i32); // unkInts

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "name : " + std::to_string(name) + ", ";
            tmp += "flags : " + std::to_string(flags) + ", ";
            tmp += "lightID : " + std::to_string(lightID) + ", ";
            tmp += "soundBank : " + std::to_string(soundBank) + ", ";
            tmp += "soundID : " + std::to_string(soundID) + ", ";
            tmp += "materialID : " + std::to_string(materialID) + ", ";
            tmp += "particleScale : " + std::to_string(particleScale) + ", ";
            tmp += "particleMovement : " + std::to_string(particleMovement) + ", ";
            tmp += "particleTextureSlot : " + std::to_string(particleTextureSlot) + ", ";
            tmp += "minimapColor : " + std::to_string(minimapColor) + ", ";
            tmp += "maxDarkenDepth : " + std::to_string(maxDarkenDepth) + ", ";
            tmp += "fogDarkenIntensity : " + std::to_string(fogDarkenIntensity) + ", ";
            tmp += "ambDarkenIntensity : " + std::to_string(ambDarkenIntensity) + ", ";
            tmp += "dirDarkenIntensity : " + std::to_string(dirDarkenIntensity) + ", ";
            tmp += std::string("textures : Arrays are unsupported") + ", ";
            tmp += std::string("frameCounts : Arrays are unsupported") + ", ";
            tmp += std::string("unkFloats : Arrays are unsupported") + ", ";
            tmp += std::string("unkInts : Arrays are unsupported");

            return tmp;
        }
    };

    struct LiquidMaterialRecord
    {
    public:
        u8 flags;
        u8 liquidVertexFormat;

    public:
        static inline std::string NAME = "LiquidMaterialRecord";
        static constexpr u32 NAME_HASH = 1192870151;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "flags", ::ClientDB::FieldType::u8 }, { "liquidVertexFormat", ::ClientDB::FieldType::u8 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU8(flags);
            failed |= !buffer->PutU8(liquidVertexFormat);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU8(flags);
            failed |= !buffer->GetU8(liquidVertexFormat);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 1; // flags
            result += 1; // liquidVertexFormat

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "flags : " + std::to_string(flags) + ", ";
            tmp += "liquidVertexFormat : " + std::to_string(liquidVertexFormat);

            return tmp;
        }
    };

    struct LiquidObjectRecord
    {
    public:
        u16 liquidTypeID;
        u8 fishable;

    public:
        static inline std::string NAME = "LiquidObjectRecord";
        static constexpr u32 NAME_HASH = 2367557013;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "liquidTypeID", ::ClientDB::FieldType::u16 }, { "fishable", ::ClientDB::FieldType::u8 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU16(liquidTypeID);
            failed |= !buffer->PutU8(fishable);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU16(liquidTypeID);
            failed |= !buffer->GetU8(fishable);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 2; // liquidTypeID
            result += 1; // fishable

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "liquidTypeID : " + std::to_string(liquidTypeID) + ", ";
            tmp += "fishable : " + std::to_string(fishable);

            return tmp;
        }
    };

    struct MapRecord
    {
    public:
        StringRef nameInternal;
        StringRef name;
        u32 flags;
        u8 instanceType;
        u8 expansionID;
        u16 maxPlayers;

    public:
        static inline std::string NAME = "MapRecord";
        static constexpr u32 NAME_HASH = 1200716880;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "nameInternal", ::ClientDB::FieldType::StringRef }, { "name", ::ClientDB::FieldType::StringRef }, { "flags", ::ClientDB::FieldType::u32 }, { "instanceType", ::ClientDB::FieldType::u8 }, { "expansionID", ::ClientDB::FieldType::u8 }, { "maxPlayers", ::ClientDB::FieldType::u16 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(nameInternal);
            failed |= !buffer->PutU32(name);
            failed |= !buffer->PutU32(flags);
            failed |= !buffer->PutU8(instanceType);
            failed |= !buffer->PutU8(expansionID);
            failed |= !buffer->PutU16(maxPlayers);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(nameInternal);
            failed |= !buffer->GetU32(name);
            failed |= !buffer->GetU32(flags);
            failed |= !buffer->GetU8(instanceType);
            failed |= !buffer->GetU8(expansionID);
            failed |= !buffer->GetU16(maxPlayers);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // nameInternal
            result += 4; // name
            result += 4; // flags
            result += 1; // instanceType
            result += 1; // expansionID
            result += 2; // maxPlayers

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "nameInternal : " + std::to_string(nameInternal) + ", ";
            tmp += "name : " + std::to_string(name) + ", ";
            tmp += "flags : " + std::to_string(flags) + ", ";
            tmp += "instanceType : " + std::to_string(instanceType) + ", ";
            tmp += "expansionID : " + std::to_string(expansionID) + ", ";
            tmp += "maxPlayers : " + std::to_string(maxPlayers);

            return tmp;
        }
    };

    struct SpellRecord
    {
    public:
        StringRef name;
        StringRef description;
        StringRef auraDescription;
        u32 iconID;
        f32 castTime;
        f32 cooldown;
        f32 duration;

    public:
        static inline std::string NAME = "SpellRecord";
        static constexpr u32 NAME_HASH = 1345691144;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "name", ::ClientDB::FieldType::StringRef }, { "description", ::ClientDB::FieldType::StringRef }, { "auraDescription", ::ClientDB::FieldType::StringRef }, { "iconID", ::ClientDB::FieldType::u32 }, { "castTime", ::ClientDB::FieldType::f32 }, { "cooldown", ::ClientDB::FieldType::f32 }, { "duration", ::ClientDB::FieldType::f32 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(name);
            failed |= !buffer->PutU32(description);
            failed |= !buffer->PutU32(auraDescription);
            failed |= !buffer->PutU32(iconID);
            failed |= !buffer->PutF32(castTime);
            failed |= !buffer->PutF32(cooldown);
            failed |= !buffer->PutF32(duration);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(name);
            failed |= !buffer->GetU32(description);
            failed |= !buffer->GetU32(auraDescription);
            failed |= !buffer->GetU32(iconID);
            failed |= !buffer->GetF32(castTime);
            failed |= !buffer->GetF32(cooldown);
            failed |= !buffer->GetF32(duration);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // name
            result += 4; // description
            result += 4; // auraDescription
            result += 4; // iconID
            result += 4; // castTime
            result += 4; // cooldown
            result += 4; // duration

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "name : " + std::to_string(name) + ", ";
            tmp += "description : " + std::to_string(description) + ", ";
            tmp += "auraDescription : " + std::to_string(auraDescription) + ", ";
            tmp += "iconID : " + std::to_string(iconID) + ", ";
            tmp += "castTime : " + std::to_string(castTime) + ", ";
            tmp += "cooldown : " + std::to_string(cooldown) + ", ";
            tmp += "duration : " + std::to_string(duration);

            return tmp;
        }
    };

    struct SpellEffectsRecord
    {
    public:
        u32 spellID;
        u8 effectPriority;
        u8 effectType;
        std::array<i32, 3> effectValues;
        std::array<i32, 3> effectMiscValues;

    public:
        static inline std::string NAME = "SpellEffectsRecord";
        static constexpr u32 NAME_HASH = 2263295976;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "spellID", ::ClientDB::FieldType::u32 }, { "effectPriority", ::ClientDB::FieldType::u8 }, { "effectType", ::ClientDB::FieldType::u8 }, { "effectValues", ::ClientDB::FieldType::i32, 3 }, { "effectMiscValues", ::ClientDB::FieldType::i32, 3 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(spellID);
            failed |= !buffer->PutU8(effectPriority);
            failed |= !buffer->PutU8(effectType);
            failed |= !buffer->PutBytes(effectValues.data(), effectValues.size());
            failed |= !buffer->PutBytes(effectMiscValues.data(), effectMiscValues.size());

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(spellID);
            failed |= !buffer->GetU8(effectPriority);
            failed |= !buffer->GetU8(effectType);
            failed |= !buffer->GetBytes(effectValues.data(), 3 * sizeof(i32));
            failed |= !buffer->GetBytes(effectMiscValues.data(), 3 * sizeof(i32));

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // spellID
            result += 1; // effectPriority
            result += 1; // effectType
            result += 3 * sizeof(i32); // effectValues
            result += 3 * sizeof(i32); // effectMiscValues

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "spellID : " + std::to_string(spellID) + ", ";
            tmp += "effectPriority : " + std::to_string(effectPriority) + ", ";
            tmp += "effectType : " + std::to_string(effectType) + ", ";
            tmp += std::string("effectValues : Arrays are unsupported") + ", ";
            tmp += std::string("effectMiscValues : Arrays are unsupported");

            return tmp;
        }
    };

    struct SpellProcDataRecord
    {
    public:
        u32 phaseMask;
        u64 typeMask;
        u64 hitMask;
        u64 flags;
        f32 procsPerMinute;
        f32 chanceToProc;
        u32 internalCooldownMS;
        i32 charges;

    public:
        static inline std::string NAME = "SpellProcDataRecord";
        static constexpr u32 NAME_HASH = 2445328060;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "phaseMask", ::ClientDB::FieldType::u32 }, { "typeMask", ::ClientDB::FieldType::u64 }, { "hitMask", ::ClientDB::FieldType::u64 }, { "flags", ::ClientDB::FieldType::u64 }, { "procsPerMinute", ::ClientDB::FieldType::f32 }, { "chanceToProc", ::ClientDB::FieldType::f32 }, { "internalCooldownMS", ::ClientDB::FieldType::u32 }, { "charges", ::ClientDB::FieldType::i32 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(phaseMask);
            failed |= !buffer->PutU64(typeMask);
            failed |= !buffer->PutU64(hitMask);
            failed |= !buffer->PutU64(flags);
            failed |= !buffer->PutF32(procsPerMinute);
            failed |= !buffer->PutF32(chanceToProc);
            failed |= !buffer->PutU32(internalCooldownMS);
            failed |= !buffer->PutI32(charges);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(phaseMask);
            failed |= !buffer->GetU64(typeMask);
            failed |= !buffer->GetU64(hitMask);
            failed |= !buffer->GetU64(flags);
            failed |= !buffer->GetF32(procsPerMinute);
            failed |= !buffer->GetF32(chanceToProc);
            failed |= !buffer->GetU32(internalCooldownMS);
            failed |= !buffer->GetI32(charges);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // phaseMask
            result += 8; // typeMask
            result += 8; // hitMask
            result += 8; // flags
            result += 4; // procsPerMinute
            result += 4; // chanceToProc
            result += 4; // internalCooldownMS
            result += 4; // charges

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "phaseMask : " + std::to_string(phaseMask) + ", ";
            tmp += "typeMask : " + std::to_string(typeMask) + ", ";
            tmp += "hitMask : " + std::to_string(hitMask) + ", ";
            tmp += "flags : " + std::to_string(flags) + ", ";
            tmp += "procsPerMinute : " + std::to_string(procsPerMinute) + ", ";
            tmp += "chanceToProc : " + std::to_string(chanceToProc) + ", ";
            tmp += "internalCooldownMS : " + std::to_string(internalCooldownMS) + ", ";
            tmp += "charges : " + std::to_string(charges);

            return tmp;
        }
    };

    struct SpellProcLinkRecord
    {
    public:
        u32 spellID;
        u64 effectMask;
        u32 procDataID;

    public:
        static inline std::string NAME = "SpellProcLinkRecord";
        static constexpr u32 NAME_HASH = 1578347552;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "spellID", ::ClientDB::FieldType::u32 }, { "effectMask", ::ClientDB::FieldType::u64 }, { "procDataID", ::ClientDB::FieldType::u32 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(spellID);
            failed |= !buffer->PutU64(effectMask);
            failed |= !buffer->PutU32(procDataID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(spellID);
            failed |= !buffer->GetU64(effectMask);
            failed |= !buffer->GetU32(procDataID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // spellID
            result += 8; // effectMask
            result += 4; // procDataID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "spellID : " + std::to_string(spellID) + ", ";
            tmp += "effectMask : " + std::to_string(effectMask) + ", ";
            tmp += "procDataID : " + std::to_string(procDataID);

            return tmp;
        }
    };

    struct UnitRaceRecord
    {
    public:
        StringRef prefix;
        StringRef nameInternal;
        StringRef name;
        u8 flags;
        u32 factionID;
        u32 maleDisplayID;
        u32 femaleDisplayID;

    public:
        static inline std::string NAME = "UnitRaceRecord";
        static constexpr u32 NAME_HASH = 309613293;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "prefix", ::ClientDB::FieldType::StringRef }, { "nameInternal", ::ClientDB::FieldType::StringRef }, { "name", ::ClientDB::FieldType::StringRef }, { "flags", ::ClientDB::FieldType::u8 }, { "factionID", ::ClientDB::FieldType::u32 }, { "maleDisplayID", ::ClientDB::FieldType::u32 }, { "femaleDisplayID", ::ClientDB::FieldType::u32 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(prefix);
            failed |= !buffer->PutU32(nameInternal);
            failed |= !buffer->PutU32(name);
            failed |= !buffer->PutU8(flags);
            failed |= !buffer->PutU32(factionID);
            failed |= !buffer->PutU32(maleDisplayID);
            failed |= !buffer->PutU32(femaleDisplayID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(prefix);
            failed |= !buffer->GetU32(nameInternal);
            failed |= !buffer->GetU32(name);
            failed |= !buffer->GetU8(flags);
            failed |= !buffer->GetU32(factionID);
            failed |= !buffer->GetU32(maleDisplayID);
            failed |= !buffer->GetU32(femaleDisplayID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // prefix
            result += 4; // nameInternal
            result += 4; // name
            result += 1; // flags
            result += 4; // factionID
            result += 4; // maleDisplayID
            result += 4; // femaleDisplayID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "prefix : " + std::to_string(prefix) + ", ";
            tmp += "nameInternal : " + std::to_string(nameInternal) + ", ";
            tmp += "name : " + std::to_string(name) + ", ";
            tmp += "flags : " + std::to_string(flags) + ", ";
            tmp += "factionID : " + std::to_string(factionID) + ", ";
            tmp += "maleDisplayID : " + std::to_string(maleDisplayID) + ", ";
            tmp += "femaleDisplayID : " + std::to_string(femaleDisplayID);

            return tmp;
        }
    };

    struct UnitTextureSectionRecord
    {
    public:
        u8 section;
        uvec2 position;
        uvec2 size;

    public:
        static inline std::string NAME = "UnitTextureSectionRecord";
        static constexpr u32 NAME_HASH = 24564424;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "section", ::ClientDB::FieldType::u8 }, { "position", ::ClientDB::FieldType::uvec2 }, { "size", ::ClientDB::FieldType::uvec2 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU8(section);
            failed |= !buffer->Put(position);
            failed |= !buffer->Put(size);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU8(section);
            failed |= !buffer->Get(position);
            failed |= !buffer->Get(size);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 1; // section
            result += 8; // position
            result += 8; // size

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "section : " + std::to_string(section) + ", ";
            tmp += std::string("position : ") + "(X : " + std::to_string(position.x) + ", Y : " + std::to_string(position.y) + ")" + ", ";
            tmp += std::string("size : ") + "(X : " + std::to_string(size.x) + ", Y : " + std::to_string(size.y) + ")";

            return tmp;
        }
    };

    struct UnitCustomizationOptionRecord
    {
    public:
        StringRef name;
        u32 flags;

    public:
        static inline std::string NAME = "UnitCustomizationOptionRecord";
        static constexpr u32 NAME_HASH = 2375256404;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "name", ::ClientDB::FieldType::StringRef }, { "flags", ::ClientDB::FieldType::u32 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU32(name);
            failed |= !buffer->PutU32(flags);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU32(name);
            failed |= !buffer->GetU32(flags);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 4; // name
            result += 4; // flags

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "name : " + std::to_string(name) + ", ";
            tmp += "flags : " + std::to_string(flags);

            return tmp;
        }
    };

    struct UnitCustomizationMaterialRecord
    {
    public:
        u8 textureSection;
        u32 materialResourcesID;

    public:
        static inline std::string NAME = "UnitCustomizationMaterialRecord";
        static constexpr u32 NAME_HASH = 3027868548;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "textureSection", ::ClientDB::FieldType::u8 }, { "materialResourcesID", ::ClientDB::FieldType::u32 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU8(textureSection);
            failed |= !buffer->PutU32(materialResourcesID);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU8(textureSection);
            failed |= !buffer->GetU32(materialResourcesID);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 1; // textureSection
            result += 4; // materialResourcesID

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "textureSection : " + std::to_string(textureSection) + ", ";
            tmp += "materialResourcesID : " + std::to_string(materialResourcesID);

            return tmp;
        }
    };

    struct UnitCustomizationGeosetRecord
    {
    public:
        u8 geosetType;
        u8 geosetValue;

    public:
        static inline std::string NAME = "UnitCustomizationGeosetRecord";
        static constexpr u32 NAME_HASH = 3505026402;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "geosetType", ::ClientDB::FieldType::u8 }, { "geosetValue", ::ClientDB::FieldType::u8 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU8(geosetType);
            failed |= !buffer->PutU8(geosetValue);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU8(geosetType);
            failed |= !buffer->GetU8(geosetValue);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 1; // geosetType
            result += 1; // geosetValue

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "geosetType : " + std::to_string(geosetType) + ", ";
            tmp += "geosetValue : " + std::to_string(geosetValue);

            return tmp;
        }
    };

    struct UnitRaceCustomizationChoiceRecord
    {
    public:
        u8 raceID;
        u8 gender;
        u32 customizationOptionID;
        u16 customizationOptionData1;
        u16 customizationOptionData2;
        u32 customizationGeosetID;
        u32 customizationMaterialID1;
        u32 customizationMaterialID2;
        u32 customizationMaterialID3;

    public:
        static inline std::string NAME = "UnitRaceCustomizationChoiceRecord";
        static constexpr u32 NAME_HASH = 1958818467;
        static inline std::vector<::ClientDB::FieldInfo> FIELD_LIST = { { "raceID", ::ClientDB::FieldType::u8 }, { "gender", ::ClientDB::FieldType::u8 }, { "customizationOptionID", ::ClientDB::FieldType::u32 }, { "customizationOptionData1", ::ClientDB::FieldType::u16 }, { "customizationOptionData2", ::ClientDB::FieldType::u16 }, { "customizationGeosetID", ::ClientDB::FieldType::u32 }, { "customizationMaterialID1", ::ClientDB::FieldType::u32 }, { "customizationMaterialID2", ::ClientDB::FieldType::u32 }, { "customizationMaterialID3", ::ClientDB::FieldType::u32 } };

        bool Serialize(Bytebuffer* buffer) const
        {
            bool failed = false;

            failed |= !buffer->PutU8(raceID);
            failed |= !buffer->PutU8(gender);
            failed |= !buffer->PutU32(customizationOptionID);
            failed |= !buffer->PutU16(customizationOptionData1);
            failed |= !buffer->PutU16(customizationOptionData2);
            failed |= !buffer->PutU32(customizationGeosetID);
            failed |= !buffer->PutU32(customizationMaterialID1);
            failed |= !buffer->PutU32(customizationMaterialID2);
            failed |= !buffer->PutU32(customizationMaterialID3);

            return !failed;
        }
        bool Deserialize(Bytebuffer* buffer)
        {
            bool failed = false;

            failed |= !buffer->GetU8(raceID);
            failed |= !buffer->GetU8(gender);
            failed |= !buffer->GetU32(customizationOptionID);
            failed |= !buffer->GetU16(customizationOptionData1);
            failed |= !buffer->GetU16(customizationOptionData2);
            failed |= !buffer->GetU32(customizationGeosetID);
            failed |= !buffer->GetU32(customizationMaterialID1);
            failed |= !buffer->GetU32(customizationMaterialID2);
            failed |= !buffer->GetU32(customizationMaterialID3);

            return !failed;
        }
        u32 GetSerializedSize() const
        {
            u32 result = 0;

            result += 1; // raceID
            result += 1; // gender
            result += 4; // customizationOptionID
            result += 2; // customizationOptionData1
            result += 2; // customizationOptionData2
            result += 4; // customizationGeosetID
            result += 4; // customizationMaterialID1
            result += 4; // customizationMaterialID2
            result += 4; // customizationMaterialID3

            return result;
        }

        std::string ToString() const
        {
            std::string tmp = "";
            tmp.reserve(128);

            tmp += "raceID : " + std::to_string(raceID) + ", ";
            tmp += "gender : " + std::to_string(gender) + ", ";
            tmp += "customizationOptionID : " + std::to_string(customizationOptionID) + ", ";
            tmp += "customizationOptionData1 : " + std::to_string(customizationOptionData1) + ", ";
            tmp += "customizationOptionData2 : " + std::to_string(customizationOptionData2) + ", ";
            tmp += "customizationGeosetID : " + std::to_string(customizationGeosetID) + ", ";
            tmp += "customizationMaterialID1 : " + std::to_string(customizationMaterialID1) + ", ";
            tmp += "customizationMaterialID2 : " + std::to_string(customizationMaterialID2) + ", ";
            tmp += "customizationMaterialID3 : " + std::to_string(customizationMaterialID3);

            return tmp;
        }
    };
}
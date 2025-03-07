#pragma once
#include <Base/Types.h>
#include <Base/Container/StringTable.h>
#include <Base/Math/Color.h>
#include <Base/Math/Geometry.h>
#include <Base/Memory/Bytebuffer.h>

#include <limits>

namespace ClientDB
{
    typedef u32 StringRef;

    namespace Definitions
    {
        struct Map
        {
        public:
            StringRef name;
            StringRef internalName;
            u32 instanceType;
            u32 flags;
            u32 expansion;
            u32 maxPlayers;
        };

        struct LiquidObject
        {
        public:
            u16 liquidTypeID;
            u8 fishable;
            u8 reflection;
            f32 flowDirection;
            f32 flowSpeed;
        };

        struct LiquidType
        {
        public:
            u32 name;
            u32 textures[6];
            u16 flags;
            u16 lightID;
            u32 soundID;
            u32 spellID;
            f32 maxDarkenDepth;
            f32 fogDarkenIntensity;
            f32 ambDarkenIntensity;
            f32 dirDarkenIntensity;
            f32 particleScale;
            u8 materialID;
            u8 soundBank;
            u8 particleMovement;
            u8 particleTexSlots;
            u32 minimapStaticCol;
            u32 frameCountTextures[6];
            u32 colors[2];
            f32 unkFloats[16];
            u32 unkInts[4];
            u32 coefficients[4];
        };

        struct LiquidMaterial
        {
        public:
            u8 flags;
            u8 liquidVertexFormat;
        };

        struct CinematicCamera
        {
        public:
            vec3 endPosition;
            u32 soundID;
            f32 rotation;
            u32 cameraPath;
        };

        struct CinematicSequence
        {
        public:
            u32 soundID;
            u16 cameraIDs[8];
        };

        struct AnimationData
        {
        public:
            struct Flags
            {
                u32 UsedForEmote : 1 = 0;
                u32 UsedForSpell : 1 = 0;
                u32 IsPierceAnim : 1 = 0;
                u32 HideWeapons : 1 = 0;
                u32 FallbackPlaysReverse : 1 = 0;
                u32 FallbackHoldsEnd : 1 = 0;
                u32 Unused0x40 : 1 = 0;
                u32 FallbackToVariationZero : 1 = 0;
                u32 Unused0x100 : 1 = 0;
                u32 Unused0x200 : 1 = 0;
                u32 Unused0x400 : 1 = 0;
                u32 MoveWeaponsToSheath : 1 = 0;
                u32 MoveMeleeWeaponsToHand : 1 = 0;
                u32 ScaleToGround : 1 = 0;
                u32 ScaleToGroundRev : 1 = 0;
                u32 ScaleToGroundAlways : 1 = 0;
                u32 IsSplitBodyBehavior : 1 = 0;
                u32 IsBowWeaponBehavior : 1 = 0;
                u32 IsRifleWeaponBehavior : 1 = 0;
                u32 IsThrownWeaponBehavior : 1 = 0;
                u32 IsDeathBehavior : 1 = 0;
                u32 IsMeleeCombatBehavior : 1 = 0;
                u32 IsSpecialCombatBehavior : 1 = 0;
                u32 IsWoundBehavior : 1 = 0;
                u32 IsUnarmedBehavior : 1 = 0;
                u32 UseMountedNameplatePos : 1 = 0;
                u32 FlipSpearWeapons180Deg : 1 = 0;
                u32 Unused0x8000000 : 1 = 0;
                u32 Unused0x10000000 : 1 = 0;
                u32 IsSpellCombatBehavior : 1 = 0;
                u32 BrewmasterSheathe : 1 = 0;
                u32 Unused0x80000000 : 1 = 0;
            };

        public:
            u16 fallback;
            u8 behaviorTier;
            u32 behaviorID;
            Flags flags[2];
        };

        enum class DisplayInfoType
        {
            Creature = 0,
            GameObject = 1,
            Item = 2
        };

        struct CreatureDisplayInfo
        {
        public:
            u16 modelID;
            u16 soundID;
            u8 flags;
            i8 gender;
            i8 sizeClass;
            u8 bloodID;
            i8 unarmedWeaponType;
            u8 creatureModelAlpha;
            f32 creatureModelScale;
            i32 extendedDisplayInfoID;
            u16 npcSoundID;
            u16 particleColorID;
            i32 portraitCreatureDisplayInfoID;
            i32 portraitTextureFileDataID;
            u16 objectEffectPackageID;
            u16 animReplacementSetID;
            i32 stateSpellVisualKitID;
            f32 playerOverrideScale;
            f32 petInstanceScale;
            i32 mountPoofSpellVisualKitID;
            i32 dissolveEffectID;
            i32 dissolveOutEffectID;
            u32 textureVariations[4];
        };

        struct CreatureDisplayInfoExtra
        {
        public:
            i8 displayRaceID;
            i8 displaySexID;
            i8 displayClassID;
            i8 skinID;
            i8 faceID;
            i8 hairStyleID;
            i8 hairColorID;
            i8 facialHairID;
            i8 flags;
            u8 customDisplayOptions[3];
            u32 bakedTextureHash;
        };

        struct CreatureModelData
        {
        public:
            Geometry::AABoundingBox boundingBox;
            u32 flags;
            u32 modelHash;
            u32 bloodID;
            u32 footprintTextureID;
            f32 footprintTextureLength;
            f32 footprintTextureWidth;
            f32 footprintParticleScale;
            u32 foleyMaterialID;
            u32 footstepCameraEffectID;
            u32 deathThudCameraEffectID;
            u32 soundID;
            u32 sizeClass;
            f32 collisionWidth;
            f32 collisionHeight;
            f32 worldEffectScale;
            u32 creatureGeosetDataID;
            f32 hoverHeight;
            f32 attachedEffectScale;
            f32 modelScale;
            f32 missileCollisionRadius;
            f32 missileCollisionPush;
            f32 missileCollisionRaise;
            f32 mountHeight;
            f32 overrideLootEffectScale;
            f32 overrideNameScale;
            f32 overrideSelectionRadius;
            f32 tamedPetBaseScale;
        };

        struct ItemDisplayMaterialResources
        {
        public:
            u32 displayID;
            u8 componentSection;
            u32 materialResourcesID;
        };

        struct ItemDisplayModelMaterialResources
        {
        public:
            u32 displayID;
            u8 modelIndex;
            u8 textureType;
            u32 materialResourcesID;
        };

        struct ItemDisplayInfo
        {
        public:
            u32 itemVisual;
            u32 particleColorID;
            u32 itemRangedDisplayInfoID;
            u32 overrideSwooshSoundKitID;
            u32 sheatheTransformMatrixID;
            u32 stateSpellVisualKitID;
            u32 sheathedSpellVisualKitID;
            u32 unsheathedSpellVisualKitID;
            u32 flags;
            u32 modelResourcesID[2];
            u32 materialResourcesID[2];
            u32 modelType[2];
            u32 geosetGroup[6];
            u32 geosetAttachmentGroup[6];
            u32 geosetHelmetVis[2];
        };

        struct ModelFileData
        {
        public:
            u8 flags;
            u32 modelPath;
            u32 modelResourcesID;
        };

        struct TextureFileData
        {
        public:
            u32 textureHash;
            u32 materialResourcesID;
        };

        struct Light
        {
        public:
            u16 mapID;
            vec3 position;
            vec2 fallOff;
            u16 lightParamsID[8];
        };

        struct LightParam
        {
        public:
            struct Flags
            {
                u8 highlightSky : 1 = 0;
                u8 : 7;
            };

        public:
            Flags flags;
            u16 lightSkyboxID;
            f32 glow;
            f32 waterShallowAlpha;
            f32 waterDeepAlpha;
            f32 oceanShallowAlpha;
            f32 oceanDeepAlpha;
        };

        struct LightData
        {
        public:
            u16 lightParamID;
            u32 timestamp;
            u32 diffuseColor;
            u32 ambientColor;
            u32 skyTopColor;
            u32 skyMiddleColor;
            u32 skyBand1Color;
            u32 skyBand2Color;
            u32 skySmogColor;
            u32 skyFogColor;
            u32 sunColor;
            u32 sunFogColor;
            f32 sunFogStrength;
            u32 cloudSunColor;
            u32 cloudEmissiveColor;
            u32 cloudLayer1AmbientColor;
            u32 cloudLayer2AmbientColor;
            u32 oceanShallowColor;
            u32 oceanDeepColor;
            u32 riverShallowColor;
            u32 riverDeepColor;
            u32 shadowColor;
            f32 fogEnd;
            f32 fogScaler;
            f32 fogDensity;
            f32 sunFogAngle;
            f32 cloudDensity;
            u32 fogHeightColor;
            u32 fogEndColor;
            u32 fogEndHeightColor;
        };

        struct LightSkybox
        {
        public:
            u32 modelHash;
        };
    }
}
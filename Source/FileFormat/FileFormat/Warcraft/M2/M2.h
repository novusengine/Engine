#pragma once
#include "FileFormat/Shared.h"

#include <Base/Types.h>
#include <Base/Platform.h>

#include <robinhood/robinhood.h>

namespace M2
{
	template <typename T>
	struct M2Array
	{
		u32 size = 0;
		u32 offset = 0;

		void Init(u32 md21Offset)
		{
			offset += md21Offset * (offset > 0);
		}

		T* Get(const std::shared_ptr<Bytebuffer>& buffer) const
		{
			return GetElement(buffer, 0);
		}

		T* GetElement(const std::shared_ptr<Bytebuffer>& buffer, u32 index) const
		{
			assert(index < size);
			return reinterpret_cast<T*>(&buffer->GetDataPointer()[offset + (index * sizeof(T))]);
		}
	};
	struct M2TrackBase
	{
		u16 interpolationType = 0;
		i16 globalSequence = 0;

		M2Array<M2Array<u32>> timestamps = { };
	};
	template <typename T>
	struct M2Track : M2TrackBase
	{
		M2Array<M2Array<T>> values = { };
	};

	template <typename T>
	struct FBlock
	{
		M2Array<u16> timestamps = { };
		M2Array<T> values = { };
	};
	template<typename Base, size_t integerBits, size_t decimalBits>
	struct FixedPoint
	{
		static const float constexpr factor = integerBits ? (1 << decimalBits) : (1 << (decimalBits + 1)) - 1;

		union
		{
			struct
			{
				Base integerAndDecimal : integerBits + decimalBits;
				Base sign : 1;
			};

			Base raw;
		};

		constexpr operator f32() const
		{
			return (sign ? -1.0f : 1.0f) * integerAndDecimal / factor;
		}
		constexpr FixedPoint(float x) : sign(x < 0.f), integerAndDecimal(glm::abs(x)* factor) { }
	};
	template<typename Base, size_t integerBits, size_t decimalBits>
	struct Vec2FixedPoint
	{
		using fp = FixedPoint<Base, integerBits, decimalBits>;

		fp x;
		fp y;
	};
	struct M2Range
	{
		u32 minimum = 0;
		u32 maximum = 0;
	};
	struct M2Box
	{
		vec3 min = { };
		vec3 max = { };
	};
	struct M2Bounds
	{
		M2Box aabb = { };
		f32 radius = 0.0f;
	};
	struct M2CompressedQuaternion
	{
		M2CompressedQuaternion() { }
		M2CompressedQuaternion(i16 inX, i16 inY, i16 inZ, i16 inW) : x(inX), y(inY), z(inZ), w(inW) { }

		i16 x = 0;
		i16 y = 0;
		i16 z = 0;
		i16 w = 0;

		quat ToQuat()
		{
			quat quat;
			quat.x = static_cast<f32>(x < 0 ? x + 32768 : x - 32767) / 32767.f;
			quat.y = static_cast<f32>(y < 0 ? y + 32768 : y - 32767) / 32767.f;
			quat.z = static_cast<f32>(z < 0 ? z + 32768 : z - 32767) / 32767.f;
			quat.w = static_cast<f32>(w < 0 ? w + 32768 : w - 32767) / 32767.f;

			return quat;
		}
	};

	struct M2SkinSelection
	{
		u16 skinSectionID = 0;
		u16 level = 0;
		u16 vertexStart = 0;
		u16 vertexCount = 0;
		u16 indexStart = 0;
		u16 indexCount = 0;
		u16 boneCount = 0;
		u16 boneComboIndex = 0;
		u16 boneInfluences = 0;

		u16 centerBoneIndex = 0;
		vec3 centerPosition = { };
		vec3 sortCenterPosition = { };
		f32 sortRadius = 0.0f;
	};
	struct M2Batch
	{
		u8 flags = 0;
		i8 priorityPlane = 0;
		i16 shaderID = 0;
		u16 skinSectionIndex = 0;
		u16 geosetIndex = 0;
		u16 colorIndex = 0;
		u16 materialIndex = 0;
		u16 materialLayer = 0;
		u16 textureCount = 0;
		u16 textureLookupID = 0;
		u16 textureUnitLookupID = 0;
		u16 textureTransparencyLookupID = 0;
		u16 textureUVAnimationLookupID = 0;
	};
	struct M2Skin
	{
	public:
		u32 token;

		M2Array<u16> vertices = { };
		M2Array<u16> indices = { };
		M2Array<u8vec4> boneIndices = { };
		M2Array<M2SkinSelection> subMeshes = { };
		M2Array<M2Batch> batches = { };
		u32 boneCountMax = { };
	};

	struct M2Particle
	{
		u32 id = 0;
		u32 flags = 0;

		vec3 position = { };
		u16 bone = 0;

		union
		{
			u16 textureID;

			struct
			{
				u16 texture0 : 5;
				u16 texture1 : 5;
				u16 texture2 : 5;
				u16 : 1;
			};
		};

		M2Array<char> unused1 = { };
		M2Array<char> unused2 = { };

		u8 blendingType = 0;
		u8 emitterType = 0;
		u16 particleColorIndex = 0;

		FixedPoint<u8, 2, 5> multiTextureUV[2]; // For Texture1 & Texture2

		u16 textureTileRotation = 0;
		u16 textureDImensionsRows = 0;
		u16 textureDImensionsColumns = 0;

		M2Track<f32> emissionSpeed = { };
		M2Track<f32> speedVariation = { };
		M2Track<f32> verticalRange = { };
		M2Track<f32> horizontalRange = { };
		M2Track<f32> gravity = { };
		M2Track<f32> lifespan = { };
		f32 lifespanVariation = 0.0f;
		M2Track<f32> emissionRate = { };
		f32 emissionRateVariation = 0.0f;
		M2Track<f32> emissionAreaLength = { };
		M2Track<f32> emissionAreaWidth = { };
		M2Track<f32> zSource = { };

		FBlock<vec3> colorTrack = { };
		FBlock<i16> alphaTrack = { }; // This is referred to as a "Fixed16" basically divide by 0x7FFF to get the f32 value
		FBlock<vec2> scaleTrack = { };
		vec2 scaleVariation = { };
		FBlock<u16> headCellTrack = { };
		FBlock<u16> tailCellTrack = { };

		f32 tailLength = 0.0f;
		f32 twinkleSpeed = 0.0f;
		f32 twinklePercent = 0.0f;
		f32 twinkleScaleMin = 0.0f;
		f32 twinkleScaleMax = 0.0f;
		f32 burstMultiplier = 0.0f;
		f32 drag = 0.0f;
		f32 baseSpin = 0.0f;
		f32 baseSpinVariation = 0.0f;
		f32 spin = 0.0f;
		f32 spinVariation = 0.0f;

		M2Box tumble = { };
		vec3 windVector = { };
		f32 windTime = 0.0f;

		f32 followSpeed1 = 0.0f;
		f32 followScale1 = 0.0f;
		f32 followSpeed2 = 0.0f;
		f32 followScale2 = 0.0f;

		M2Array<vec3> splinePoints = { };
		M2Track<u8> enabledIn = { };

		Vec2FixedPoint<u16, 6, 9> multiTextureParam0[2]; // For Texture1 & Texture2
		Vec2FixedPoint<u16, 6, 9> multiTextureParam1[2]; // For Texture1 & Texture2
	};
	struct M2Ribbon
	{
		u32 id = 0;
		u32 boneIndex = 0;
		vec3 position = { };

		M2Array<u16> textureIndices = { };
		M2Array<u16> materialIndices = { };
		M2Track<vec3> colorTrack = { };
		M2Track<i16> alphaTrack = { }; // This is referred to as a "Fixed16" basically divide by 0x7FFF to get the f32 value
		M2Track<f32> heightAboveTrack = { };
		M2Track<f32> heightBelowTrack = { };

		f32 edgesPerSecond = 0.0f;
		f32 edgeLifeTime = 0.0f;
		f32 gravity = 0.0f;
		u16 textureRows = 0;
		u16 textureColumns = 0;

		M2Track<u16> texSlotTrack = { };
		M2Track<u8> visibilityTrack = { };

		i16 priorityPlane = 0;
		u16 padding = 0;
	};
	struct M2Camera
	{
		u32 type = 0;
		f32 farClip = 0.0f;
		f32 nearClip = 0.0f;

		M2Track<SplineKey<vec3>> positions = { };
		vec3 positionBase = { };
		M2Track<SplineKey<vec3>> targetPosition = { };
		vec3 targetPositionBase = { };

		M2Track<SplineKey<f32>> roll = { };
		M2Track<SplineKey<f32>> fov = { };
	};
	struct M2Light
	{
		u16 type = 0; // 0 == Directional, 1 == Point Light (Not used outside of login screen in 3.3.5)
		i16 bone = 0; // -1 if not attached to a bone
		vec3 position = { }; // Relative to bone

		M2Track<vec3> ambientColor = { };
		M2Track<f32> ambientIntensity = { };
		M2Track<vec3> diffuseColor = { };
		M2Track<f32> diffuseIntensity = { };
		M2Track<f32> attenuationStart = { };
		M2Track<f32> attenuationEnd = { };
		M2Track<u8> visibility = { };
	};
	struct M2Event
	{
		u32 identifier = 0;
		u32 data = 0;
		u32 bone = 0;
		vec3 position = { };
		M2TrackBase enabled = { };
	};
	struct M2Attachment
	{
		u32 id = 0;
		u16 bone = 0;
		u16 unk = 0;
		vec3 position = { };

		M2Track<u8> isAnimated = { };
	};
	struct M2Material
	{
	public:
		struct Flags
		{
			u16 unLit : 1;
			u16 unFogged : 1;
			u16 disableBackfaceCulling : 1;
			u16 depthTest : 1;
			u16 depthWrite : 1;
			u16 : 5;
		};

		enum class BlendingMode : u16
		{
			Opaque,
			AlphaKey,
			Alpha,
			NoAlphaAdd,
			Add,
			Mod,
			Mod2X,
			BlendAdd
		};

		Flags flags = { };
		BlendingMode blendingMode = { };
	};
	struct M2TextureTransform
	{
	public:
		M2Track<vec3> translation = { };
		M2Track<quat> rotation = { }; // Check https://wowdev.wiki/M2#Texture_Transforms
		M2Track<vec3> scale = { };
	};
	struct M2TextureWeight
	{
	public:
		M2Track<i16> weight = { }; // This is referred to as a "Fixed16" basically divide by 0x7FFF to get the f32 value
	};
	struct M2Texture
	{
	public:
		enum class Type : u32
		{
			None,
			Skin,
			ObjectSkin,
			WeaponBlade,
			WeaponHandle,
			Environment,
			CharacterHair,
			CharacterFacialHair,
			SkinExtra,
			UISkin,
			TaurenMane,
			MonsterSkin1,
			MonsterSkin2,
			MonsterSkin3,
			ItemIcon,
			GuildBackgroundColor,
			GuildEmblemColor,
			GuildEmblem,
			CharacterEyes,
			CharacterAccessory,
			CharacterSecondarySkin,
			CharacterSecondaryHair,
			CharacterSecondaryArmor
		};
		struct Flags
		{
			u32 wrapX : 1;
			u32 wrapY : 1;
		};

		Type type = { };
		Flags flags = { };

		M2Array<char> unused = { };
	};
	struct M2Color
	{
	public:
		M2Track<vec3> color = { }; // RGB
		M2Track<i16> alpha = { }; // (0) == Transparent, (0x7FFF) == Opaque. (Normally NonInterp) This is referred to as a "Fixed16" basically divide by 0x7FFF to get the f32 value
	};
	struct M2Vertex
	{
	public:
		vec3 position = { };
		u8 boneWeights[4] = { };
		u8 boneIndices[4] = { };
		vec3 normal = { };
		vec2 uvCords[2] = { };
	};
	struct M2Sequence
	{
	public:
		struct Flags
		{
			u32 : 1; // Sets 0x80 when loaded. (M2Init)
			u32 : 3;
			u32 : 1; // Set during CM2Shared::LoadLowPrioritySequence
			u32 HasEmbeddedAnimationData : 1; // If set the animation data is in the .m2 file, if not the data is in a .anim file
			u32 IsAlias : 1; // If set continue to go to the next Sequence to get the animation data)
			u32 BlendTransition : 1; // (This applies if set on either side of the transition) If set we lerp between the end -> start states, but only if end != start (Compare Bone Values)
			u32 : 1; // Sequence Stored in model?
			u32 : 2;
			u32 : 1; // Seen in Legion 24500 Models
		};

		u16 id = 0; // Animation ID (AnimationData.dbc)
		u16 variationID = 0; // Sub Animation ID (An id to indicate the variation index of this animation)
		u32 duration = 0; // Duration of animation in milliseconds.
		f32 moveSpeed = 0.0f; // The speed at which the model moves with.
		Flags flags = { };
		i16 frequency = 0; // Determines how often the animation is played.
		u16 padding = 0;
		M2Range repeatRange = { }; // (0,0) == No Repeat, (x, y) == Client picks random number within range
		u16 blendTimeIn = 0;
		u16 blendTimeOut = 0;
		M2Bounds bounds = { };
		i16 nextVariationID = 0; // Specifies the variation id for the next variation for this animation or (-1) for none.
		u16 nextAliasID = 0; // Specifies the id for the actual animation.
	};
	struct M2CompBone
	{
	public:
		struct Flags
		{
			u32 IgnoreParentTranslate : 1;
			u32 IgnoreParentScale : 1;
			u32 IgnoreParentRotation : 1;
			u32 SphericalBillboard : 1;
			u32 CylindricialBillboardLockX : 1;
			u32 CylindricialBillboardLockY : 1;
			u32 CylindricialBillboardLockZ : 1;
			u32 Unk0x80 : 1;
			u32 : 1;
			u32 Transformed : 1;
			u32 KinematicBone : 1; // MOP+ Allow physics to influence this bone
			u32 : 1;
			u32 HelmetAnimScaled : 1; // Set blend_modificator to helmetAnimScaling
			u32 SequenceID : 1; // The WowDev Wiki states (<= bfa+) this has something to do with the "parent_bone+submesh_id" being a sequence id
		};

		i32 keyBoneID = { }; // An index into the "Key-Bone Lookup Table"
		Flags flags = { };

		i16 parentBone = 0; // Parent Bone ID or (-1) for none
		u16 submeshID = 0; // Mesh part ID or uDistToParent?

		union
		{
			struct
			{
				u16 distToFurthDesc;
				u16 zRatioOfChain;
			} CompressData;

			u32 boneNameCRC = 0; // [Original Client] This is for debugging only
		};

		M2Track<vec3> translation = { };
		M2Track<M2CompressedQuaternion> rotation = { };
		M2Track<vec3> scale = { };

		vec3 pivot = { }; // The pivot point for the bone
	};

	struct BFID
	{
	public:
		std::vector<u32> boneFileIDs = { };
	};
	struct AFID
	{
	public:
		struct AnimationFileInfo
		{
			u16 animID = 0;
			u16 variationID = 0;
			u32 fileID = 0;
		};

		std::vector<AnimationFileInfo> data = { };
	};
	struct SFID
	{
	public:
		std::vector<u32> skinFileIDs = { };
		std::vector<u32> skinLodFileIDs = { };
	};
	struct TXID
	{
	public:
		std::vector<u32> textureFileIDs = { };
	};
	struct MD21
	{
	public:
		struct Flags
		{
			u32 TiltX : 1;
			u32 TiltY : 1;
			u32 : 1;

			u32 UseTextureCombinerCombos : 1;
			u32 : 1;

			u32 LoadPhysicsData : 1;
			u32 : 1;

			u32 Unk0x80 : 1;
			u32 Unk0x100 : 1;

			u32 HasNewParticleFormat : 1;
			u32 Unk0x400 : 1;
			u32 TextureTransformUseBoneSequences : 1;
		};

		u32 token = 0;
		u32 version = 0;
		M2Array<char> uniqueName = { };
		Flags flags = { };

		M2Array<u32> loopingSequenceTimestamps = { };
		M2Array<M2Sequence> sequences = { };
		M2Array<u16> sequenceIDToAnimationID = { };
		M2Array<M2CompBone> bones = { };
		M2Array<u16> keyBoneToBoneIndexList = { };

		M2Array<M2Vertex> vertices = { };

		u32 numSkinProfiles = 0;
		M2Array<M2Color> colors = { };
		M2Array<M2Texture> textures = { };
		M2Array<M2TextureWeight> textureWeights = { };
		M2Array<M2TextureTransform> textureTransforms = { };
		M2Array<u16> textureSwapTable = { };
		M2Array<M2Material> materials = { };
		M2Array<u16> boneCombinationList = { };
		M2Array<u16> textureCombinationList = { };
		M2Array<u16> textureUnitLookupList = { };
		M2Array<u16> textureTransparencyLookupList = { };
		M2Array<u16> textureUVAnimationLookupList = { };

		M2Bounds cullingAABBBounds = { };
		M2Bounds collisionAABBBounds = { };

		M2Array<u16> collisionIndices = { };
		M2Array<vec3> collisionVertices = { };
		M2Array<vec3> collisionNormals = { };

		M2Array<M2Attachment> attachments = { };
		M2Array<u16> attachmentLooukpList = { };

		M2Array<M2Event> events = { };
		M2Array<M2Light> lights = { };

		M2Array<M2Camera> cameras = { };
		M2Array<u16> cameraLookupList = { };

		M2Array<M2Ribbon> ribbonEmitters = { };
		M2Array<M2Particle> particleEmitters = { };

		M2Array<u16> textureCombinerCombos = { };
	};

	struct Layout
	{
	public:
		MD21 md21 = { };
		SFID sfid = { };
		AFID afid = { };
		BFID bfid = { };
		TXID txid = { };

		M2Skin skin = { };

	private:
		i8 GetVertexShaderID(i16 shaderID, u16 textureCount);
		i8 GetPixelShaderID(i16 shaderID, u16 textureCount);
	};
}

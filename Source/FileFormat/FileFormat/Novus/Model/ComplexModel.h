#pragma once
#include "FileFormat/Shared.h"
#include "FileFormat/Novus/FileHeader.h"

#include <Base/Types.h>
#include <Base/Platform.h>

#include <array>
#include <fstream>
#include <vector>

class Bytebuffer;

namespace M2
{
	struct Layout;
}

namespace Model
{
PRAGMA_NO_PADDING_START;
	struct MapObject;
	struct ComplexModel
	{
	public:
		static const u32 CURRENT_VERSION = 6;

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
			u32 : 12;

			// Custom Flags
			u32 IsConvertedMapObject : 1;
		};

		template <typename T>
		struct AnimationTrack
		{
		public:
			u32 sequenceID = 0;
			std::vector<u32> timestamps = { };
			std::vector<T> values = { };

		public:
			AnimationTrack() { }
			AnimationTrack(AnimationTrack<T>& other) : sequenceID(sequenceID)
			{
				size_t numTimestamps = other.timestamps.size();
				if (numTimestamps)
				{
					timestamps.resize(numTimestamps);
					memcpy(timestamps.data(), other.timestamps.data(), numTimestamps * sizeof(u32));
				}

				size_t numValues = other.values.size();
				if (numValues)
				{
					values.resize(numValues);

					if constexpr (std::is_trivially_copyable<T>::value)
					{
						memcpy(values.data(), other.values.data(), numValues * sizeof(T));
					}
					else
					{
						for (u32 i = 0; i < numValues; i++)
						{
							values[i] = other.values[i];
						}
					}
				}
			}
			AnimationTrack(AnimationTrack<T>&& other) : sequenceID(other.sequenceID), timestamps(std::move(other.timestamps)), values(std::move(other.values)) { }
			AnimationTrack<T>& operator=(const AnimationTrack<T>& other)
			{
				sequenceID = other.sequenceID;

				size_t numTimestamps = other.timestamps.size();
				if (numTimestamps)
				{
					timestamps.resize(numTimestamps);
					memcpy(timestamps.data(), other.timestamps.data(), numTimestamps * sizeof(u32));
				}

				size_t numValues = other.values.size();
				if (numValues)
				{
					values.resize(numValues);

					if constexpr (std::is_trivially_copyable<T>::value)
					{
						memcpy(values.data(), other.values.data(), numValues * sizeof(T));
					}
					else
					{
						for (u32 i = 0; i < numValues; i++)
						{
							values[i] = other.values[i];
						}
					}
				}

				return *this;
			}
		};

		enum class AnimationInterpolationType : u8
		{
			NONE,
			LINEAR,
			CUBIC_BEZIER_SPLINE, // Only used for M2SplineKey tracks
			CUBIC_HERMIT_SPLINE // Only used for M2SplineKey tracks
		};

		template <typename T>
		struct AnimationData
		{
		public:
			AnimationInterpolationType interpolationType = AnimationInterpolationType::NONE;
			bool isGlobalSequence = false;

			std::vector<AnimationTrack<T>> tracks;

		public:
			AnimationData() { }

			AnimationData(AnimationData<T>& other)
			{
				interpolationType = other.interpolationType;
				isGlobalSequence = other.isGlobalSequence;

				size_t numTracks = other.tracks.size();
				tracks.resize(numTracks);

				for (size_t i = 0; i < numTracks; i++)
				{
					tracks[i] = other.tracks[i];
				}
			}
			AnimationData(AnimationData<T>&& other) : interpolationType(other.interpolationType), isGlobalSequence(other.isGlobalSequence), tracks(std::move(other.tracks)) { }
			AnimationData<T>& operator=(const AnimationData<T>& other)
			{
				interpolationType = other.interpolationType;
				isGlobalSequence = other.isGlobalSequence;

				size_t numTracks = other.tracks.size();
				tracks.resize(numTracks);

				for (size_t i = 0; i < numTracks; i++)
				{
					tracks[i] = other.tracks[i];
				}

				return *this;
			}

			void Serialize(std::ofstream& stream) const
			{
				stream.write(reinterpret_cast<char const*>(&interpolationType), sizeof(AnimationInterpolationType));
				stream.write(reinterpret_cast<char const*>(&isGlobalSequence), sizeof(bool));

				u32 numTracks = static_cast<u32>(tracks.size());
				{
					stream.write(reinterpret_cast<char const*>(&numTracks), sizeof(u32));

					for (u32 i = 0; i < numTracks; i++)
					{
						const AnimationTrack<T>& track = tracks[i];

						stream.write(reinterpret_cast<char const*>(&track.sequenceID), sizeof(u32));

						u32 numTimestamps = static_cast<u32>(track.timestamps.size());
						{
							stream.write(reinterpret_cast<char const*>(&numTimestamps), sizeof(u32));
							stream.write(reinterpret_cast<char const*>(track.timestamps.data()), numTimestamps * sizeof(u32));
						}

						u32 numValues = static_cast<u32>(track.values.size());
						{
							stream.write(reinterpret_cast<char const*>(&numValues), sizeof(u32));
							stream.write(reinterpret_cast<char const*>(track.values.data()), numTimestamps * sizeof(T));
						}
					}
				}
			}
			bool Deserialize(Bytebuffer* buffer)
			{
				if (!buffer->Get(interpolationType))
					return false;

				if (!buffer->Get(isGlobalSequence))
					return false;

				u32 numTracks = 0;
				if (!buffer->GetU32(numTracks))
					return false;

				tracks.resize(numTracks);

				for (u32 i = 0; i < numTracks; i++)
				{
					AnimationTrack<T>& track = tracks[i];

					if (!buffer->GetU32(track.sequenceID))
						return false;

					u32 numTimestamps = 0;
					if (!buffer->GetU32(numTimestamps))
						return false;

					track.timestamps.resize(numTimestamps);
					if (!buffer->GetBytes(reinterpret_cast<u8*>(track.timestamps.data()), numTimestamps * sizeof(u32)))
						return false;

					u32 numValues = 0;
					if (!buffer->GetU32(numValues))
						return false;

					track.values.resize(numValues);
					if (!buffer->GetBytes(reinterpret_cast<u8*>(track.values.data()), numValues * sizeof(T)))
						return false;
				}

				return true;
			}
		};

		struct CullingData
		{
			hvec3 center = hvec3(static_cast<f16>(0.0f));
			hvec3 extents = hvec3(static_cast<f16>(65535.0f));
			f32 boundingSphereRadius = 0.0f;
		};

		struct Material
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
		struct Texture
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

			u32 textureHash = 0;
		};
		struct Vertex
		{
		public:
			hvec3 position = hvec3(f16(0), f16(0), f16(0));
			u8 octNormal[2] = { 0 };
			hvec2 uvCoords[2] = { hvec2(f16(0), f16(0)), hvec2(f16(0), f16(0)) };

			u8 boneIndices[4] = { 0, 0, 0, 0 };
			u8 boneWeights[4] = { 0, 0, 0, 0 };
		};
		struct TextureTransform
		{
		public:
			AnimationData<vec3> translation = { };
			AnimationData<quat> rotation = { };
			AnimationData<vec3> scale = { };
		};

		struct Bone
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

			i32 primaryBoneIndex = -1;
			Flags flags = { };

			i16 parentBoneID = -1;
			u16 submeshID = 0;

			AnimationData<vec3> translation = { };
			AnimationData<quat> rotation = { };
			AnimationData<vec3> scale = { };

			vec3 pivot = { };

		public:
			Bone() { }
			Bone(Bone& other)
			{
				primaryBoneIndex = other.primaryBoneIndex;
				flags = other.flags;
				parentBoneID = other.parentBoneID;
				submeshID = other.submeshID;

				translation = other.translation;
				rotation = other.rotation;
				scale = other.scale;


				pivot = other.pivot;
			}
            Bone(Bone&& other) noexcept
                : primaryBoneIndex(std::move(other.primaryBoneIndex)),
                flags(std::move(other.flags)),
                parentBoneID(std::move(other.parentBoneID)),
                submeshID(std::move(other.submeshID)),
                translation(std::move(other.translation)),
                rotation(std::move(other.rotation)),
                scale(std::move(other.scale)),
                pivot(std::move(other.pivot))
            {}
            Bone& operator=(const Bone& other) = default;
        };
		struct AnimationSequence
		{
		public:
			struct Flags
			{
				u32 isAlwaysPlaying : 1;
				u32 isAlias : 1;
				u32 blendTransition : 1; // (This applies if set on either side of the transition) If set we lerp between the end -> start states, but only if end != start (Compare Bone Values)
			};

			u16 id = 0; // Animation Id (AnimationData.ndbc)
			u16 subID = 0; // Sub Animation Id (An id to indicate the variation index of this animation)

			u32 duration = 0; // Duration of animation in milliseconds.
			f32 moveSpeed = 0.0f; // The speed at which the model moves with.

			Flags flags = { };

			i16 frequency = 0; // Determines how often the animation is played.
			uvec2 repetitionRange = { }; // Unless the value is (0,0) pick a random number of repetitions to do based on (min, max)
			u16 blendTimeStart = 0;
			u16 blendTimeEnd = 0;

			vec3 aabbCenter = { };
			vec3 aabbExtents = { };
			f32 radius = 0.0f;

			i16 nextVariationID = -1; // Specifies the variation id for the next variation for this animation or (-1) for none.
			u16 nextAliasID = 0; // Specifies the id for the actual animation.
		};

		struct TextureUnit
		{
		public:
			struct Flags
			{
				u8 InvertMaterial : 1;
				u8 Transform : 1;
				u8 IsProjectedTexture : 1;
				u8 : 3;
				u8 FixTransparency : 1;
			};

			Flags flags = { };

			union
			{
				struct
				{
					i8 vertexShaderID;
					i8 pixelShaderID;
				};

				i16 shaderID = 0;
			};

			u16 materialIndex = 0;
			u16 materialLayer = 0;

			u16 textureCount = 0;
			u16 textureIndexStart = 0;
			u16 textureTransformIndexStart = 0;
			u16 textureUnitLookupID = 0;
			u16 textureTransparencyLookupID = 0;
		};
		struct RenderBatch
		{
		public:
			u16 groupID = 0;
			u32 vertexStart = 0;
			u32 vertexCount = 0;
			u32 indexStart = 0;
			u32 indexCount = 0;
			u8 renderPriority = 0;
			bool isTransparent = false;

			std::vector<TextureUnit> textureUnits = { };
		};
		struct Camera
		{
			u32 type = 0;
			f32 farClip = 0.0f;
			f32 nearClip = 0.0f;

			vec3 positionBase = { };
			vec3 targetPositionBase = { };

			AnimationData<SplineKey<vec3>> positions = { };
			AnimationData<SplineKey<vec3>> targetPositions = { };
			AnimationData<SplineKey<f32>> roll = { };
			AnimationData<SplineKey<f32>> fov = { };
		};
		struct Decoration
		{
			u32 nameID = 0;
			vec3 position = vec3(0.0f, 0.0f, 0.0f);
			quat rotation = quat(1.0f, 0.0f, 0.0f, 0.0f);
			f32 scale = 0.0f;
			u32 color = 0;
		};
		struct DecorationSet
		{
			char name[20] = { 0 };
			u16 index = 0;
			u32 count = 0;
		};
		struct ModelData
		{
		public:
			std::vector<u16> vertexLookupIDs = { };
			std::vector<u16> indices = { }; // These are relative to the index of vertexLookupIds and needs to be translated
			std::vector<RenderBatch> renderBatches = { };
		};
		struct ModelHeader
		{
			u32 numVertices;

			u32 numVertexLookupIDs;
			u32 numIndices;

			u32 numRenderBatches;
			u32 numOpaqueRenderBatches;
			u32 numTransparentRenderBatches;

			u32 numTextures;
			u32 numTextureUnits;
			u32 numMaterials;
			u32 numTextureTransforms;

			u32 numSequences;
			u32 numBones;
			u32 numCameras;

			u32 numDecorationSets;
			u32 numDecorations;
		};

	public:
		FileHeader header = FileHeader(FileHeader::Type::ComplexModel, CURRENT_VERSION);
		ModelHeader modelHeader = { };

		Flags flags = { };

		std::vector<AnimationSequence> sequences;
		std::vector<Bone> bones;

		std::vector<Vertex> vertices;
		std::vector<Texture> textures;
		std::vector<Material> materials;
		std::vector<TextureTransform> textureTransforms;

		std::vector<u16> textureIndexLookupTable;
		std::vector<u16> textureUnitLookupTable;
		std::vector<u16> textureTransparencyLookupTable;
		std::vector<u16> textureTransformLookupTable;
		std::vector<u16> textureCombinerCombos;

		std::vector<vec3> collisionVertexPositions;
		std::vector<u16> collisionIndices;
		std::vector<std::array<u8, 2>> collisionNormals;

		std::vector<Camera> cameras;
		std::vector<DecorationSet> decorationSets;
		std::vector<Decoration> decorations;

		vec3 aabbCenter = { };
		vec3 aabbExtents = { };
		CullingData cullingData = { };

		ModelData modelData = { };

	public:
		bool Save(const std::string& path);

		static bool Read(std::shared_ptr<Bytebuffer>& buffer, ComplexModel& out);
		static bool ReadHeader(std::shared_ptr<Bytebuffer>& buffer, ModelHeader& out);
		static bool FromM2(std::shared_ptr<Bytebuffer>& rootBuffer, std::shared_ptr<Bytebuffer>& skinBuffer, M2::Layout& layout, ComplexModel& out);
		static bool FromMapObject(const MapObject& mapObject, ComplexModel& out);

	private:
		bool PostProcessing();
		static i8 GetVertexShaderID(i16 shaderID, u16 textureCount);
		static i8 GetPixelShaderID(i16 shaderID, u16 textureCount);
	};
PRAGMA_NO_PADDING_END;
}

#pragma once
#include "FileFormat/Novus/FileHeader.h"

#include <Base/Types.h>

#include <limits>
#include <memory>
#include <type_traits>
#include <vector>

namespace FileFormat::Animation
{
    inline constexpr char RIG_FAMILY_FILE_EXTENSION[] = ".rigfamily";
    inline constexpr char SKELETON_FILE_EXTENSION[] = ".skeleton";
    inline constexpr char CLIP_FILE_EXTENSION[] = ".animationclip";
    inline constexpr char ANIMATION_SET_FILE_EXTENSION[] = ".animationset";
    inline constexpr char GRAPH_FILE_EXTENSION[] = ".animationgraph";
    inline constexpr char BONE_MASK_FILE_EXTENSION[] = ".bonemask";
    inline constexpr char IK_RIG_FILE_EXTENSION[] = ".ikrig";
    inline constexpr char RETARGET_PROFILE_FILE_EXTENSION[] = ".retargetprofile";
    inline constexpr char ANIMATION_BOUNDS_FILE_EXTENSION[] = ".animationbounds";

    // Keep this value fixed during coordinated pre-main development. The first
    // accepted main-branch formats establish their initial shipping versions.
    inline constexpr u32 DEVELOPMENT_VERSION = 0;
    inline constexpr FileHeader::Type RIG_FAMILY_FILE_TYPE = FileHeader::Type::RigFamily;
    inline constexpr FileHeader::Type SKELETON_FILE_TYPE = FileHeader::Type::Skeleton;
    inline constexpr FileHeader::Type CLIP_FILE_TYPE = FileHeader::Type::AnimationClip;
    inline constexpr FileHeader::Type ANIMATION_SET_FILE_TYPE = FileHeader::Type::AnimationSet;
    inline constexpr FileHeader::Type GRAPH_FILE_TYPE = FileHeader::Type::AnimationGraph;
    inline constexpr FileHeader::Type BONE_MASK_FILE_TYPE = FileHeader::Type::BoneMask;
    inline constexpr FileHeader::Type IK_RIG_FILE_TYPE = FileHeader::Type::IKRig;
    inline constexpr FileHeader::Type RETARGET_PROFILE_FILE_TYPE = FileHeader::Type::RetargetProfile;
    inline constexpr FileHeader::Type ANIMATION_BOUNDS_FILE_TYPE = FileHeader::Type::AnimationBounds;

    // Every non-empty root section begins on a 16-byte boundary. Asset-root
    // offsets are byte offsets from the start of the file; child-record offsets
    // are element offsets into the corresponding root array. AssetConverter writes
    // final contiguous arrays for direct bulk copies. Save/Read use Bytebuffer;
    // PACT remains responsible for archive/disk I/O.

    struct LocalTransform
    {
        vec3 translation = {};
        u32 reserved0 = 0;
        quat rotation = quat(1.0f, 0.0f, 0.0f, 0.0f);
        vec3 scale = vec3(1.0f);
        u32 reserved1 = 0;
    };

    struct Bounds
    {
        vec3 center = {};
        f32 sphereRadius = 0.0f;
        vec3 extents = {};
        u32 reserved = 0;
    };

    enum class PoseDomainType : u8
    {
        RigFamily,
        Skeleton
    };

    enum RigFamilyJointFlags : u32
    {
        RigFamilyJointFlags_None = 0,
        RigFamilyJointFlags_Required = 1u << 0
    };

    struct RigFamilyJoint
    {
        u64 semanticID = 0;
        i32 parentJointIndex = -1;
        u32 flags = RigFamilyJointFlags_None;
        LocalTransform canonicalTransform;
    };

    struct RigFamilyChain
    {
        u64 semanticID = 0;
        u32 jointIndexOffset = 0;
        u32 numJointIndices = 0;
        u32 flags = 0;
        u32 reserved = 0;
    };

    struct RigFamilyData
    {
        std::vector<RigFamilyJoint> joints;
        std::vector<RigFamilyChain> chains;
        std::vector<u32> chainJointIndices;
    };

    struct RigFamilyAsset
    {
        FileHeader header = FileHeader(RIG_FAMILY_FILE_TYPE, DEVELOPMENT_VERSION);
        u32 jointsOffset = 0;
        u32 numJoints = 0;
        u32 chainsOffset = 0;
        u32 numChains = 0;
        u32 chainJointIndicesOffset = 0;
        u32 numChainJointIndices = 0;
        u32 flags = 0;
        u32 reserved = 0;

        size_t GetSerializedSize(const RigFamilyData& data) const;
        bool Save(std::shared_ptr<Bytebuffer>& buffer, const RigFamilyData& data);
        static bool Read(std::shared_ptr<Bytebuffer>& buffer, RigFamilyAsset& out);
    };

    enum SkeletonJointFlags : u16
    {
        SkeletonJointFlags_None = 0,
        SkeletonJointFlags_Deformation = 1u << 0,
        SkeletonJointFlags_Helper = 1u << 1,
        SkeletonJointFlags_Extension = 1u << 2
    };

    struct SkeletonJoint
    {
        u64 semanticID = 0;
        i32 parentJointIndex = -1;
        i32 familyJointIndex = -1;
        u16 hierarchyDepth = 0;
        u16 flags = SkeletonJointFlags_None;
        u32 reserved = 0;
        LocalTransform restTransform;
        mat4a inverseBindTransform = mat4a(1.0f);
    };

    // Pre-resolved conversion between one concrete joint and its inherited family
    // joint. Runtime retargeting performs no name matching or hierarchy discovery.
    struct SkeletonFamilyBinding
    {
        u32 skeletonJointIndex = 0;
        u32 familyJointIndex = 0;
        LocalTransform skeletonToFamily;
        LocalTransform familyToSkeleton;
    };

    enum class SkeletonPropagationType : u8
    {
        CopyLocalTransform,
        CopyModelTransform,
        DistributeRotation
    };

    // Explicit helper/twist propagation for concrete skeleton topology that does
    // not match the logical RigFamily hierarchy. Rules are pre-sorted in authored
    // execution order by the cooker.
    struct SkeletonPropagationRule
    {
        u32 sourceJointIndex = 0;
        u32 targetJointIndex = 0;
        SkeletonPropagationType type = SkeletonPropagationType::CopyLocalTransform;
        u8 flags = 0;
        u16 executionOrder = 0;
        f32 weight = 1.0f;
        LocalTransform offsetTransform;
    };

    struct SkeletonAttachment
    {
        u64 semanticID = 0;
        u32 jointIndex = 0;
        u32 flags = 0;
        LocalTransform localTransform;
    };

    struct HierarchyDepthRange
    {
        u32 jointIndexOffset = 0;
        u32 numJointIndices = 0;
    };

    struct SkeletonData
    {
        std::vector<SkeletonJoint> joints;
        std::vector<SkeletonFamilyBinding> familyBindings;
        std::vector<SkeletonPropagationRule> propagationRules;
        std::vector<HierarchyDepthRange> hierarchyDepthRanges;
        std::vector<u32> hierarchyDepthJointIndices;
        std::vector<SkeletonAttachment> attachments;
    };

    struct SkeletonAsset
    {
        FileHeader header = FileHeader(SKELETON_FILE_TYPE, DEVELOPMENT_VERSION);
        AssetID rigFamilyAssetID = INVALID_ASSET_ID;
        u32 jointsOffset = 0;
        u32 numJoints = 0;
        u32 familyBindingsOffset = 0;
        u32 numFamilyBindings = 0;
        u32 propagationRulesOffset = 0;
        u32 numPropagationRules = 0;
        u32 hierarchyDepthRangesOffset = 0;
        u32 numHierarchyDepthRanges = 0;
        u32 hierarchyDepthJointIndicesOffset = 0;
        u32 numHierarchyDepthJointIndices = 0;
        u32 attachmentsOffset = 0;
        u32 numAttachments = 0;
        u32 flags = 0;
        u32 reserved = 0;

        size_t GetSerializedSize(const SkeletonData& data) const;
        bool Save(std::shared_ptr<Bytebuffer>& buffer, const SkeletonData& data);
        static bool Read(std::shared_ptr<Bytebuffer>& buffer, SkeletonAsset& out);
    };

    enum AnimationClipFlags : u8
    {
        AnimationClipFlags_None = 0,
        AnimationClipFlags_Looping = 1u << 0,
        AnimationClipFlags_Additive = 1u << 1
    };

    enum class AdditiveReferenceType : u8
    {
        SkeletonRestPose,
        AnimationClipSample
    };

    enum AnimationTrackFlags : u32
    {
        AnimationTrackFlags_None = 0,
        AnimationTrackFlags_HasTranslationSamples = 1u << 0,
        AnimationTrackFlags_HasRotationSamples = 1u << 1,
        AnimationTrackFlags_HasScaleSamples = 1u << 2
    };

    // Baseline uniformly sampled, bounded-random-access track. Each non-empty
    // channel offset/count pair contains exactly AnimationClipAsset::sampleCount
    // elements when non-empty.
    // Empty channels use the corresponding default value. A future block codec
    // replaces this payload behind an explicit codec contract after A/B testing;
    // no timestamp search is reintroduced.
    struct AnimationTrack
    {
        u32 sourceJointIndex = 0;
        u32 flags = AnimationTrackFlags_None;
        u32 translationOffset = 0;
        u32 numTranslations = 0;
        u32 rotationOffset = 0;
        u32 numRotations = 0;
        u32 scaleOffset = 0;
        u32 numScales = 0;
        vec3 defaultTranslation = {};
        u32 reserved0 = 0;
        quat defaultRotation = quat(1.0f, 0.0f, 0.0f, 0.0f);
        vec3 defaultScale = vec3(1.0f);
        u32 reserved1 = 0;
    };

    struct SynchronizationMarker
    {
        u64 semanticID = 0;
        u32 timeMicroseconds = 0;
        u32 flags = 0;
    };

    struct ActionWindow
    {
        u64 semanticID = 0;
        u32 startTimeMicroseconds = 0;
        u32 endTimeMicroseconds = 0;
    };

    struct AnimationEvent
    {
        u32 timeMicroseconds = 0;
        u32 eventTypeID = 0;
        u32 payloadTypeID = 0;
        u32 flags = 0;
        u32 payloadOffset = 0;
        u32 payloadSize = 0;
    };

    struct AnimationClipData
    {
        std::vector<AnimationTrack> tracks;
        std::vector<vec3> translationSamples;
        std::vector<quat> rotationSamples;
        std::vector<vec3> scaleSamples;
        std::vector<SynchronizationMarker> synchronizationMarkers;
        std::vector<ActionWindow> actionWindows;
        std::vector<AnimationEvent> events;
        std::vector<u8> eventPayloadBytes;
    };

    struct AnimationClipAsset
    {
        FileHeader header = FileHeader(CLIP_FILE_TYPE, DEVELOPMENT_VERSION);
        AssetID sourcePoseDomainAssetID = INVALID_ASSET_ID;
        AssetID additiveReferenceClipAssetID = INVALID_ASSET_ID;
        u32 durationMicroseconds = 0;
        u16 sampleRateHz = 0;
        PoseDomainType sourcePoseDomainType = PoseDomainType::RigFamily;
        u8 flags = AnimationClipFlags_None;
        u32 sampleCount = 0;
        u32 additiveReferenceTimeMicroseconds = 0;
        AdditiveReferenceType additiveReferenceType = AdditiveReferenceType::SkeletonRestPose;
        u8 reserved[7] = {0, 0, 0, 0, 0, 0, 0};

        u32 tracksOffset = 0;
        u32 numTracks = 0;
        u32 translationSamplesOffset = 0;
        u32 numTranslationSamples = 0;
        u32 rotationSamplesOffset = 0;
        u32 numRotationSamples = 0;
        u32 scaleSamplesOffset = 0;
        u32 numScaleSamples = 0;
        u32 synchronizationMarkersOffset = 0;
        u32 numSynchronizationMarkers = 0;
        u32 actionWindowsOffset = 0;
        u32 numActionWindows = 0;
        u32 eventsOffset = 0;
        u32 numEvents = 0;
        u32 eventPayloadBytesOffset = 0;
        u32 numEventPayloadBytes = 0;

        size_t GetSerializedSize(const AnimationClipData& data) const;
        bool Save(std::shared_ptr<Bytebuffer>& buffer, const AnimationClipData& data);
        static bool Read(std::shared_ptr<Bytebuffer>& buffer, AnimationClipAsset& out);
    };

    enum AnimationSetSlotFlags : u32
    {
        AnimationSetSlotFlags_None = 0,
        AnimationSetSlotFlags_Required = 1u << 0
    };

    struct AnimationClipVariant
    {
        AssetID animationClipAssetID = INVALID_ASSET_ID;
        u16 selectionWeight = 1;
        u16 flags = 0;
        u32 stableVariationID = 0;
    };

    struct AnimationSetSlot
    {
        u64 semanticID = 0;
        u32 variantOffset = 0;
        u32 numVariants = 0;
        i32 fallbackSlotIndex = -1;
        u32 flags = AnimationSetSlotFlags_None;
    };

    struct AnimationSetData
    {
        std::vector<AnimationSetSlot> slots;
        std::vector<AnimationClipVariant> variants;
    };

    struct AnimationSetAsset
    {
        FileHeader header = FileHeader(ANIMATION_SET_FILE_TYPE, DEVELOPMENT_VERSION);
        AssetID declaredPoseDomainAssetID = INVALID_ASSET_ID;
        PoseDomainType declaredPoseDomainType = PoseDomainType::RigFamily;
        u8 reserved0[3] = {0, 0, 0};
        u32 flags = 0;
        u32 slotsOffset = 0;
        u32 numSlots = 0;
        u32 variantsOffset = 0;
        u32 numVariants = 0;

        size_t GetSerializedSize(const AnimationSetData& data) const;
        bool Save(std::shared_ptr<Bytebuffer>& buffer, const AnimationSetData& data);
        static bool Read(std::shared_ptr<Bytebuffer>& buffer, AnimationSetAsset& out);
    };

    enum class GraphValueType : u8
    {
        Bool,
        Int,
        Float,
        Float2,
        Float3,
        Quaternion,
        AssetHandle
    };

    struct GraphParameter
    {
        u64 nameHash = 0;
        u32 defaultValueByteOffset = 0;
        u16 byteSize = 0;
        GraphValueType type = GraphValueType::Float;
        u8 flags = 0;
    };

    struct GraphCommand
    {
        u64 semanticID = 0;
        u16 payloadByteSize = 0;
        u8 flags = 0;
        u8 reserved0 = 0;
        u32 reserved1 = 0;
    };

    struct GraphActionSlot
    {
        u64 semanticID = 0;
        u32 runtimeStateByteOffset = 0;
        u16 runtimeStateByteSize = 0;
        u16 flags = 0;
    };

    struct GraphStateMigrationEntry
    {
        u64 sourceNodeGUID = 0;
        u32 runtimeStateByteOffset = 0;
        u16 runtimeStateByteSize = 0;
        u16 stateTypeID = 0;
    };

    struct AnimationGraphData
    {
        std::vector<u8> defaultParameterData;
        std::vector<GraphParameter> parameters;
        std::vector<GraphCommand> commands;
        std::vector<GraphActionSlot> actionSlots;
        std::vector<GraphStateMigrationEntry> stateMigrationEntries;
        std::vector<u8> controllerProgram;
        std::vector<u8> poseProgram;
    };

    // Controller and pose programs are immutable compiler outputs. Their opcode
    // layouts remain a separate reviewed ABI; this root already fixes ownership,
    // schemas, bounded instance state, and hot-reload migration metadata.
    struct AnimationGraphAsset
    {
        FileHeader header = FileHeader(GRAPH_FILE_TYPE, DEVELOPMENT_VERSION);
        u32 instanceStateByteSize = 0;
        u16 maximumActiveClips = 0;
        u16 maximumActivePoseDomains = 0;
        u16 maximumPoseOperations = 0;
        u16 flags = 0;
        u32 reserved = 0;

        u32 defaultParameterDataOffset = 0;
        u32 defaultParameterDataSize = 0;
        u32 parametersOffset = 0;
        u32 numParameters = 0;
        u32 commandsOffset = 0;
        u32 numCommands = 0;
        u32 actionSlotsOffset = 0;
        u32 numActionSlots = 0;
        u32 stateMigrationEntriesOffset = 0;
        u32 numStateMigrationEntries = 0;
        u32 controllerProgramOffset = 0;
        u32 controllerProgramSize = 0;
        u32 poseProgramOffset = 0;
        u32 poseProgramSize = 0;

        size_t GetSerializedSize(const AnimationGraphData& data) const;
        bool Save(std::shared_ptr<Bytebuffer>& buffer, const AnimationGraphData& data);
        static bool Read(std::shared_ptr<Bytebuffer>& buffer, AnimationGraphAsset& out);
    };

    struct BoneMaskData
    {
        std::vector<u8> jointWeights;
    };

    struct BoneMaskAsset
    {
        FileHeader header = FileHeader(BONE_MASK_FILE_TYPE, DEVELOPMENT_VERSION);
        AssetID poseDomainAssetID = INVALID_ASSET_ID;
        PoseDomainType poseDomainType = PoseDomainType::RigFamily;
        u8 reserved0[3] = {0, 0, 0};
        u32 flags = 0;
        u32 jointWeightsOffset = 0;
        u32 numJointWeights = 0;

        size_t GetSerializedSize(const BoneMaskData& data) const;
        bool Save(std::shared_ptr<Bytebuffer>& buffer, const BoneMaskData& data);
        static bool Read(std::shared_ptr<Bytebuffer>& buffer, BoneMaskAsset& out);
    };

    enum class IKSolverType : u8
    {
        TwoBone,
        Aim,
        LookAt,
        IterativeChain
    };

    enum class IKGoalSpace : u8
    {
        Model,
        World,
        Joint
    };

    struct IKJointLimit
    {
        vec3 minimumRadians = {};
        f32 reserved0 = 0.0f;
        vec3 maximumRadians = {};
        f32 reserved1 = 0.0f;
    };

    struct IKChain
    {
        u64 semanticID = 0;
        u32 jointIndexOffset = 0;
        u32 numJointIndices = 0;
        u32 jointLimitOffset = 0;
        u32 numJointLimits = 0;
        u32 effectorJointIndex = 0;
        u32 poleJointIndex = std::numeric_limits<u32>::max();
        vec3 preferredBendDirection = {};
        f32 maximumReach = 0.0f;
    };

    struct IKSolver
    {
        u64 semanticID = 0;
        u32 chainIndex = 0;
        IKSolverType type = IKSolverType::TwoBone;
        IKGoalSpace goalSpace = IKGoalSpace::Model;
        u8 minimumQualityTier = 0;
        u8 flags = 0;
        u16 maximumIterations = 0;
        u16 reserved0 = 0;
        f32 convergenceTolerance = 0.0f;
        f32 maximumReachScale = 1.0f;
        u32 reserved1 = 0;
    };

    struct IKRigData
    {
        std::vector<IKChain> chains;
        std::vector<u32> chainJointIndices;
        std::vector<IKJointLimit> jointLimits;
        std::vector<IKSolver> solvers;
    };

    struct IKRigAsset
    {
        FileHeader header = FileHeader(IK_RIG_FILE_TYPE, DEVELOPMENT_VERSION);
        AssetID skeletonAssetID = INVALID_ASSET_ID;
        u32 chainsOffset = 0;
        u32 numChains = 0;
        u32 chainJointIndicesOffset = 0;
        u32 numChainJointIndices = 0;
        u32 jointLimitsOffset = 0;
        u32 numJointLimits = 0;
        u32 solversOffset = 0;
        u32 numSolvers = 0;
        vec3 conservativeExtentExpansion = {};
        f32 maximumGoalDistance = 0.0f;
        u32 flags = 0;
        u32 reserved = 0;

        size_t GetSerializedSize(const IKRigData& data) const;
        bool Save(std::shared_ptr<Bytebuffer>& buffer, const IKRigData& data);
        static bool Read(std::shared_ptr<Bytebuffer>& buffer, IKRigAsset& out);
    };

    struct RetargetJointMapping
    {
        u64 semanticID = 0;
        i32 sourceJointIndex = -1;
        i32 targetJointIndex = -1;
        LocalTransform sourceToTarget;
    };

    struct RetargetProfileData
    {
        std::vector<RetargetJointMapping> jointMappings;
    };

    struct RetargetProfileAsset
    {
        FileHeader header = FileHeader(RETARGET_PROFILE_FILE_TYPE, DEVELOPMENT_VERSION);
        AssetID sourceRigFamilyAssetID = INVALID_ASSET_ID;
        AssetID targetRigFamilyAssetID = INVALID_ASSET_ID;
        u32 jointMappingsOffset = 0;
        u32 numJointMappings = 0;
        u32 flags = 0;
        u32 reserved = 0;

        size_t GetSerializedSize(const RetargetProfileData& data) const;
        bool Save(std::shared_ptr<Bytebuffer>& buffer, const RetargetProfileData& data);
        static bool Read(std::shared_ptr<Bytebuffer>& buffer, RetargetProfileAsset& out);
    };

    struct AnimatedBoundsSegment
    {
        u32 startTimeMicroseconds = 0;
        u32 endTimeMicroseconds = 0;
        Bounds bounds;
    };

    struct ClipBounds
    {
        AssetID animationClipAssetID = INVALID_ASSET_ID;
        u32 segmentOffset = 0;
        u32 numSegments = 0;
        Bounds fallbackBounds;
    };

    struct AnimationBoundsData
    {
        std::vector<ClipBounds> clips;
        std::vector<AnimatedBoundsSegment> segments;
    };

    // Bounds are binding-dependent: this asset belongs to one concrete Model and
    // Skeleton combination, not to a clip or skeleton in isolation.
    struct AnimationBoundsAsset
    {
        FileHeader header = FileHeader(ANIMATION_BOUNDS_FILE_TYPE, DEVELOPMENT_VERSION);
        AssetID modelAssetID = INVALID_ASSET_ID;
        AssetID skeletonAssetID = INVALID_ASSET_ID;
        u32 meshIndex = 0;
        u32 reserved0 = 0;
        u32 clipsOffset = 0;
        u32 numClips = 0;
        u32 segmentsOffset = 0;
        u32 numSegments = 0;
        u32 flags = 0;
        u32 reserved = 0;

        size_t GetSerializedSize(const AnimationBoundsData& data) const;
        bool Save(std::shared_ptr<Bytebuffer>& buffer, const AnimationBoundsData& data);
        static bool Read(std::shared_ptr<Bytebuffer>& buffer, AnimationBoundsAsset& out);
    };

    static_assert(sizeof(RigFamilyJoint) == 64);
    static_assert(sizeof(RigFamilyChain) == 24);
    static_assert(sizeof(LocalTransform) == 48);
    static_assert(sizeof(Bounds) == 32);
    static_assert(sizeof(vec2) == 8);
    static_assert(sizeof(vec3) == 12);
    static_assert(sizeof(vec4) == 16);
    static_assert(sizeof(quat) == 16);
    static_assert(sizeof(mat4a) == 48);
    static_assert(sizeof(RigFamilyAsset) == 40);
    static_assert(sizeof(SkeletonJoint) == 120);
    static_assert(sizeof(SkeletonFamilyBinding) == 104);
    static_assert(sizeof(SkeletonPropagationRule) == 64);
    static_assert(sizeof(SkeletonAttachment) == 64);
    static_assert(sizeof(HierarchyDepthRange) == 8);
    static_assert(sizeof(SkeletonAsset) == 72);
    static_assert(sizeof(AnimationTrack) == 80);
    static_assert(sizeof(SynchronizationMarker) == 16);
    static_assert(sizeof(ActionWindow) == 16);
    static_assert(sizeof(AnimationEvent) == 24);
    static_assert(sizeof(AnimationClipAsset) == 112);
    static_assert(sizeof(AnimationClipVariant) == 16);
    static_assert(sizeof(AnimationSetSlot) == 24);
    static_assert(sizeof(AnimationSetAsset) == 40);
    static_assert(sizeof(GraphParameter) == 16);
    static_assert(sizeof(GraphCommand) == 16);
    static_assert(sizeof(GraphActionSlot) == 16);
    static_assert(sizeof(GraphStateMigrationEntry) == 16);
    static_assert(sizeof(AnimationGraphAsset) == 80);
    static_assert(sizeof(BoneMaskAsset) == 32);
    static_assert(sizeof(IKJointLimit) == 32);
    static_assert(sizeof(IKChain) == 48);
    static_assert(sizeof(IKSolver) == 32);
    static_assert(sizeof(IKRigAsset) == 72);
    static_assert(sizeof(RetargetJointMapping) == 64);
    static_assert(sizeof(RetargetProfileAsset) == 40);
    static_assert(sizeof(AnimatedBoundsSegment) == 40);
    static_assert(sizeof(ClipBounds) == 48);
    static_assert(sizeof(AnimationBoundsAsset) == 56);

    static_assert(std::is_trivially_copyable_v<RigFamilyAsset>);
    static_assert(std::is_standard_layout_v<RigFamilyAsset>);
    static_assert(std::is_trivially_copyable_v<SkeletonAsset>);
    static_assert(std::is_standard_layout_v<SkeletonAsset>);
    static_assert(std::is_trivially_copyable_v<AnimationClipAsset>);
    static_assert(std::is_standard_layout_v<AnimationClipAsset>);
    static_assert(std::is_trivially_copyable_v<AnimationGraphAsset>);
    static_assert(std::is_standard_layout_v<AnimationGraphAsset>);
    static_assert(std::is_trivially_copyable_v<IKRigAsset>);
    static_assert(std::is_standard_layout_v<IKRigAsset>);
} // namespace FileFormat::Animation

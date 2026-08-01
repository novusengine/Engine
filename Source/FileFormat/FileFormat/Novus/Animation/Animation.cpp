#include "Animation.h"

#include <Base/Memory/Bytebuffer.h>

#include <limits>

namespace
{
    constexpr size_t SECTION_ALIGNMENT = 16;

    size_t AlignSection(size_t size)
    {
        return (size + SECTION_ALIGNMENT - 1) & ~(SECTION_ALIGNMENT - 1);
    }

    template <typename T>
    size_t AddSectionSize(size_t size, const std::vector<T>& values)
    {
        static_assert(std::is_trivially_copyable_v<T>);
        if (values.empty())
            return size;

        return AlignSection(size) + values.size() * sizeof(T);
    }

    bool AlignWrite(std::shared_ptr<Bytebuffer>& buffer)
    {
        constexpr u8 ZERO_PADDING[SECTION_ALIGNMENT] = {};
        const size_t alignedSize = AlignSection(buffer->writtenData);
        return buffer->PutBytes(ZERO_PADDING, alignedSize - buffer->writtenData);
    }

    template <typename T>
    bool WriteSection(std::shared_ptr<Bytebuffer>& buffer, const std::vector<T>& values, u32& offset, u32& count)
    {
        static_assert(std::is_trivially_copyable_v<T>);

        if (values.empty())
        {
            offset = 0;
            count = 0;
            return true;
        }

        if (values.size() > std::numeric_limits<u32>::max() || !AlignWrite(buffer) || buffer->writtenData > std::numeric_limits<u32>::max())
            return false;

        offset = static_cast<u32>(buffer->writtenData);
        count = static_cast<u32>(values.size());
        return buffer->PutBytes(values.data(), values.size() * sizeof(T));
    }

    template <typename T>
    bool IsValidSection(const std::shared_ptr<Bytebuffer>& buffer, u32 offset, u32 count, size_t headerSize)
    {
        static_assert(std::is_trivially_copyable_v<T>);

        if (count == 0)
            return offset == 0;

        if (offset < headerSize || (offset & (SECTION_ALIGNMENT - 1)) != 0 || offset > buffer->writtenData)
            return false;

        return count <= (buffer->writtenData - offset) / sizeof(T);
    }
}

namespace FileFormat::Animation
{
    size_t RigFamilyAsset::GetSerializedSize(const RigFamilyData& data) const
    {
        size_t size = sizeof(RigFamilyAsset);
        size = AddSectionSize(size, data.joints);
        size = AddSectionSize(size, data.chains);
        size = AddSectionSize(size, data.chainJointIndices);
        return size;
    }

    bool RigFamilyAsset::Save(std::shared_ptr<Bytebuffer>& buffer, const RigFamilyData& data)
    {
        if (!buffer || buffer->writtenData != 0 || GetSerializedSize(data) > buffer->size)
            return false;

        RigFamilyAsset serialized = *this;
        serialized.header = FileHeader(RIG_FAMILY_FILE_TYPE, DEVELOPMENT_VERSION);

        if (!buffer->Put(serialized))
            return false;

        bool failed = false;
        failed |= !WriteSection(buffer, data.joints, serialized.jointsOffset, serialized.numJoints);
        failed |= !WriteSection(buffer, data.chains, serialized.chainsOffset, serialized.numChains);
        failed |= !WriteSection(buffer, data.chainJointIndices, serialized.chainJointIndicesOffset, serialized.numChainJointIndices);
        failed |= !buffer->Put(serialized, 0);

        if (failed)
            return false;

        *this = serialized;
        return true;
    }

    bool RigFamilyAsset::Read(std::shared_ptr<Bytebuffer>& buffer, RigFamilyAsset& out)
    {
        if (!buffer || buffer->readData != 0 || !buffer->Get(out))
            return false;

        if (out.header.type != RIG_FAMILY_FILE_TYPE || out.header.version != DEVELOPMENT_VERSION)
            return false;

        const size_t headerSize = sizeof(RigFamilyAsset);
        return IsValidSection<RigFamilyJoint>(buffer, out.jointsOffset, out.numJoints, headerSize) &&
               IsValidSection<RigFamilyChain>(buffer, out.chainsOffset, out.numChains, headerSize) &&
               IsValidSection<u32>(buffer, out.chainJointIndicesOffset, out.numChainJointIndices, headerSize);
    }

    size_t SkeletonAsset::GetSerializedSize(const SkeletonData& data) const
    {
        size_t size = sizeof(SkeletonAsset);
        size = AddSectionSize(size, data.joints);
        size = AddSectionSize(size, data.familyBindings);
        size = AddSectionSize(size, data.propagationRules);
        size = AddSectionSize(size, data.hierarchyDepthRanges);
        size = AddSectionSize(size, data.hierarchyDepthJointIndices);
        size = AddSectionSize(size, data.attachments);
        return size;
    }

    bool SkeletonAsset::Save(std::shared_ptr<Bytebuffer>& buffer, const SkeletonData& data)
    {
        if (!buffer || buffer->writtenData != 0 || GetSerializedSize(data) > buffer->size)
            return false;

        SkeletonAsset serialized = *this;
        serialized.header = FileHeader(SKELETON_FILE_TYPE, DEVELOPMENT_VERSION);

        if (!buffer->Put(serialized))
            return false;

        bool failed = false;
        failed |= !WriteSection(buffer, data.joints, serialized.jointsOffset, serialized.numJoints);
        failed |= !WriteSection(buffer, data.familyBindings, serialized.familyBindingsOffset, serialized.numFamilyBindings);
        failed |= !WriteSection(buffer, data.propagationRules, serialized.propagationRulesOffset, serialized.numPropagationRules);
        failed |= !WriteSection(buffer, data.hierarchyDepthRanges, serialized.hierarchyDepthRangesOffset, serialized.numHierarchyDepthRanges);
        failed |= !WriteSection(buffer, data.hierarchyDepthJointIndices, serialized.hierarchyDepthJointIndicesOffset, serialized.numHierarchyDepthJointIndices);
        failed |= !WriteSection(buffer, data.attachments, serialized.attachmentsOffset, serialized.numAttachments);
        failed |= !buffer->Put(serialized, 0);

        if (failed)
            return false;

        *this = serialized;
        return true;
    }

    bool SkeletonAsset::Read(std::shared_ptr<Bytebuffer>& buffer, SkeletonAsset& out)
    {
        if (!buffer || buffer->readData != 0 || !buffer->Get(out))
            return false;

        if (out.header.type != SKELETON_FILE_TYPE || out.header.version != DEVELOPMENT_VERSION)
            return false;

        const size_t headerSize = sizeof(SkeletonAsset);
        return IsValidSection<SkeletonJoint>(buffer, out.jointsOffset, out.numJoints, headerSize) &&
               IsValidSection<SkeletonFamilyBinding>(buffer, out.familyBindingsOffset, out.numFamilyBindings, headerSize) &&
               IsValidSection<SkeletonPropagationRule>(buffer, out.propagationRulesOffset, out.numPropagationRules, headerSize) &&
               IsValidSection<HierarchyDepthRange>(buffer, out.hierarchyDepthRangesOffset, out.numHierarchyDepthRanges, headerSize) &&
               IsValidSection<u32>(buffer, out.hierarchyDepthJointIndicesOffset, out.numHierarchyDepthJointIndices, headerSize) &&
               IsValidSection<SkeletonAttachment>(buffer, out.attachmentsOffset, out.numAttachments, headerSize);
    }

    size_t AnimationClipAsset::GetSerializedSize(const AnimationClipData& data) const
    {
        size_t size = sizeof(AnimationClipAsset);
        size = AddSectionSize(size, data.tracks);
        size = AddSectionSize(size, data.translationSamples);
        size = AddSectionSize(size, data.rotationSamples);
        size = AddSectionSize(size, data.scaleSamples);
        size = AddSectionSize(size, data.synchronizationMarkers);
        size = AddSectionSize(size, data.actionWindows);
        size = AddSectionSize(size, data.events);
        size = AddSectionSize(size, data.eventPayloadBytes);
        return size;
    }

    bool AnimationClipAsset::Save(std::shared_ptr<Bytebuffer>& buffer, const AnimationClipData& data)
    {
        if (!buffer || buffer->writtenData != 0 || GetSerializedSize(data) > buffer->size)
            return false;

        AnimationClipAsset serialized = *this;
        serialized.header = FileHeader(CLIP_FILE_TYPE, DEVELOPMENT_VERSION);

        if (!buffer->Put(serialized))
            return false;

        bool failed = false;
        failed |= !WriteSection(buffer, data.tracks, serialized.tracksOffset, serialized.numTracks);
        failed |= !WriteSection(buffer, data.translationSamples, serialized.translationSamplesOffset, serialized.numTranslationSamples);
        failed |= !WriteSection(buffer, data.rotationSamples, serialized.rotationSamplesOffset, serialized.numRotationSamples);
        failed |= !WriteSection(buffer, data.scaleSamples, serialized.scaleSamplesOffset, serialized.numScaleSamples);
        failed |= !WriteSection(buffer, data.synchronizationMarkers, serialized.synchronizationMarkersOffset, serialized.numSynchronizationMarkers);
        failed |= !WriteSection(buffer, data.actionWindows, serialized.actionWindowsOffset, serialized.numActionWindows);
        failed |= !WriteSection(buffer, data.events, serialized.eventsOffset, serialized.numEvents);
        failed |= !WriteSection(buffer, data.eventPayloadBytes, serialized.eventPayloadBytesOffset, serialized.numEventPayloadBytes);
        failed |= !buffer->Put(serialized, 0);

        if (failed)
            return false;

        *this = serialized;
        return true;
    }

    bool AnimationClipAsset::Read(std::shared_ptr<Bytebuffer>& buffer, AnimationClipAsset& out)
    {
        if (!buffer || buffer->readData != 0 || !buffer->Get(out))
            return false;

        if (out.header.type != CLIP_FILE_TYPE || out.header.version != DEVELOPMENT_VERSION)
            return false;

        const size_t headerSize = sizeof(AnimationClipAsset);
        return IsValidSection<AnimationTrack>(buffer, out.tracksOffset, out.numTracks, headerSize) &&
               IsValidSection<vec3>(buffer, out.translationSamplesOffset, out.numTranslationSamples, headerSize) &&
               IsValidSection<quat>(buffer, out.rotationSamplesOffset, out.numRotationSamples, headerSize) &&
               IsValidSection<vec3>(buffer, out.scaleSamplesOffset, out.numScaleSamples, headerSize) &&
               IsValidSection<SynchronizationMarker>(buffer, out.synchronizationMarkersOffset, out.numSynchronizationMarkers, headerSize) &&
               IsValidSection<ActionWindow>(buffer, out.actionWindowsOffset, out.numActionWindows, headerSize) &&
               IsValidSection<AnimationEvent>(buffer, out.eventsOffset, out.numEvents, headerSize) &&
               IsValidSection<u8>(buffer, out.eventPayloadBytesOffset, out.numEventPayloadBytes, headerSize);
    }

    size_t AnimationSetAsset::GetSerializedSize(const AnimationSetData& data) const
    {
        size_t size = sizeof(AnimationSetAsset);
        size = AddSectionSize(size, data.slots);
        size = AddSectionSize(size, data.variants);
        return size;
    }

    bool AnimationSetAsset::Save(std::shared_ptr<Bytebuffer>& buffer, const AnimationSetData& data)
    {
        if (!buffer || buffer->writtenData != 0 || GetSerializedSize(data) > buffer->size)
            return false;

        AnimationSetAsset serialized = *this;
        serialized.header = FileHeader(ANIMATION_SET_FILE_TYPE, DEVELOPMENT_VERSION);

        if (!buffer->Put(serialized))
            return false;

        bool failed = false;
        failed |= !WriteSection(buffer, data.slots, serialized.slotsOffset, serialized.numSlots);
        failed |= !WriteSection(buffer, data.variants, serialized.variantsOffset, serialized.numVariants);
        failed |= !buffer->Put(serialized, 0);

        if (failed)
            return false;

        *this = serialized;
        return true;
    }

    bool AnimationSetAsset::Read(std::shared_ptr<Bytebuffer>& buffer, AnimationSetAsset& out)
    {
        if (!buffer || buffer->readData != 0 || !buffer->Get(out))
            return false;

        if (out.header.type != ANIMATION_SET_FILE_TYPE || out.header.version != DEVELOPMENT_VERSION)
            return false;

        const size_t headerSize = sizeof(AnimationSetAsset);
        return IsValidSection<AnimationSetSlot>(buffer, out.slotsOffset, out.numSlots, headerSize) &&
               IsValidSection<AnimationClipVariant>(buffer, out.variantsOffset, out.numVariants, headerSize);
    }

    size_t AnimationGraphAsset::GetSerializedSize(const AnimationGraphData& data) const
    {
        size_t size = sizeof(AnimationGraphAsset);
        size = AddSectionSize(size, data.defaultParameterData);
        size = AddSectionSize(size, data.parameters);
        size = AddSectionSize(size, data.commands);
        size = AddSectionSize(size, data.actionSlots);
        size = AddSectionSize(size, data.stateMigrationEntries);
        size = AddSectionSize(size, data.controllerProgram);
        size = AddSectionSize(size, data.poseProgram);
        return size;
    }

    bool AnimationGraphAsset::Save(std::shared_ptr<Bytebuffer>& buffer, const AnimationGraphData& data)
    {
        if (!buffer || buffer->writtenData != 0 || GetSerializedSize(data) > buffer->size)
            return false;

        AnimationGraphAsset serialized = *this;
        serialized.header = FileHeader(GRAPH_FILE_TYPE, DEVELOPMENT_VERSION);

        if (!buffer->Put(serialized))
            return false;

        bool failed = false;
        failed |= !WriteSection(buffer, data.defaultParameterData, serialized.defaultParameterDataOffset, serialized.defaultParameterDataSize);
        failed |= !WriteSection(buffer, data.parameters, serialized.parametersOffset, serialized.numParameters);
        failed |= !WriteSection(buffer, data.commands, serialized.commandsOffset, serialized.numCommands);
        failed |= !WriteSection(buffer, data.actionSlots, serialized.actionSlotsOffset, serialized.numActionSlots);
        failed |= !WriteSection(buffer, data.stateMigrationEntries, serialized.stateMigrationEntriesOffset, serialized.numStateMigrationEntries);
        failed |= !WriteSection(buffer, data.controllerProgram, serialized.controllerProgramOffset, serialized.controllerProgramSize);
        failed |= !WriteSection(buffer, data.poseProgram, serialized.poseProgramOffset, serialized.poseProgramSize);
        failed |= !buffer->Put(serialized, 0);

        if (failed)
            return false;

        *this = serialized;
        return true;
    }

    bool AnimationGraphAsset::Read(std::shared_ptr<Bytebuffer>& buffer, AnimationGraphAsset& out)
    {
        if (!buffer || buffer->readData != 0 || !buffer->Get(out))
            return false;

        if (out.header.type != GRAPH_FILE_TYPE || out.header.version != DEVELOPMENT_VERSION)
            return false;

        const size_t headerSize = sizeof(AnimationGraphAsset);
        return IsValidSection<u8>(buffer, out.defaultParameterDataOffset, out.defaultParameterDataSize, headerSize) &&
               IsValidSection<GraphParameter>(buffer, out.parametersOffset, out.numParameters, headerSize) &&
               IsValidSection<GraphCommand>(buffer, out.commandsOffset, out.numCommands, headerSize) &&
               IsValidSection<GraphActionSlot>(buffer, out.actionSlotsOffset, out.numActionSlots, headerSize) &&
               IsValidSection<GraphStateMigrationEntry>(buffer, out.stateMigrationEntriesOffset, out.numStateMigrationEntries, headerSize) &&
               IsValidSection<u8>(buffer, out.controllerProgramOffset, out.controllerProgramSize, headerSize) &&
               IsValidSection<u8>(buffer, out.poseProgramOffset, out.poseProgramSize, headerSize);
    }

    size_t BoneMaskAsset::GetSerializedSize(const BoneMaskData& data) const
    {
        return AddSectionSize(sizeof(BoneMaskAsset), data.jointWeights);
    }

    bool BoneMaskAsset::Save(std::shared_ptr<Bytebuffer>& buffer, const BoneMaskData& data)
    {
        if (!buffer || buffer->writtenData != 0 || GetSerializedSize(data) > buffer->size)
            return false;

        BoneMaskAsset serialized = *this;
        serialized.header = FileHeader(BONE_MASK_FILE_TYPE, DEVELOPMENT_VERSION);

        if (!buffer->Put(serialized))
            return false;

        bool failed = false;
        failed |= !WriteSection(buffer, data.jointWeights, serialized.jointWeightsOffset, serialized.numJointWeights);
        failed |= !buffer->Put(serialized, 0);

        if (failed)
            return false;

        *this = serialized;
        return true;
    }

    bool BoneMaskAsset::Read(std::shared_ptr<Bytebuffer>& buffer, BoneMaskAsset& out)
    {
        if (!buffer || buffer->readData != 0 || !buffer->Get(out))
            return false;

        if (out.header.type != BONE_MASK_FILE_TYPE || out.header.version != DEVELOPMENT_VERSION)
            return false;

        return IsValidSection<u8>(buffer, out.jointWeightsOffset, out.numJointWeights, sizeof(BoneMaskAsset));
    }

    size_t IKRigAsset::GetSerializedSize(const IKRigData& data) const
    {
        size_t size = sizeof(IKRigAsset);
        size = AddSectionSize(size, data.chains);
        size = AddSectionSize(size, data.chainJointIndices);
        size = AddSectionSize(size, data.jointLimits);
        size = AddSectionSize(size, data.solvers);
        return size;
    }

    bool IKRigAsset::Save(std::shared_ptr<Bytebuffer>& buffer, const IKRigData& data)
    {
        if (!buffer || buffer->writtenData != 0 || GetSerializedSize(data) > buffer->size)
            return false;

        IKRigAsset serialized = *this;
        serialized.header = FileHeader(IK_RIG_FILE_TYPE, DEVELOPMENT_VERSION);

        if (!buffer->Put(serialized))
            return false;

        bool failed = false;
        failed |= !WriteSection(buffer, data.chains, serialized.chainsOffset, serialized.numChains);
        failed |= !WriteSection(buffer, data.chainJointIndices, serialized.chainJointIndicesOffset, serialized.numChainJointIndices);
        failed |= !WriteSection(buffer, data.jointLimits, serialized.jointLimitsOffset, serialized.numJointLimits);
        failed |= !WriteSection(buffer, data.solvers, serialized.solversOffset, serialized.numSolvers);
        failed |= !buffer->Put(serialized, 0);

        if (failed)
            return false;

        *this = serialized;
        return true;
    }

    bool IKRigAsset::Read(std::shared_ptr<Bytebuffer>& buffer, IKRigAsset& out)
    {
        if (!buffer || buffer->readData != 0 || !buffer->Get(out))
            return false;

        if (out.header.type != IK_RIG_FILE_TYPE || out.header.version != DEVELOPMENT_VERSION)
            return false;

        const size_t headerSize = sizeof(IKRigAsset);
        return IsValidSection<IKChain>(buffer, out.chainsOffset, out.numChains, headerSize) &&
               IsValidSection<u32>(buffer, out.chainJointIndicesOffset, out.numChainJointIndices, headerSize) &&
               IsValidSection<IKJointLimit>(buffer, out.jointLimitsOffset, out.numJointLimits, headerSize) &&
               IsValidSection<IKSolver>(buffer, out.solversOffset, out.numSolvers, headerSize);
    }

    size_t RetargetProfileAsset::GetSerializedSize(const RetargetProfileData& data) const
    {
        return AddSectionSize(sizeof(RetargetProfileAsset), data.jointMappings);
    }

    bool RetargetProfileAsset::Save(std::shared_ptr<Bytebuffer>& buffer, const RetargetProfileData& data)
    {
        if (!buffer || buffer->writtenData != 0 || GetSerializedSize(data) > buffer->size)
            return false;

        RetargetProfileAsset serialized = *this;
        serialized.header = FileHeader(RETARGET_PROFILE_FILE_TYPE, DEVELOPMENT_VERSION);

        if (!buffer->Put(serialized))
            return false;

        bool failed = false;
        failed |= !WriteSection(buffer, data.jointMappings, serialized.jointMappingsOffset, serialized.numJointMappings);
        failed |= !buffer->Put(serialized, 0);

        if (failed)
            return false;

        *this = serialized;
        return true;
    }

    bool RetargetProfileAsset::Read(std::shared_ptr<Bytebuffer>& buffer, RetargetProfileAsset& out)
    {
        if (!buffer || buffer->readData != 0 || !buffer->Get(out))
            return false;

        if (out.header.type != RETARGET_PROFILE_FILE_TYPE || out.header.version != DEVELOPMENT_VERSION)
            return false;

        return IsValidSection<RetargetJointMapping>(buffer, out.jointMappingsOffset, out.numJointMappings, sizeof(RetargetProfileAsset));
    }

    size_t AnimationBoundsAsset::GetSerializedSize(const AnimationBoundsData& data) const
    {
        size_t size = sizeof(AnimationBoundsAsset);
        size = AddSectionSize(size, data.clips);
        size = AddSectionSize(size, data.segments);
        return size;
    }

    bool AnimationBoundsAsset::Save(std::shared_ptr<Bytebuffer>& buffer, const AnimationBoundsData& data)
    {
        if (!buffer || buffer->writtenData != 0 || GetSerializedSize(data) > buffer->size)
            return false;

        AnimationBoundsAsset serialized = *this;
        serialized.header = FileHeader(ANIMATION_BOUNDS_FILE_TYPE, DEVELOPMENT_VERSION);

        if (!buffer->Put(serialized))
            return false;

        bool failed = false;
        failed |= !WriteSection(buffer, data.clips, serialized.clipsOffset, serialized.numClips);
        failed |= !WriteSection(buffer, data.segments, serialized.segmentsOffset, serialized.numSegments);
        failed |= !buffer->Put(serialized, 0);

        if (failed)
            return false;

        *this = serialized;
        return true;
    }

    bool AnimationBoundsAsset::Read(std::shared_ptr<Bytebuffer>& buffer, AnimationBoundsAsset& out)
    {
        if (!buffer || buffer->readData != 0 || !buffer->Get(out))
            return false;

        if (out.header.type != ANIMATION_BOUNDS_FILE_TYPE || out.header.version != DEVELOPMENT_VERSION)
            return false;

        const size_t headerSize = sizeof(AnimationBoundsAsset);
        return IsValidSection<ClipBounds>(buffer, out.clipsOffset, out.numClips, headerSize) &&
               IsValidSection<AnimatedBoundsSegment>(buffer, out.segmentsOffset, out.numSegments, headerSize);
    }
}

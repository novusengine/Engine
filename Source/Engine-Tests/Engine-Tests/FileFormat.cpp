#include <catch2/catch2.hpp>

#include <Base/Memory/Bytebuffer.h>
#include <FileFormat/Novus/Animation/Animation.h>
#include <FileFormat/Novus/Map/Map.h>
#include <FileFormat/Novus/Model/Material.h>
#include <FileFormat/Novus/Model/MaterialPack.h>
#include <FileFormat/Novus/Model/Model.h>
#include <FileFormat/Novus/ShaderPack/ShaderPack.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

namespace
{
    constexpr u64 Fnv1a64(const u8* bytes, size_t size)
    {
        u64 value = 0xCBF29CE484222325ull;
        for (size_t index = 0; index < size; index++)
        {
            value ^= bytes[index];
            value *= 0x100000001B3ull;
        }
        return value;
    }

    FileFormat::Animation::LocalTransform MakeLocalTransform(const vec3& translation = vec3(0.0f), const quat& rotation = quat(1.0f, 0.0f, 0.0f, 0.0f), const vec3& scale = vec3(1.0f))
    {
        FileFormat::Animation::LocalTransform transform;
        transform.translation = translation;
        transform.rotation = rotation;
        transform.scale = scale;
        return transform;
    }

    template <typename TAsset, typename TData>
    void VerifyEmptyRoundTrip(TAsset asset, const TData& data)
    {
        const size_t serializedSize = asset.GetSerializedSize(data);
        std::shared_ptr<Bytebuffer> buffer = Bytebuffer::BorrowRuntime(serializedSize);

        REQUIRE(asset.Save(buffer, data));
        REQUIRE(buffer->writtenData == serializedSize);

        TAsset loaded;
        REQUIRE(TAsset::Read(buffer, loaded));
        REQUIRE(loaded.header == asset.header);
    }
}

static_assert(sizeof(FileFormat::Animation::LocalTransform) == 48);
static_assert(sizeof(FileFormat::Animation::SkeletonJoint) == 120);
static_assert(sizeof(FileFormat::Animation::SkeletonFamilyBinding) == 104);
static_assert(sizeof(FileFormat::Animation::SkeletonPropagationRule) == 64);
static_assert(sizeof(FileFormat::Animation::HierarchyDepthRange) == 8);
static_assert(sizeof(FileFormat::Animation::SkeletonAttachment) == 64);
static_assert(sizeof(FileFormat::Animation::SkeletonAsset) == 72);
static_assert(sizeof(FileFormat::Animation::AnimationTrack) == 80);
static_assert(sizeof(FileFormat::Animation::SynchronizationMarker) == 16);
static_assert(sizeof(FileFormat::Animation::ActionWindow) == 16);
static_assert(sizeof(FileFormat::Animation::AnimationEvent) == 24);
static_assert(sizeof(FileFormat::Animation::AnimationClipAsset) == 112);
static_assert(offsetof(FileFormat::Animation::SkeletonAsset, jointsOffset) == 16);
static_assert(offsetof(FileFormat::Animation::SkeletonAsset, attachmentsOffset) == 56);
static_assert(offsetof(FileFormat::Animation::AnimationClipAsset, tracksOffset) == 48);
static_assert(offsetof(FileFormat::Animation::AnimationClipAsset, eventPayloadBytesOffset) == 104);

TEST_CASE("Flat FileFormats follow the Bytebuffer Save and Read convention", "[FileFormat]")
{
    SECTION("Every asset root supports an empty round trip")
    {
        VerifyEmptyRoundTrip(FileFormat::Model::ModelAsset{}, FileFormat::Model::ModelData{});
        VerifyEmptyRoundTrip(FileFormat::Material::MaterialAsset{}, FileFormat::Material::MaterialData{});
        VerifyEmptyRoundTrip(FileFormat::Material::MaterialInstanceAsset{}, FileFormat::Material::MaterialInstanceData{});
        VerifyEmptyRoundTrip(FileFormat::Material::MaterialAnimationAsset{}, FileFormat::Material::MaterialAnimationData{});
        VerifyEmptyRoundTrip(FileFormat::Material::MaterialPack{}, FileFormat::Material::MaterialPackData{});
        VerifyEmptyRoundTrip(FileFormat::Animation::RigFamilyAsset{}, FileFormat::Animation::RigFamilyData{});
        VerifyEmptyRoundTrip(FileFormat::Animation::SkeletonAsset{}, FileFormat::Animation::SkeletonData{});
        VerifyEmptyRoundTrip(FileFormat::Animation::AnimationClipAsset{}, FileFormat::Animation::AnimationClipData{});
        VerifyEmptyRoundTrip(FileFormat::Animation::AnimationSetAsset{}, FileFormat::Animation::AnimationSetData{});
        VerifyEmptyRoundTrip(FileFormat::Animation::AnimationGraphAsset{}, FileFormat::Animation::AnimationGraphData{});
        VerifyEmptyRoundTrip(FileFormat::Animation::BoneMaskAsset{}, FileFormat::Animation::BoneMaskData{});
        VerifyEmptyRoundTrip(FileFormat::Animation::IKRigAsset{}, FileFormat::Animation::IKRigData{});
        VerifyEmptyRoundTrip(FileFormat::Animation::RetargetProfileAsset{}, FileFormat::Animation::RetargetProfileData{});
        VerifyEmptyRoundTrip(FileFormat::Animation::AnimationBoundsAsset{}, FileFormat::Animation::AnimationBoundsData{});
    }

    SECTION("Model sections remain aligned and directly addressable")
    {
        FileFormat::Model::ModelAsset asset;
        FileFormat::Model::ModelData data;
        data.positions.push_back({1, 2, 3, 0});
        data.physicsData = {10, 20, 30, 40};

        const size_t serializedSize = asset.GetSerializedSize(data);
        std::shared_ptr<Bytebuffer> buffer = Bytebuffer::BorrowRuntime(serializedSize);
        REQUIRE(asset.Save(buffer, data));
        REQUIRE(buffer->writtenData == serializedSize);
        REQUIRE((asset.positionsOffset & 15u) == 0);
        REQUIRE(asset.numPositions == 1);
        REQUIRE((asset.physicsDataOffset & 15u) == 0);
        REQUIRE(asset.numPhysicsDataBytes == data.physicsData.size());

        const auto* position = reinterpret_cast<const FileFormat::Model::PackedPosition*>(buffer->GetDataPointer() + asset.positionsOffset);
        REQUIRE(position->x == 1);
        REQUIRE(position->y == 2);
        REQUIRE(position->z == 3);
        REQUIRE(buffer->GetDataPointer()[asset.physicsDataOffset + 2] == 30);

        FileFormat::Model::ModelAsset loaded;
        REQUIRE(FileFormat::Model::ModelAsset::Read(buffer, loaded));

        buffer->writtenData--;
        buffer->readData = 0;
        REQUIRE_FALSE(FileFormat::Model::ModelAsset::Read(buffer, loaded));
    }

    SECTION("Material blocks remain raw contiguous bytes")
    {
        FileFormat::Material::MaterialAsset asset;
        asset.programKey = 0x123456789abcdef0ull;
        asset.programID = 0x88888888u;
        FileFormat::Material::MaterialData data;
        data.parameters.push_back({0x1234u, 0, 4, FileFormat::Material::ParameterType::Float, 1});
        data.defaultParameterData = {10, 20, 30, 40};

        const size_t serializedSize = asset.GetSerializedSize(data);
        std::shared_ptr<Bytebuffer> buffer = Bytebuffer::BorrowRuntime(serializedSize);
        REQUIRE(asset.Save(buffer, data));
        REQUIRE(buffer->writtenData == serializedSize);
        REQUIRE(asset.parameterBlockSize == data.defaultParameterData.size());
        REQUIRE((asset.defaultParameterDataOffset & 15u) == 0);
        REQUIRE(buffer->GetDataPointer()[asset.defaultParameterDataOffset + 2] == 30);

        FileFormat::Material::MaterialAsset loaded;
        REQUIRE(FileFormat::Material::MaterialAsset::Read(buffer, loaded));
        REQUIRE(loaded.programKey == asset.programKey);
        REQUIRE(loaded.programID == asset.programID);
    }

    SECTION("MaterialPack tables remain aligned and directly addressable")
    {
        FileFormat::Material::MaterialPack pack;
        pack.materialABIVersion = 1;
        pack.sourceManifestFingerprint = 11;
        pack.routingFingerprint = 22;
        pack.functionalCookFingerprint = 33;

        FileFormat::Material::MaterialPackData data;
        data.executionGroups.push_back({.numPrograms = 1, .executionGroupID = 2});
        data.programs.push_back({.programKey = 0x123456789abcdef0ull,
                                 .parameterLayoutHash = 44,
                                 .programID = 55,
                                 .parameterDefinitionOffset = 0,
                                 .parameterBlockSize = 96,
                                 .parameterBlockAlignment = 16,
                                 .rasterRoutes = {{{0, 0}, {2, 0}, {4, 0}}},
                                 .numParameterDefinitions = 1});
        data.programLookups.push_back({.programKey = 0x123456789abcdef0ull,
                                       .programIndex = 0});
        data.parameterDefinitions.push_back({0x9876u, 0, 4,
                                             FileFormat::Material::ParameterType::Float, 1});

        std::shared_ptr<Bytebuffer> buffer = Bytebuffer::BorrowRuntime(pack.GetSerializedSize(data));
        REQUIRE(pack.Save(buffer, data));
        REQUIRE((pack.executionGroupsOffset & 15u) == 0);
        REQUIRE((pack.programsOffset & 15u) == 0);
        REQUIRE((pack.programLookupsOffset & 15u) == 0);
        REQUIRE((pack.parameterDefinitionsOffset & 15u) == 0);

        FileFormat::Material::MaterialPack loaded;
        REQUIRE(FileFormat::Material::MaterialPack::Read(buffer, loaded));
        REQUIRE(loaded.materialABIVersion == 1);
        REQUIRE(loaded.numPrograms == 1);
        REQUIRE(loaded.numProgramLookups == 1);

        buffer->writtenData--;
        buffer->readData = 0;
        REQUIRE_FALSE(FileFormat::Material::MaterialPack::Read(buffer, loaded));
    }

    SECTION("Animation samples remain raw contiguous engine math types")
    {
        FileFormat::Animation::AnimationClipAsset asset;
        asset.sampleCount = 1;
        FileFormat::Animation::AnimationClipData data;
        data.rotationSamples.push_back(quat(1.0f, 0.0f, 0.0f, 0.0f));

        const size_t serializedSize = asset.GetSerializedSize(data);
        std::shared_ptr<Bytebuffer> buffer = Bytebuffer::BorrowRuntime(serializedSize);
        REQUIRE(asset.Save(buffer, data));
        REQUIRE(buffer->writtenData == serializedSize);
        REQUIRE((asset.rotationSamplesOffset & 15u) == 0);
        REQUIRE(asset.numRotationSamples == 1);

        const auto* rotation = reinterpret_cast<const quat*>(buffer->GetDataPointer() + asset.rotationSamplesOffset);
        REQUIRE(rotation->w == 1.0f);

        FileFormat::Animation::AnimationClipAsset loaded;
        REQUIRE(FileFormat::Animation::AnimationClipAsset::Read(buffer, loaded));
    }

    SECTION("Map Model V2 allocation hints round trip without becoming validation requirements")
    {
        Map::ModelResourceAllocationHints first;
        first.models = 2;
        first.meshes = 3;
        first.meshletTriangleRecords = 17;
        Map::ModelResourceAllocationHints second;
        second.models = 5;
        second.meshes = 7;
        second.meshletTriangleRecords = 19;
        first += second;
        REQUIRE(first.models == 7);
        REQUIRE(first.meshes == 10);
        REQUIRE(first.meshletTriangleRecords == 36);

        Map::ModelSceneAllocationHints scene;
        scene.rootPlacements = 11;
        scene.selectedRenderableEmbeddedInstances = 13;
        scene.totalModelInstances = 1; // Deliberately not arithmetically related: hints are not validation.
        scene.geometryGroupMaskWords = 23;
        scene.meshletHistoryWords = 29;

        Map::MapHeader asset;
        asset.modelAllocationHints.resources = first;
        asset.modelAllocationHints.scene = scene;
        asset.modelAllocationHints.flags = Map::ModelAllocationHintFlags_SceneCountsAreUpperBounds;
        asset.chunkHashes = { 0x1234u, 0x5678u };

        std::shared_ptr<Bytebuffer> buffer = Bytebuffer::BorrowRuntime(4096);
        REQUIRE(asset.Save(buffer));
        buffer->readData = 0;

        Map::MapHeader loaded;
        REQUIRE(Map::MapHeader::Read(buffer, loaded));
        REQUIRE(loaded.modelAllocationHints.resources.models == 7);
        REQUIRE(loaded.modelAllocationHints.resources.meshes == 10);
        REQUIRE(loaded.modelAllocationHints.resources.meshletTriangleRecords == 36);
        REQUIRE(loaded.modelAllocationHints.scene.rootPlacements == 11);
        REQUIRE(loaded.modelAllocationHints.scene.totalModelInstances == 1);
        REQUIRE(loaded.modelAllocationHints.scene.meshletHistoryWords == 29);
        REQUIRE(loaded.modelAllocationHints.flags == Map::ModelAllocationHintFlags_SceneCountsAreUpperBounds);
        REQUIRE(loaded.chunkHashes == asset.chunkHashes);
    }
}

TEST_CASE("Animation development ABI matches the independent NBS conformance fixtures", "[FileFormat][Animation]")
{
    using namespace FileFormat::Animation;

    SECTION("Non-empty Skeleton bytes, sections, padding, and rejection behavior are locked")
    {
        SkeletonAsset asset;
        asset.rigFamilyAssetID = 0x1122334455667788ull;
        asset.flags = 0xA5A5A5A5u;

        SkeletonData data;
        SkeletonJoint root;
        root.semanticID = 0x0102030405060708ull;
        root.restTransform = MakeLocalTransform(vec3(1.0f, 2.0f, 3.0f), quat(1.0f, 0.0f, 0.0f, 0.0f), vec3(1.0f, 2.0f, 1.0f));
        data.joints.push_back(root);

        SkeletonJoint child;
        child.semanticID = 0x1112131415161718ull;
        child.parentJointIndex = 0;
        child.familyJointIndex = 1;
        child.hierarchyDepth = 1;
        child.flags = SkeletonJointFlags_Deformation;
        child.restTransform = MakeLocalTransform(vec3(0.0f, 2.0f, 0.0f), quat(0.7071067690849304f, 0.0f, 0.0f, 0.7071067690849304f));
        child.inverseBindTransform[0] = vec3(1.0f, 0.0f, 0.0f);
        child.inverseBindTransform[1] = vec3(0.0f, 1.0f, 0.0f);
        child.inverseBindTransform[2] = vec3(0.0f, 0.0f, 1.0f);
        child.inverseBindTransform[3] = vec3(-1.0f, -4.0f, -3.0f);
        data.joints.push_back(child);

        SkeletonFamilyBinding binding;
        binding.skeletonJointIndex = 1;
        binding.familyJointIndex = 7;
        binding.skeletonToFamily = MakeLocalTransform(vec3(0.25f, 0.0f, 0.0f));
        binding.familyToSkeleton = MakeLocalTransform(vec3(-0.25f, 0.0f, 0.0f));
        data.familyBindings.push_back(binding);

        SkeletonPropagationRule rule;
        rule.sourceJointIndex = 0;
        rule.targetJointIndex = 1;
        rule.type = SkeletonPropagationType::DistributeRotation;
        rule.flags = 3;
        rule.executionOrder = 4;
        rule.weight = 0.5f;
        rule.offsetTransform = MakeLocalTransform(vec3(0.0f, 0.5f, 0.0f));
        data.propagationRules.push_back(rule);
        data.hierarchyDepthRanges = {{0, 1}, {1, 1}};
        data.hierarchyDepthJointIndices = {0, 1};

        SkeletonAttachment attachment;
        attachment.semanticID = 0x2122232425262728ull;
        attachment.jointIndex = 1;
        attachment.flags = 9;
        attachment.localTransform = MakeLocalTransform(vec3(0.0f, 0.0f, 0.5f));
        data.attachments.push_back(attachment);

        std::shared_ptr<Bytebuffer> buffer = Bytebuffer::BorrowRuntime(asset.GetSerializedSize(data));
        REQUIRE(asset.Save(buffer, data));
        REQUIRE(buffer->writtenData == 592);
        CHECK(Fnv1a64(buffer->GetDataPointer(), buffer->writtenData) == 0xCF94CCA9D382C8D4ull);
        CHECK(asset.jointsOffset == 80);
        CHECK(asset.familyBindingsOffset == 320);
        CHECK(asset.propagationRulesOffset == 432);
        CHECK(asset.hierarchyDepthRangesOffset == 496);
        CHECK(asset.hierarchyDepthJointIndicesOffset == 512);
        CHECK(asset.attachmentsOffset == 528);
        CHECK(std::all_of(buffer->GetDataPointer() + 72, buffer->GetDataPointer() + 80, [](u8 value) { return value == 0; }));

        SkeletonAsset loaded;
        REQUIRE(SkeletonAsset::Read(buffer, loaded));
        CHECK(loaded.rigFamilyAssetID == asset.rigFamilyAssetID);
        CHECK(loaded.numJoints == 2);
        CHECK(loaded.numAttachments == 1);

        buffer->writtenData = sizeof(SkeletonAsset) - 1;
        buffer->readData = 0;
        REQUIRE_FALSE(SkeletonAsset::Read(buffer, loaded));
        buffer->writtenData = 592;
        auto* serialized = reinterpret_cast<SkeletonAsset*>(buffer->GetDataPointer());
        serialized->jointsOffset = 81;
        buffer->readData = 0;
        REQUIRE_FALSE(SkeletonAsset::Read(buffer, loaded));
        serialized->jointsOffset = asset.jointsOffset;
        serialized->numJoints = std::numeric_limits<u32>::max();
        buffer->readData = 0;
        REQUIRE_FALSE(SkeletonAsset::Read(buffer, loaded));
        serialized->numJoints = asset.numJoints;
        serialized->attachmentsOffset = 16;
        serialized->numAttachments = 0;
        buffer->readData = 0;
        REQUIRE_FALSE(SkeletonAsset::Read(buffer, loaded));
    }

    SECTION("Non-empty AnimationClip bytes, sections, transforms, and rejection behavior are locked")
    {
        AnimationClipAsset asset;
        asset.sourcePoseDomainAssetID = 0x3132333435363738ull;
        asset.durationMicroseconds = 1'000'000;
        asset.sampleRateHz = 2;
        asset.sourcePoseDomainType = PoseDomainType::Skeleton;
        asset.flags = AnimationClipFlags_Looping;
        asset.sampleCount = 3;

        AnimationClipData data;
        AnimationTrack sampled;
        sampled.sourceJointIndex = 0;
        sampled.flags = AnimationTrackFlags_HasTranslationSamples | AnimationTrackFlags_HasRotationSamples | AnimationTrackFlags_HasScaleSamples;
        sampled.numTranslations = 3;
        sampled.numRotations = 3;
        sampled.numScales = 3;
        data.tracks.push_back(sampled);
        AnimationTrack defaults;
        defaults.sourceJointIndex = 1;
        defaults.defaultTranslation = vec3(0.0f, 2.0f, 0.0f);
        defaults.defaultRotation = quat(0.7071067690849304f, 0.0f, 0.0f, 0.7071067690849304f);
        data.tracks.push_back(defaults);
        data.translationSamples = {vec3(0.0f), vec3(0.5f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)};
        data.rotationSamples = {quat(1.0f, 0.0f, 0.0f, 0.0f), quat(0.7071067690849304f, 0.0f, 0.0f, 0.7071067690849304f), quat(0.0f, 0.0f, 0.0f, 1.0f)};
        data.scaleSamples = {vec3(1.0f), vec3(1.0f, 1.5f, 1.0f), vec3(1.0f, 2.0f, 1.0f)};
        data.synchronizationMarkers = {{0x4142434445464748ull, 0, 1}, {0x5152535455565758ull, 500'000, 2}};
        data.actionWindows = {{0x6162636465666768ull, 250'000, 750'000}};
        data.events = {{500'000, 0x71727374u, 0x81828384u, 5, 1, 3}};
        data.eventPayloadBytes = {0x10, 0x20, 0x30, 0x40, 0x50};

        std::shared_ptr<Bytebuffer> buffer = Bytebuffer::BorrowRuntime(asset.GetSerializedSize(data));
        REQUIRE(asset.Save(buffer, data));
        REQUIRE(buffer->writtenData == 501);
        CHECK(Fnv1a64(buffer->GetDataPointer(), buffer->writtenData) == 0x1925CD4355FF7D5Dull);
        CHECK(asset.tracksOffset == 112);
        CHECK(asset.translationSamplesOffset == 272);
        CHECK(asset.rotationSamplesOffset == 320);
        CHECK(asset.scaleSamplesOffset == 368);
        CHECK(asset.synchronizationMarkersOffset == 416);
        CHECK(asset.actionWindowsOffset == 448);
        CHECK(asset.eventsOffset == 464);
        CHECK(asset.eventPayloadBytesOffset == 496);
        CHECK(std::all_of(buffer->GetDataPointer() + 308, buffer->GetDataPointer() + 320, [](u8 value) { return value == 0; }));
        const auto* firstRotation = reinterpret_cast<const quat*>(buffer->GetDataPointer() + asset.rotationSamplesOffset);
        CHECK(firstRotation->x == 0.0f);
        CHECK(firstRotation->y == 0.0f);
        CHECK(firstRotation->z == 0.0f);
        CHECK(firstRotation->w == 1.0f);

        AnimationClipAsset loaded;
        REQUIRE(AnimationClipAsset::Read(buffer, loaded));
        CHECK(loaded.sourcePoseDomainType == PoseDomainType::Skeleton);
        CHECK(loaded.sampleCount == 3);

        buffer->writtenData--;
        buffer->readData = 0;
        REQUIRE_FALSE(AnimationClipAsset::Read(buffer, loaded));
        buffer->writtenData = 501;
        auto* serialized = reinterpret_cast<AnimationClipAsset*>(buffer->GetDataPointer());
        serialized->rotationSamplesOffset = 319;
        buffer->readData = 0;
        REQUIRE_FALSE(AnimationClipAsset::Read(buffer, loaded));
        serialized->rotationSamplesOffset = asset.rotationSamplesOffset;
        serialized->numEvents = std::numeric_limits<u32>::max();
        buffer->readData = 0;
        REQUIRE_FALSE(AnimationClipAsset::Read(buffer, loaded));
        serialized->numEvents = asset.numEvents;
        serialized->eventPayloadBytesOffset = 16;
        serialized->numEventPayloadBytes = 0;
        buffer->readData = 0;
        REQUIRE_FALSE(AnimationClipAsset::Read(buffer, loaded));
    }
}

TEST_CASE("ShaderPack serialization clears raw record padding", "[FileFormat]")
{
    std::array<u8, 4> shaderData = {3, 2, 1, 0};
    FileFormat::ShaderInMemory shader;
    shader.permutationNameHash = 0x12345678u;
    shader.data = shaderData.data();
    shader.size = static_cast<u32>(shaderData.size());

    const std::filesystem::path path = std::filesystem::temp_directory_path() /
        ("novus-shaderpack-padding-" +
         std::to_string(reinterpret_cast<uintptr_t>(&shader)) + ".shaderpack");
    FileFormat::ShaderPack pack;
    REQUIRE(pack.Save(path.string(), {shader}));

    std::ifstream input(path, std::ios::binary);
    const std::vector<u8> bytes{
        std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
    REQUIRE(bytes.size() >= sizeof(FileHeader) + sizeof(FileFormat::ShaderPackManifest) +
                                sizeof(FileFormat::ShaderRef));

    const size_t recordOffset = sizeof(FileHeader) + sizeof(FileFormat::ShaderPackManifest);
    const auto IsZero = [&](size_t begin, size_t end) {
        return std::all_of(bytes.begin() + recordOffset + begin,
                           bytes.begin() + recordOffset + end,
                           [](u8 value) { return value == 0; });
    };
    CHECK(IsZero(sizeof(u32), offsetof(FileFormat::ShaderRef, dataOffset)));
    CHECK(IsZero(offsetof(FileFormat::ShaderRef, dataSize) + sizeof(u32),
                 offsetof(FileFormat::ShaderRef, reflectionOffset)));
    CHECK(IsZero(offsetof(FileFormat::ShaderRef, reflectionSize) + sizeof(u32),
                 sizeof(FileFormat::ShaderRef)));

    std::error_code ignored;
    std::filesystem::remove(path, ignored);
}

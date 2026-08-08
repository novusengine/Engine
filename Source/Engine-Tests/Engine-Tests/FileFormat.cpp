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

        const size_t serializedSize = asset.GetSerializedSize(data);
        std::shared_ptr<Bytebuffer> buffer = Bytebuffer::BorrowRuntime(serializedSize);
        REQUIRE(asset.Save(buffer, data));
        REQUIRE(buffer->writtenData == serializedSize);
        REQUIRE((asset.positionsOffset & 15u) == 0);
        REQUIRE(asset.numPositions == 1);

        const auto* position = reinterpret_cast<const FileFormat::Model::PackedPosition*>(buffer->GetDataPointer() + asset.positionsOffset);
        REQUIRE(position->x == 1);
        REQUIRE(position->y == 2);
        REQUIRE(position->z == 3);

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

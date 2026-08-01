#include <catch2/catch2.hpp>

#include <Base/Memory/Bytebuffer.h>
#include <FileFormat/Novus/Animation/Animation.h>
#include <FileFormat/Novus/Model/Material.h>
#include <FileFormat/Novus/Model/Model.h>

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
}

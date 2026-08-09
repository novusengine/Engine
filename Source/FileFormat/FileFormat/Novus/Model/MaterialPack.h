#pragma once

#include "FileFormat/Novus/FileHeader.h"
#include "FileFormat/Novus/Model/Material.h"
#include "FileFormat/Novus/Model/MaterialABI.h"

#include <Base/Types.h>

#include <array>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <vector>

namespace FileFormat::Material
{
    inline constexpr char MATERIAL_PACK_FILE_EXTENSION[] = ".matpack";
    inline constexpr u32 MATERIAL_PACK_DEVELOPMENT_VERSION = 0;
    inline constexpr FileHeader::Type MATERIAL_PACK_FILE_TYPE = FileHeader::Type::MaterialPack;

    enum MaterialExecutionGroupFlags : u16
    {
        MaterialExecutionGroupFlags_None = 0,
        MaterialExecutionGroupFlags_HasCoverageShader = 1u << 0
    };

    // Routes one bounded execution group to its cooked shader permutations.
    struct MaterialExecutionGroup
    {
        u32 resolveShaderPermutationHash = 0;
        u32 coverageShaderPermutationHash = 0;
        u32 forwardShaderPermutationHash = 0;
        u32 numPrograms = 0;
        u16 executionGroupID = 0;
        u16 flags = MaterialExecutionGroupFlags_None;
        u32 reserved0 = 0;
        u32 reserved1 = 0;
        u32 reserved2 = 0;
    };

    struct MaterialProgramRoute
    {
        u16 executionGroupID = 0;
        u16 groupLocalProgramID = 0;
    };

    // Joins one source Material identity to stable shader routing and parameter reflection.
    struct MaterialProgramRecord
    {
        MaterialProgramKey programKey = INVALID_MATERIAL_PROGRAM_KEY;
        u64 parameterLayoutHash = 0;
        u32 programID = 0;
        u32 flags = MaterialFlags_None;
        u32 parameterDefinitionOffset = 0;
        u32 parameterBlockSize = 0;
        u32 parameterBlockAlignment = 0;
        std::array<MaterialProgramRoute, 3> rasterRoutes;
        u16 numParameterDefinitions = 0;
        u16 reserved0 = 0;
        u32 reserved1 = 0;
    };

    // Provides a key-sorted lookup without constraining the GPU-facing program-table order.
    struct MaterialProgramLookup
    {
        MaterialProgramKey programKey = INVALID_MATERIAL_PROGRAM_KEY;
        u32 programIndex = 0;
        u32 reserved = 0;
    };

    struct MaterialPackData
    {
        std::vector<MaterialExecutionGroup> executionGroups;
        std::vector<MaterialProgramRecord> programs;
        std::vector<MaterialProgramLookup> programLookups;
        std::vector<ParameterDefinition> parameterDefinitions;
    };

    // Contains the CPU-side routing and reflection metadata for the globally cooked Material library.
    struct MaterialPack
    {
        FileHeader header = FileHeader(MATERIAL_PACK_FILE_TYPE, MATERIAL_PACK_DEVELOPMENT_VERSION);
        u32 materialABIVersion = 0;
        u32 reserved = 0;
        u64 sourceManifestFingerprint = 0;
        u64 routingFingerprint = 0;
        u64 functionalCookFingerprint = 0;
        u32 executionGroupsOffset = 0;
        u32 numExecutionGroups = 0;
        u32 programsOffset = 0;
        u32 numPrograms = 0;
        u32 programLookupsOffset = 0;
        u32 numProgramLookups = 0;
        u32 parameterDefinitionsOffset = 0;
        u32 numParameterDefinitions = 0;

        size_t GetSerializedSize(const MaterialPackData& data) const;
        bool Save(std::shared_ptr<Bytebuffer>& buffer, const MaterialPackData& data);
        static bool Read(std::shared_ptr<Bytebuffer>& buffer, MaterialPack& out);
    };

    static_assert(sizeof(MaterialExecutionGroup) == 32);
    static_assert(sizeof(MaterialProgramRoute) == 4);
    static_assert(sizeof(MaterialProgramRecord) == 56);
    static_assert(sizeof(MaterialProgramLookup) == 16);
    static_assert(sizeof(MaterialPack) == 72);

    static_assert(std::is_trivially_copyable_v<MaterialExecutionGroup>);
    static_assert(std::is_trivially_copyable_v<MaterialProgramRecord>);
    static_assert(std::is_trivially_copyable_v<MaterialProgramLookup>);
    static_assert(std::is_trivially_copyable_v<MaterialPack>);
    static_assert(std::is_standard_layout_v<MaterialPack>);
} // namespace FileFormat::Material

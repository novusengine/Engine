#pragma once
#include "Renderer/PermutationField.h"

#include <Base/Types.h>
#include <Base/Util/StringUtils.h>

#include <filesystem>

namespace Renderer
{
    inline u32 GetShaderEntryNameHash(const std::string& shaderName, const std::vector<PermutationField>& permutationFields)
    {
        std::filesystem::path shaderPath = shaderName;
        std::string filename = shaderPath.filename().string();

        size_t firstExtensionOffset = filename.find_first_of('.');

        std::string permutationName = filename.substr(0, firstExtensionOffset);
        std::string extension = filename.substr(firstExtensionOffset);

        for (const PermutationField& permutationField : permutationFields)
        {
            permutationName += "-" + permutationField.key + permutationField.value;
        }

        shaderPath = shaderPath.parent_path() / (permutationName + extension);
        std::string shaderPathString = shaderPath.generic_string();

        return StringUtils::fnv1a_32(shaderPathString.c_str(), shaderPathString.length());
    }

    struct ShaderEntry
    {
        std::string debugName;
        u32 permutationNameHash;
        u8* shaderData = nullptr;
        u32 shaderSize = 0;
    };
}
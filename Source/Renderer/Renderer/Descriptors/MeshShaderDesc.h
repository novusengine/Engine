#pragma once
#include "Renderer/ShaderEntry.h"

#include <Base/Types.h>

#include <type_safe/strong_typedef.hpp>

namespace Renderer
{
    struct MeshShaderDesc
    {
        const ShaderEntry* shaderEntry = nullptr;

        void AddPermutationField(const std::string& key, const std::string& value)
        {
            PermutationField& permutationField = permutationFields.emplace_back();
            permutationField.key = key;
            permutationField.value = value;
        }

        std::string path;
        std::vector<PermutationField> permutationFields;
    };

    STRONG_TYPEDEF(MeshShaderID, u16);
}

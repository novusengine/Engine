#pragma once

#include <Base/Types.h>

#include <xxhash/xxhash64.h>

#include <array>
#include <concepts>
#include <string_view>
#include <type_traits>

namespace Gameplay::Faction
{
    namespace ContentHashDetail
    {
        template <std::integral T>
        void AddContentHashValue(XXHash64& hasher, T value)
        {
            using Unsigned = std::make_unsigned_t<T>;

            Unsigned remaining = static_cast<Unsigned>(value);
            std::array<u8, sizeof(Unsigned)> bytes;

            for (u8& byte : bytes)
            {
                byte = static_cast<u8>(remaining & 0xffu);
                remaining >>= 8u;
            }

            hasher.add(bytes.data(), bytes.size());
        }

        template <typename T> requires std::is_enum_v<T>
        void AddContentHashValue(XXHash64& hasher, T value)
        {
            AddContentHashValue(hasher, static_cast<std::underlying_type_t<T>>(value));
        }

        inline void AddContentHashString(XXHash64& hasher, std::string_view value)
        {
            AddContentHashValue(hasher, static_cast<u64>(value.size()));

            if (!value.empty())
                hasher.add(value.data(), value.size());
        }
    }

    template <typename Runtime>
    u64 CalculateRuntimeContentHash(const Runtime& runtime)
    {
        XXHash64 hasher(0);

        ContentHashDetail::AddContentHashValue(hasher, static_cast<u64>(runtime.definitions.size()));
        for (const auto& definition : runtime.definitions)
        {
            ContentHashDetail::AddContentHashValue(hasher, definition.id);
            ContentHashDetail::AddContentHashString(hasher, definition.name);
            ContentHashDetail::AddContentHashValue(hasher, definition.flags);
            ContentHashDetail::AddContentHashValue(hasher, definition.defaultReactionToOthers);
            ContentHashDetail::AddContentHashValue(hasher, definition.defaultPlayerReactionBounds);
            ContentHashDetail::AddContentHashValue(hasher, definition.defaultReputationValue);
        }

        ContentHashDetail::AddContentHashValue(hasher, runtime.wordsPerRelationRow);
        ContentHashDetail::AddContentHashValue(hasher, static_cast<u64>(runtime.packedRelations.size()));
        for (u64 relations : runtime.packedRelations)
        {
            ContentHashDetail::AddContentHashValue(hasher, relations);
        }

        ContentHashDetail::AddContentHashValue(hasher, static_cast<u64>(runtime.standingThresholds.size()));
        for (const auto& standing : runtime.standingThresholds)
        {
            ContentHashDetail::AddContentHashValue(hasher, standing.id);
            ContentHashDetail::AddContentHashString(hasher, standing.name);
            ContentHashDetail::AddContentHashValue(hasher, standing.minimumValue);
            ContentHashDetail::AddContentHashValue(hasher, standing.reaction);
            ContentHashDetail::AddContentHashValue(hasher, standing.sortOrder);
        }

        return hasher.hash();
    }
}

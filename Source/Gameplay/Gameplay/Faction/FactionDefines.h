#pragma once

#include <Base/Types.h>

#include <algorithm>

namespace Gameplay::Faction
{
    using FactionID = u16;
    using StandingID = u16;

    static constexpr FactionID NONE_FACTION_ID = 0;
    static constexpr u16 INHERIT_REACTION_BOUND = 255;

    enum class Reaction : u8
    {
        Hostile = 0,
        Unfriendly = 1,
        Neutral = 2,
        Friendly = 3
    };

    enum class DefinitionFlags : u16
    {
        None = 0,
        AllowsReputation = 1 << 0,
        DiscoverOnInteract = 1 << 1,
        DiscoverOnTarget = 1 << 2,
        HiddenUntilEarned = 1 << 3,
        CanSetAtWar = 1 << 4
    };

    enum class ReputationFlags : u16
    {
        None = 0,
        Visible = 1 << 0,
        Tracked = 1 << 1,
        AtWar = 1 << 2,
        Inactive = 1 << 3,
        Locked = 1 << 4
    };

    static constexpr u16 DEFINITION_FLAG_MASK = static_cast<u16>(DefinitionFlags::AllowsReputation) | static_cast<u16>(DefinitionFlags::DiscoverOnInteract) | static_cast<u16>(DefinitionFlags::DiscoverOnTarget) | static_cast<u16>(DefinitionFlags::HiddenUntilEarned) | static_cast<u16>(DefinitionFlags::CanSetAtWar);
    static constexpr u16 REPUTATION_FLAG_MASK = static_cast<u16>(ReputationFlags::Visible) | static_cast<u16>(ReputationFlags::Tracked) | static_cast<u16>(ReputationFlags::AtWar) | static_cast<u16>(ReputationFlags::Inactive) | static_cast<u16>(ReputationFlags::Locked);

    constexpr bool HasFlag(u16 flags, DefinitionFlags flag)
    {
        return (flags & static_cast<u16>(flag)) != 0;
    }

    constexpr bool IsValidReaction(u16 value)
    {
        return value <= static_cast<u16>(Reaction::Friendly);
    }

    struct ReactionBounds
    {
        constexpr bool IsValid() const
        {
            const u8 minimumValue = static_cast<u8>(minimum);
            const u8 maximumValue = static_cast<u8>(maximum);
            return IsValidReaction(minimumValue) && IsValidReaction(maximumValue) && minimumValue <= maximumValue;
        }

        constexpr u8 Pack() const
        {
            return static_cast<u8>(minimum) | (static_cast<u8>(maximum) << 2);
        }

        constexpr Reaction Clamp(Reaction reaction) const
        {
            const u8 value = std::clamp(static_cast<u8>(reaction), static_cast<u8>(minimum), static_cast<u8>(maximum));
            return static_cast<Reaction>(value);
        }

        static constexpr ReactionBounds Unpack(u8 packed)
        {
            return {
                .minimum = static_cast<Reaction>(packed & 0x3),
                .maximum = static_cast<Reaction>((packed >> 2) & 0x3)
            };
        }

        static constexpr bool IsValidPacked(u8 packed)
        {
            return (packed & 0xF0) == 0 && Unpack(packed).IsValid();
        }

        Reaction minimum = Reaction::Hostile;
        Reaction maximum = Reaction::Friendly;
    };

    static constexpr u8 NEUTRAL_REACTION_BOUNDS = ReactionBounds{ Reaction::Neutral, Reaction::Neutral }.Pack();
}

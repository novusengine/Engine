#pragma once

#include <Array>
#include <Base/Types.h>

namespace MetaGen::Shared::CombatLog
{
    enum class CombatLogEventEnum : u16
    {
        DamageDealt = 0,
        HealingDone = 1,
        Resurrected = 2,
        Count = 3
    };
    struct CombatLogEventEnumMeta
    {
        using Type = u16;

        static constexpr u16 ENUM_ID = 13;
        static constexpr std::string_view ENUM_NAME = "CombatLogEventEnum";
        static constexpr std::array<std::pair<std::string_view, u16>, 4> ENUM_FIELD_LIST = { std::pair("DamageDealt", 0u), std::pair("HealingDone", 1u), std::pair("Resurrected", 2u), std::pair("Count", 3u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(CombatLogEventEnum);
}
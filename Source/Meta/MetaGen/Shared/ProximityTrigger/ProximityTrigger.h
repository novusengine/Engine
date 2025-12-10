#pragma once

#include <Array>
#include <Base/Types.h>

namespace MetaGen::Shared::ProximityTrigger
{
    enum class ProximityTriggerFlagEnum : u8
    {
        None = 0,
        IsServerAuthorative = 1,
        IsServerSideOnly = 2
    };
    struct ProximityTriggerFlagEnumMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 17;
        static constexpr std::string_view ENUM_NAME = "ProximityTriggerFlagEnum";
        static constexpr std::array<std::pair<std::string_view, u8>, 3> ENUM_FIELD_LIST = { std::pair("None", 0u), std::pair("IsServerAuthorative", 1u), std::pair("IsServerSideOnly", 2u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(ProximityTriggerFlagEnum);
}
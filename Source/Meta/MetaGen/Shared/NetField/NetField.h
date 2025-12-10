#pragma once

#include <Array>
#include <Base/Types.h>

namespace MetaGen::Shared::NetField
{
    enum class ObjectNetFieldEnum : u16
    {
        ObjectGUIDLow = 0,
        ObjectGUIDHigh = 1,
        Scale = 2
    };
    struct ObjectNetFieldEnumMeta
    {
        using Type = u16;

        static constexpr u16 ENUM_ID = 14;
        static constexpr std::string_view ENUM_NAME = "ObjectNetFieldEnum";
        static constexpr std::array<std::pair<std::string_view, u16>, 3> ENUM_FIELD_LIST = { std::pair("ObjectGUIDLow", 0u), std::pair("ObjectGUIDHigh", 1u), std::pair("Scale", 2u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(ObjectNetFieldEnum);

    enum class UnitNetFieldEnum : u16
    {
        LevelRaceGenderClassPacked = 0,
        DisplayID = 1
    };
    struct UnitNetFieldEnumMeta
    {
        using Type = u16;

        static constexpr u16 ENUM_ID = 15;
        static constexpr std::string_view ENUM_NAME = "UnitNetFieldEnum";
        static constexpr std::array<std::pair<std::string_view, u16>, 2> ENUM_FIELD_LIST = { std::pair("LevelRaceGenderClassPacked", 0u), std::pair("DisplayID", 1u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(UnitNetFieldEnum);

    enum class UnitLevelRaceGenderClassPackedInfoEnum : u8
    {
        LevelByteOffset = 0,
        LevelBitOffset = 0,
        RaceBitOffset = 0,
        RaceByteOffset = 2,
        GenderByteOffset = 2,
        ClassByteOffset = 2,
        GenderBitSize = 2,
        GenderBitOffset = 7,
        ClassBitSize = 7,
        RaceBitSize = 7,
        ClassBitOffset = 9,
        LevelBitSize = 16
    };
    struct UnitLevelRaceGenderClassPackedInfoEnumMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 16;
        static constexpr std::string_view ENUM_NAME = "UnitLevelRaceGenderClassPackedInfoEnum";
        static constexpr std::array<std::pair<std::string_view, u8>, 12> ENUM_FIELD_LIST = { std::pair("LevelByteOffset", 0u), std::pair("LevelBitOffset", 0u), std::pair("RaceBitOffset", 0u), std::pair("RaceByteOffset", 2u), std::pair("GenderByteOffset", 2u), std::pair("ClassByteOffset", 2u), std::pair("GenderBitSize", 2u), std::pair("GenderBitOffset", 7u), std::pair("ClassBitSize", 7u), std::pair("RaceBitSize", 7u), std::pair("ClassBitOffset", 9u), std::pair("LevelBitSize", 16u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(UnitLevelRaceGenderClassPackedInfoEnum);
}
#pragma once

#include <Array>
#include <Base/Types.h>

namespace MetaGen::Shared::Spell
{
    enum class SpellProcPhaseTypeEnum : u8
    {
        OnSpellCast = 0,
        OnSpellHandleEffect = 1,
        OnSpellFinish = 2,
        OnAuraApply = 3,
        OnAuraHandleEffect = 4,
        OnAuraRemove = 5,
        Count = 6
    };
    struct SpellProcPhaseTypeEnumMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 18;
        static constexpr std::string_view ENUM_NAME = "SpellProcPhaseTypeEnum";
        static constexpr std::array<std::pair<std::string_view, u8>, 7> ENUM_FIELD_LIST = { std::pair("OnSpellCast", 0u), std::pair("OnSpellHandleEffect", 1u), std::pair("OnSpellFinish", 2u), std::pair("OnAuraApply", 3u), std::pair("OnAuraHandleEffect", 4u), std::pair("OnAuraRemove", 5u), std::pair("Count", 6u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(SpellProcPhaseTypeEnum);

    enum class SpellProcPhaseMaskEnum : u32
    {
        None = 0,
        OnSpellCast = 1,
        OnSpellHandleEffect = 2,
        OnSpellFinish = 4,
        OnAuraApply = 8,
        OnAuraHandleEffect = 16,
        OnAuraRemove = 32,
        All = 4294967295
    };
    struct SpellProcPhaseMaskEnumMeta
    {
        using Type = u32;

        static constexpr u16 ENUM_ID = 19;
        static constexpr std::string_view ENUM_NAME = "SpellProcPhaseMaskEnum";
        static constexpr std::array<std::pair<std::string_view, u32>, 8> ENUM_FIELD_LIST = { std::pair("None", 0u), std::pair("OnSpellCast", 1u), std::pair("OnSpellHandleEffect", 2u), std::pair("OnSpellFinish", 4u), std::pair("OnAuraApply", 8u), std::pair("OnAuraHandleEffect", 16u), std::pair("OnAuraRemove", 32u), std::pair("All", 4294967295u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(SpellProcPhaseMaskEnum);

    enum class SpellProcTypeMaskEnum : u32
    {
        None = 0,
        All = 4294967295
    };
    struct SpellProcTypeMaskEnumMeta
    {
        using Type = u32;

        static constexpr u16 ENUM_ID = 20;
        static constexpr std::string_view ENUM_NAME = "SpellProcTypeMaskEnum";
        static constexpr std::array<std::pair<std::string_view, u32>, 2> ENUM_FIELD_LIST = { std::pair("None", 0u), std::pair("All", 4294967295u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(SpellProcTypeMaskEnum);

    enum class SpellProcHitMaskEnum : u64
    {
        None = 0,
        Normal = 1,
        All = 4294967295
    };
    struct SpellProcHitMaskEnumMeta
    {
        using Type = u64;

        static constexpr u16 ENUM_ID = 21;
        static constexpr std::string_view ENUM_NAME = "SpellProcHitMaskEnum";
        static constexpr std::array<std::pair<std::string_view, u64>, 3> ENUM_FIELD_LIST = { std::pair("None", 0ull), std::pair("Normal", 1ull), std::pair("All", 4294967295ull) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(SpellProcHitMaskEnum);

    enum class SpellProcFlagEnum : u64
    {
        None = 0,
        UseUnitICD = 1,
        All = 4294967295
    };
    struct SpellProcFlagEnumMeta
    {
        using Type = u64;

        static constexpr u16 ENUM_ID = 22;
        static constexpr std::string_view ENUM_NAME = "SpellProcFlagEnum";
        static constexpr std::array<std::pair<std::string_view, u64>, 3> ENUM_FIELD_LIST = { std::pair("None", 0ull), std::pair("UseUnitICD", 1ull), std::pair("All", 4294967295ull) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(SpellProcFlagEnum);

    enum class SpellEffectTypeEnum : u8
    {
        Invalid = 0,
        Dummy = 1,
        WeaponDamage = 2,
        AuraApply = 128,
        AuraRemove = 129,
        AuraPeriodicDamage = 130,
        AuraPeriodicHeal = 131,
        Count = 132
    };
    struct SpellEffectTypeEnumMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 23;
        static constexpr std::string_view ENUM_NAME = "SpellEffectTypeEnum";
        static constexpr std::array<std::pair<std::string_view, u8>, 8> ENUM_FIELD_LIST = { std::pair("Invalid", 0u), std::pair("Dummy", 1u), std::pair("WeaponDamage", 2u), std::pair("AuraApply", 128u), std::pair("AuraRemove", 129u), std::pair("AuraPeriodicDamage", 130u), std::pair("AuraPeriodicHeal", 131u), std::pair("Count", 132u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(SpellEffectTypeEnum);

    enum class SpellEffectWeaponDamageValueEnum : u8
    {
        Invalid = 0,
        Base = 1,
        Normalized = 2,
        Count = 3
    };
    struct SpellEffectWeaponDamageValueEnumMeta
    {
        using Type = u8;

        static constexpr u16 ENUM_ID = 24;
        static constexpr std::string_view ENUM_NAME = "SpellEffectWeaponDamageValueEnum";
        static constexpr std::array<std::pair<std::string_view, u8>, 4> ENUM_FIELD_LIST = { std::pair("Invalid", 0u), std::pair("Base", 1u), std::pair("Normalized", 2u), std::pair("Count", 3u) };
    };
    DECLARE_GENERIC_BITWISE_OPERATORS(SpellEffectWeaponDamageValueEnum);
}
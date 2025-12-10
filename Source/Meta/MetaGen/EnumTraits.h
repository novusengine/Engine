#pragma once

#include "MetaGen/PacketList.h"
#include "MetaGen/Game/Lua/Lua.h"
#include "MetaGen/Server/Lua/Lua.h"
#include "MetaGen/Shared/Cheat/Cheat.h"
#include "MetaGen/Shared/CombatLog/CombatLog.h"
#include "MetaGen/Shared/NetField/NetField.h"
#include "MetaGen/Shared/ProximityTrigger/ProximityTrigger.h"
#include "MetaGen/Shared/Spell/Spell.h"
#include "MetaGen/Shared/Unit/Unit.h"

#include <Base/Types.h>

template <>
struct EnumTraits<MetaGen::Game::Lua::LuaHandlerTypeEnum>
{
    using Meta = MetaGen::Game::Lua::LuaHandlerTypeEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Game::Lua::GameEvent>
{
    using Meta = MetaGen::Game::Lua::GameEventMeta;
};

template <>
struct EnumTraits<MetaGen::Game::Lua::UnitEvent>
{
    using Meta = MetaGen::Game::Lua::UnitEventMeta;
};

template <>
struct EnumTraits<MetaGen::Game::Lua::ContainerEvent>
{
    using Meta = MetaGen::Game::Lua::ContainerEventMeta;
};

template <>
struct EnumTraits<MetaGen::Game::Lua::TriggerEvent>
{
    using Meta = MetaGen::Game::Lua::TriggerEventMeta;
};

template <>
struct EnumTraits<MetaGen::Server::Lua::LuaHandlerTypeEnum>
{
    using Meta = MetaGen::Server::Lua::LuaHandlerTypeEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Server::Lua::ServerEvent>
{
    using Meta = MetaGen::Server::Lua::ServerEventMeta;
};

template <>
struct EnumTraits<MetaGen::Server::Lua::CharacterEvent>
{
    using Meta = MetaGen::Server::Lua::CharacterEventMeta;
};

template <>
struct EnumTraits<MetaGen::Server::Lua::TriggerEvent>
{
    using Meta = MetaGen::Server::Lua::TriggerEventMeta;
};

template <>
struct EnumTraits<MetaGen::Server::Lua::SpellEvent>
{
    using Meta = MetaGen::Server::Lua::SpellEventMeta;
};

template <>
struct EnumTraits<MetaGen::Server::Lua::AuraEvent>
{
    using Meta = MetaGen::Server::Lua::AuraEventMeta;
};

template <>
struct EnumTraits<MetaGen::Server::Lua::CreatureAIEvent>
{
    using Meta = MetaGen::Server::Lua::CreatureAIEventMeta;
};

template <>
struct EnumTraits<MetaGen::Shared::Cheat::CheatCommandEnum>
{
    using Meta = MetaGen::Shared::Cheat::CheatCommandEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Shared::CombatLog::CombatLogEventEnum>
{
    using Meta = MetaGen::Shared::CombatLog::CombatLogEventEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Shared::NetField::ObjectNetFieldEnum>
{
    using Meta = MetaGen::Shared::NetField::ObjectNetFieldEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Shared::NetField::UnitNetFieldEnum>
{
    using Meta = MetaGen::Shared::NetField::UnitNetFieldEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Shared::NetField::UnitLevelRaceGenderClassPackedInfoEnum>
{
    using Meta = MetaGen::Shared::NetField::UnitLevelRaceGenderClassPackedInfoEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Shared::ProximityTrigger::ProximityTriggerFlagEnum>
{
    using Meta = MetaGen::Shared::ProximityTrigger::ProximityTriggerFlagEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Shared::Spell::SpellProcPhaseTypeEnum>
{
    using Meta = MetaGen::Shared::Spell::SpellProcPhaseTypeEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Shared::Spell::SpellProcPhaseMaskEnum>
{
    using Meta = MetaGen::Shared::Spell::SpellProcPhaseMaskEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Shared::Spell::SpellProcTypeMaskEnum>
{
    using Meta = MetaGen::Shared::Spell::SpellProcTypeMaskEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Shared::Spell::SpellProcHitMaskEnum>
{
    using Meta = MetaGen::Shared::Spell::SpellProcHitMaskEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Shared::Spell::SpellProcFlagEnum>
{
    using Meta = MetaGen::Shared::Spell::SpellProcFlagEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Shared::Spell::SpellEffectTypeEnum>
{
    using Meta = MetaGen::Shared::Spell::SpellEffectTypeEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Shared::Spell::SpellEffectWeaponDamageValueEnum>
{
    using Meta = MetaGen::Shared::Spell::SpellEffectWeaponDamageValueEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Shared::Unit::PowerTypeEnum>
{
    using Meta = MetaGen::Shared::Unit::PowerTypeEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Shared::Unit::StatTypeEnum>
{
    using Meta = MetaGen::Shared::Unit::StatTypeEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Shared::Unit::ResistanceTypeEnum>
{
    using Meta = MetaGen::Shared::Unit::ResistanceTypeEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Shared::Unit::ItemEquipSlotEnum>
{
    using Meta = MetaGen::Shared::Unit::ItemEquipSlotEnumMeta;
};

template <>
struct EnumTraits<MetaGen::Shared::Unit::UnitClassEnum>
{
    using Meta = MetaGen::Shared::Unit::UnitClassEnumMeta;
};

template <>
struct EnumTraits<MetaGen::PacketListEnum>
{
    using Meta = MetaGen::PacketListEnumMeta;
};
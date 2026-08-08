#include "GameDefine.h"

#include <Base/Memory/Bytebuffer.h>
#include <Base/Util/DebugHandler.h>

#include <string>
#include <memory>

namespace GameDefine
{
    namespace Database
    {
        bool ItemTemplate::Read(Bytebuffer* buffer, ItemTemplate& result)
        {
            bool didFail = false;

            didFail |= !buffer->GetU32(result.id);
            didFail |= !buffer->GetU32(result.displayID);
            didFail |= !buffer->GetU8(result.bind);
            didFail |= !buffer->GetU8(result.rarity);
            didFail |= !buffer->GetU8(result.category);
            didFail |= !buffer->GetU8(result.type);
            didFail |= !buffer->GetU16(result.virtualLevel);
            didFail |= !buffer->GetU16(result.requiredLevel);
            didFail |= !buffer->GetU32(result.durability);
            didFail |= !buffer->GetU32(result.iconID);

            didFail |= !buffer->GetString(result.name);
            didFail |= !buffer->GetString(result.description);

            didFail |= !buffer->GetU32(result.armor);
            didFail |= !buffer->GetU32(result.statTemplateID);
            didFail |= !buffer->GetU32(result.armorTemplateID);
            didFail |= !buffer->GetU32(result.weaponTemplateID);
            didFail |= !buffer->GetU32(result.shieldTemplateID);

            bool succeeded = !didFail;
            return succeeded;
        }
        bool ItemTemplate::Write(Bytebuffer* buffer, const ItemTemplate& data)
        {
            bool didFail = false;

            didFail |= !buffer->PutU32(data.id);
            didFail |= !buffer->PutU32(data.displayID);
            didFail |= !buffer->PutU8(data.bind);
            didFail |= !buffer->PutU8(data.rarity);
            didFail |= !buffer->PutU8(data.category);
            didFail |= !buffer->PutU8(data.type);
            didFail |= !buffer->PutU16(data.virtualLevel);
            didFail |= !buffer->PutU16(data.requiredLevel);
            didFail |= !buffer->PutU32(data.durability);
            didFail |= !buffer->PutU32(data.iconID);

            didFail |= !buffer->PutString(data.name);
            didFail |= !buffer->PutString(data.description);

            didFail |= !buffer->PutU32(data.armor);
            didFail |= !buffer->PutU32(data.statTemplateID);
            didFail |= !buffer->PutU32(data.armorTemplateID);
            didFail |= !buffer->PutU32(data.weaponTemplateID);
            didFail |= !buffer->PutU32(data.shieldTemplateID);

            bool succeeded = !didFail;
            return succeeded;
        }

        bool ItemStatTemplate::Read(Bytebuffer* buffer, ItemStatTemplate& result)
        {
            bool succeeded = buffer->Get(result);
            return succeeded;
        }
        bool ItemStatTemplate::Write(Bytebuffer* buffer, const ItemStatTemplate& data)
        {
            bool succeeded = buffer->Put(data);
            return succeeded;
        }

        bool ItemArmorTemplate::Read(Bytebuffer* buffer, ItemArmorTemplate& result)
        {
            bool succeeded = buffer->Get(result);
            return succeeded;
        }
        bool ItemArmorTemplate::Write(Bytebuffer* buffer, const ItemArmorTemplate& data)
        {
            bool succeeded = buffer->Put(data);
            return succeeded;
        }

        bool ItemWeaponTemplate::Read(Bytebuffer* buffer, ItemWeaponTemplate& result)
        {
            bool succeeded = buffer->Get(result);
            return succeeded;
        }
        bool ItemWeaponTemplate::Write(Bytebuffer* buffer, const ItemWeaponTemplate& data)
        {
            bool succeeded = buffer->Put(data);
            return succeeded;
        }

        bool ItemShieldTemplate::Read(Bytebuffer* buffer, ItemShieldTemplate& result)
        {
            bool succeeded = buffer->Get(result);
            return succeeded;
        }
        bool ItemShieldTemplate::Write(Bytebuffer* buffer, const ItemShieldTemplate& data)
        {
            bool succeeded = buffer->Put(data);
            return succeeded;
        }

        bool CreatureTemplate::Read(Bytebuffer* buffer, CreatureTemplate& result)
        {
            bool didFail = false;

            didFail |= !buffer->GetU32(result.id);
            didFail |= !buffer->GetU32(result.flags);
            didFail |= !buffer->GetString(result.name);
            didFail |= !buffer->GetString(result.subname);

            didFail |= !buffer->GetU32(result.displayID);
            didFail |= !buffer->GetF32(result.scale);

            didFail |= !buffer->GetU16(result.minLevel);
            didFail |= !buffer->GetU16(result.maxLevel);
            didFail |= !buffer->GetF32(result.armorMod);
            didFail |= !buffer->GetF32(result.healthMod);
            didFail |= !buffer->GetF32(result.resourceMod);
            didFail |= !buffer->GetF32(result.damageMod);
            didFail |= !buffer->GetString(result.scriptName);

            bool succeeded = !didFail;
            return succeeded;
        }
        bool CreatureTemplate::Write(Bytebuffer* buffer, const CreatureTemplate& data)
        {
            bool didFail = false;

            didFail |= !buffer->PutU32(data.id);
            didFail |= !buffer->PutU32(data.flags);
            didFail |= !buffer->PutString(data.name);
            didFail |= !buffer->PutString(data.subname);

            didFail |= !buffer->PutU32(data.displayID);
            didFail |= !buffer->PutF32(data.scale);

            didFail |= !buffer->PutU16(data.minLevel);
            didFail |= !buffer->PutU16(data.maxLevel);
            didFail |= !buffer->PutF32(data.armorMod);
            didFail |= !buffer->PutF32(data.healthMod);
            didFail |= !buffer->PutF32(data.resourceMod);
            didFail |= !buffer->PutF32(data.damageMod);
            didFail |= !buffer->PutString(data.scriptName);

            bool succeeded = !didFail;
            return succeeded;
        }

        bool Map::Read(Bytebuffer* buffer, Map& result)
        {
            bool didFail = false;

            didFail |= !buffer->GetU32(result.id);
            didFail |= !buffer->GetU32(result.flags);
            didFail |= !buffer->GetString(result.internalName);
            didFail |= !buffer->GetString(result.name);

            didFail |= !buffer->GetU16(result.type);
            didFail |= !buffer->GetU16(result.maxPlayers);

            bool succeeded = !didFail;
            return succeeded;
        }
        bool Map::Write(Bytebuffer* buffer, const Map& data)
        {
            bool didFail = false;

            didFail |= !buffer->PutU32(data.id);
            didFail |= !buffer->PutU32(data.flags);

            didFail |= !buffer->PutString(data.internalName);
            didFail |= !buffer->PutString(data.name);

            didFail |= !buffer->PutU16(data.type);
            didFail |= !buffer->PutU16(data.maxPlayers);

            bool succeeded = !didFail;
            return succeeded;
        }

        bool MapLocation::Read(Bytebuffer* buffer, MapLocation& result)
        {
            bool didFail = false;

            didFail |= !buffer->GetU32(result.id);
            didFail |= !buffer->GetString(result.name);

            didFail |= !buffer->GetU32(result.mapID);
            didFail |= !buffer->GetF32(result.positionX);
            didFail |= !buffer->GetF32(result.positionY);
            didFail |= !buffer->GetF32(result.positionZ);
            didFail |= !buffer->GetF32(result.orientation);

            bool succeeded = !didFail;
            return succeeded;
        }
        bool MapLocation::Write(Bytebuffer* buffer, const MapLocation& data)
        {
            bool didFail = false;

            didFail |= !buffer->PutU32(data.id);
            didFail |= !buffer->PutString(data.name);

            didFail |= !buffer->PutU32(data.mapID);
            didFail |= !buffer->PutF32(data.positionX);
            didFail |= !buffer->PutF32(data.positionY);
            didFail |= !buffer->PutF32(data.positionZ);
            didFail |= !buffer->PutF32(data.orientation);

            bool succeeded = !didFail;
            return succeeded;
        }

        bool Spell::Read(Bytebuffer* buffer, Spell& result)
        {
            bool didFail = false;

            didFail |= !buffer->GetU32(result.id);
            didFail |= !buffer->GetString(result.name);
            didFail |= !buffer->GetString(result.description);
            didFail |= !buffer->GetString(result.auraDescription);
            didFail |= !buffer->GetU32(result.iconID);
            didFail |= !buffer->GetF32(result.castTime);
            didFail |= !buffer->GetF32(result.cooldown);
            didFail |= !buffer->GetU8(result.targetSelector);
            didFail |= !buffer->GetU8(result.targetShape);
            didFail |= !buffer->GetU8(result.targetRelation);
            didFail |= !buffer->GetU8(result.targetRecipientMask);
            didFail |= !buffer->GetU8(result.rangePolicy);
            didFail |= !buffer->GetF32(result.minimumRange);
            didFail |= !buffer->GetF32(result.maximumRange);
            didFail |= !buffer->GetF32(result.targetRadius);
            didFail |= !buffer->GetU16(result.maximumTargets);

            bool succeeded = !didFail;
            return succeeded;
        }
        bool Spell::Write(Bytebuffer* buffer, const Spell& data)
        {
            bool didFail = false;

            didFail |= !buffer->PutU32(data.id);
            didFail |= !buffer->PutString(data.name);
            didFail |= !buffer->PutString(data.description);
            didFail |= !buffer->PutString(data.auraDescription);
            didFail |= !buffer->PutU32(data.iconID);
            didFail |= !buffer->PutF32(data.castTime);
            didFail |= !buffer->PutF32(data.cooldown);
            didFail |= !buffer->PutU8(data.targetSelector);
            didFail |= !buffer->PutU8(data.targetShape);
            didFail |= !buffer->PutU8(data.targetRelation);
            didFail |= !buffer->PutU8(data.targetRecipientMask);
            didFail |= !buffer->PutU8(data.rangePolicy);
            didFail |= !buffer->PutF32(data.minimumRange);
            didFail |= !buffer->PutF32(data.maximumRange);
            didFail |= !buffer->PutF32(data.targetRadius);
            didFail |= !buffer->PutU16(data.maximumTargets);

            bool succeeded = !didFail;
            return succeeded;
        }

        bool SpellAura::Read(Bytebuffer* buffer, SpellAura& result)
        {
            bool didFail = false;
            didFail |= !buffer->GetU32(result.spellID);
            didFail |= !buffer->GetF32(result.duration);
            didFail |= !buffer->GetU16(result.stacksPerApplication);
            didFail |= !buffer->GetU16(result.maximumStacks);
            didFail |= !buffer->GetU8(result.applicationPolicy);
            didFail |= !buffer->GetU8(result.disposition);
            didFail |= !buffer->GetU8(result.dispelType);
            didFail |= !buffer->GetU8(result.lifecycleFlags);
            return !didFail;
        }

        bool SpellAura::Write(Bytebuffer* buffer, const SpellAura& data)
        {
            bool didFail = false;
            didFail |= !buffer->PutU32(data.spellID);
            didFail |= !buffer->PutF32(data.duration);
            didFail |= !buffer->PutU16(data.stacksPerApplication);
            didFail |= !buffer->PutU16(data.maximumStacks);
            didFail |= !buffer->PutU8(data.applicationPolicy);
            didFail |= !buffer->PutU8(data.disposition);
            didFail |= !buffer->PutU8(data.dispelType);
            didFail |= !buffer->PutU8(data.lifecycleFlags);
            return !didFail;
        }

        bool SpellEffect::Read(Bytebuffer* buffer, SpellEffect& result)
        {
            bool didFail = false;

            didFail |= !buffer->Get(result);

            bool succeeded = !didFail;
            return succeeded;
        }
        bool SpellEffect::Write(Bytebuffer* buffer, const SpellEffect& data)
        {
            bool didFail = false;

            didFail |= !buffer->Put(data);

            bool succeeded = !didFail;
            return succeeded;
        }

        bool SpellProcData::Read(Bytebuffer* buffer, SpellProcData& result)
        {
            bool didFail = false;

            didFail |= !buffer->Get(result);

            bool succeeded = !didFail;
            return succeeded;
        }
        bool SpellProcData::Write(Bytebuffer* buffer, const SpellProcData& data)
        {
            bool didFail = false;

            didFail |= !buffer->Put(data);

            bool succeeded = !didFail;
            return succeeded;
        }
    }
}

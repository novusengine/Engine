#include "GameDefine.h"

#include <Base/Memory/Bytebuffer.h>
#include <Base/Util/DebugHandler.h>

#include <string>
#include <memory>

namespace GameDefine
{
    namespace Database
    {
        bool ItemTemplate::Read(std::shared_ptr<Bytebuffer>& buffer, ItemTemplate& result)
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
        bool ItemTemplate::Write(std::shared_ptr<Bytebuffer>& buffer, const ItemTemplate& data)
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

        bool ItemStatTemplate::Read(std::shared_ptr<Bytebuffer>& buffer, ItemStatTemplate& result)
        {
            bool succeeded = buffer->Get(result);
            return succeeded;
        }
        bool ItemStatTemplate::Write(std::shared_ptr<Bytebuffer>& buffer, const ItemStatTemplate& data)
        {
            bool succeeded = buffer->Put(data);
            return succeeded;
        }

        bool ItemArmorTemplate::Read(std::shared_ptr<Bytebuffer>& buffer, ItemArmorTemplate& result)
        {
            bool succeeded = buffer->Get(result);
            return succeeded;
        }
        bool ItemArmorTemplate::Write(std::shared_ptr<Bytebuffer>& buffer, const ItemArmorTemplate& data)
        {
            bool succeeded = buffer->Put(data);
            return succeeded;
        }

        bool ItemWeaponTemplate::Read(std::shared_ptr<Bytebuffer>& buffer, ItemWeaponTemplate& result)
        {
            bool succeeded = buffer->Get(result);
            return succeeded;
        }
        bool ItemWeaponTemplate::Write(std::shared_ptr<Bytebuffer>& buffer, const ItemWeaponTemplate& data)
        {
            bool succeeded = buffer->Put(data);
            return succeeded;
        }

        bool ItemShieldTemplate::Read(std::shared_ptr<Bytebuffer>& buffer, ItemShieldTemplate& result)
        {
            bool succeeded = buffer->Get(result);
            return succeeded;
        }
        bool ItemShieldTemplate::Write(std::shared_ptr<Bytebuffer>& buffer, const ItemShieldTemplate& data)
        {
            bool succeeded = buffer->Put(data);
            return succeeded;
        }

        bool CreatureTemplate::Read(std::shared_ptr<Bytebuffer>& buffer, CreatureTemplate& result)
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
        bool CreatureTemplate::Write(std::shared_ptr<Bytebuffer>& buffer, const CreatureTemplate& data)
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

        bool Map::Read(std::shared_ptr<Bytebuffer>& buffer, Map& result)
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
        bool Map::Write(std::shared_ptr<Bytebuffer>& buffer, const Map& data)
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

        bool MapLocation::Read(std::shared_ptr<Bytebuffer>& buffer, MapLocation& result)
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
        bool MapLocation::Write(std::shared_ptr<Bytebuffer>& buffer, const MapLocation& data)
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

        bool Spell::Read(std::shared_ptr<Bytebuffer>& buffer, Spell& result)
        {
            bool didFail = false;

            didFail |= !buffer->GetU32(result.id);
            didFail |= !buffer->GetString(result.name);
            didFail |= !buffer->GetString(result.description);
            didFail |= !buffer->GetString(result.auraDescription);
            didFail |= !buffer->GetU32(result.iconID);
            didFail |= !buffer->GetF32(result.castTime);
            didFail |= !buffer->GetF32(result.cooldown);
            didFail |= !buffer->GetF32(result.duration);

            bool succeeded = !didFail;
            return succeeded;
        }
        bool Spell::Write(std::shared_ptr<Bytebuffer>& buffer, const Spell& data)
        {
            bool didFail = false;

            didFail |= !buffer->PutU32(data.id);
            didFail |= !buffer->PutString(data.name);
            didFail |= !buffer->PutString(data.description);
            didFail |= !buffer->PutString(data.auraDescription);
            didFail |= !buffer->PutU32(data.iconID);
            didFail |= !buffer->PutF32(data.castTime);
            didFail |= !buffer->PutF32(data.cooldown);
            didFail |= !buffer->PutF32(data.duration);

            bool succeeded = !didFail;
            return succeeded;
        }

        bool SpellEffect::Read(std::shared_ptr<Bytebuffer>& buffer, SpellEffect& result)
        {
            bool didFail = false;

            didFail |= !buffer->Get(result);

            bool succeeded = !didFail;
            return succeeded;
        }
        bool SpellEffect::Write(std::shared_ptr<Bytebuffer>& buffer, const SpellEffect& data)
        {
            bool didFail = false;

            didFail |= !buffer->Put(data);

            bool succeeded = !didFail;
            return succeeded;
        }

        bool SpellProcData::Read(std::shared_ptr<Bytebuffer>& buffer, SpellProcData& result)
        {
            bool didFail = false;

            didFail |= !buffer->Get(result);

            bool succeeded = !didFail;
            return succeeded;
        }
        bool SpellProcData::Write(std::shared_ptr<Bytebuffer>& buffer, const SpellProcData& data)
        {
            bool didFail = false;

            didFail |= !buffer->Put(data);

            bool succeeded = !didFail;
            return succeeded;
        }
    }
}
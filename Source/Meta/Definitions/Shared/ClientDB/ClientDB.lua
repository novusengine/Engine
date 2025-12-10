local M = OrderedTable()

local Type = require("Type")
local Archetype = require("Archetype")
local Component = require("Component")

M.TextureFileDataRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("texture", Type.STRINGREF),
        Field("materialResourcesID", Type.U32)
    }
}

M.ModelFileDataRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("model", Type.STRINGREF),
        Field("modelResourcesID", Type.U32),
        Field("flags", Type.U8)
    }
}

M.AnimationDataRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("flags", Type.U64),
        Field("fallback", Type.U16),
        Field("behaviorID", Type.U16),
        Field("behaviorTier", Type.U8)
    }
}

M.CameraSaveRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("name", Type.STRINGREF),
        Field("code", Type.STRINGREF)
    }
}

M.CinematicCameraRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("soundID", Type.U32),
        Field("endPosition", Type.VEC3),
        Field("rotation", Type.F32),
        Field("model", Type.STRINGREF)
    }
}

M.CinematicSequenceRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("cameraID", Type.U16)
    }
}

M.CreatureModelDataRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("model", Type.STRINGREF),
        Field("flags", Type.U32),

        Field("boxMin", Type.VEC3),
        Field("boxMax", Type.VEC3),

        Field("soundID", Type.U16),
        Field("sizeClass", Type.U8),
        Field("bloodID", Type.U8),

        Field("footprintTextureID", Type.U8),
        Field("footprintTextureLength", Type.U8),
        Field("footprintTextureWidth", Type.U8),
        Field("footprintParticleScale", Type.U8),
        Field("footstepCameraEffectID", Type.U16),
        Field("deathThudCameraEffectID", Type.U16),

        Field("collisionBox", Type.VEC2),
        Field("mountHeight", Type.F32)
    }
}

M.CreatureDisplayInfoRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("modelID", Type.U32),
        Field("extendedDisplayInfoID", Type.U32),

        Field("soundID", Type.U16),
        Field("npcSoundID", Type.U16),

        Field("flags", Type.U8),
        Field("gender", Type.U8),
        Field("sizeClass", Type.I8),
        Field("bloodID", Type.U8),

        Field("unarmedWeaponType", Type.U8),

        Field("creatureModelAlpha", Type.U8),
        Field("creatureModelScale", Type.F32),
        Field("creaturePetScale", Type.F32),

        Field("textureVariations", Type.ARRAY, { type = Type.STRINGREF, count = 4 })
    }
}

M.CreatureDisplayInfoExtraRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("flags", Type.U8),

        Field("raceID", Type.U8),
        Field("gender", Type.U8),
        Field("classID", Type.U8),

        Field("skinID", Type.U8),
        Field("faceID", Type.U8),
        Field("hairStyleID", Type.U8),
        Field("hairColorID", Type.U8),
        Field("facialHairID", Type.U8),

        Field("bakedTexture", Type.STRINGREF)
    }
}

M.CursorRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("name", Type.STRINGREF),
        Field("texture", Type.STRINGREF)
    }
}

M.IconRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("texture", Type.STRINGREF)
    }
}

M.ItemDisplayInfoRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("flags", Type.U16),
        Field("itemRangedDisplayInfoID", Type.U16),

        Field("modelResourcesID", Type.ARRAY, { type = Type.U32, count = 2 }),
        Field("modelMaterialResourcesID", Type.ARRAY, { type = Type.U32, count = 2 }),
        Field("modelGeosetGroups", Type.ARRAY, { type = Type.U8, count = 4 }),
        Field("modelGeosetVisIDs", Type.ARRAY, { type = Type.U16, count = 2 })
    }
}

M.ItemDisplayInfoMaterialResourceRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("displayInfoID", Type.U32),
        Field("componentSection", Type.U8),
        Field("materialResourcesID", Type.U32)
    }
}

M.ItemDisplayInfoModelMaterialResourceRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("displayInfoID", Type.U32),
        Field("modelIndex", Type.U8),
        Field("textureType", Type.U8),
        Field("materialResourcesID", Type.U32)
    }
}

M.ItemRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("name", Type.STRINGREF),
        Field("description", Type.STRINGREF),

        Field("iconID", Type.U32),
        Field("displayID", Type.U32),

        Field("bind", Type.U8),
        Field("rarity", Type.U8),

        Field("category", Type.U8),
        Field("categoryType", Type.U8),

        Field("virtualLevel", Type.U16),
        Field("requiredLevel", Type.U16),

        Field("durability", Type.U32),
        Field("armor", Type.U32),

        Field("statTemplateID", Type.U32),
        Field("armorTemplateID", Type.U32),
        Field("weaponTemplateID", Type.U32),
        Field("shieldTemplateID", Type.U32)
    }
}

M.ItemStatTypeRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("name", Type.STRINGREF),
        Field("description", Type.STRINGREF)
    }
}

M.ItemStatTemplateRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("statTypeID", Type.ARRAY, { type = Type.U8, count = 8 }),
        Field("value", Type.ARRAY, { type = Type.I32, count = 8 })
    }
}

M.ItemArmorTemplateRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("equipType", Type.U8),
        Field("bonusArmor", Type.U32)
    }
}

M.ItemShieldTemplateRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("bonusArmor", Type.U32),
        Field("block", Type.U32)
    }
}

M.ItemWeaponTemplateRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("weaponStyle", Type.U8),
        Field("damageRange", Type.UVEC2),
        Field("speed", Type.F32)
    }
}

M.ItemEffectRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("itemID", Type.U32),
        Field("effectSlot", Type.U8),
        Field("effectType", Type.U8),
        Field("effectSpellID", Type.U32)
    }
}

M.LightRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("mapID", Type.U16),
        Field("position", Type.VEC3),
        Field("fallOff", Type.VEC2),
        Field("paramIDs", Type.ARRAY, { type = Type.U16, count = 8 })
    }
}

M.LightDataRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("lightParamID", Type.U16),
        Field("timestamp", Type.U32),

        Field("diffuseColor", Type.U32),
        Field("ambientColor", Type.U32),
        Field("skyColors", Type.ARRAY, { type = Type.U32, count = 6 }),
        
        Field("sunColor", Type.U32),
        Field("sunFogColor", Type.U32),
        Field("sunFogStrength", Type.F32),
        Field("sunFogAngle", Type.F32),
        
        Field("cloudColors", Type.ARRAY, { type = Type.U32, count = 4 }),
        Field("cloudDensity", Type.F32),

        Field("oceanColors", Type.ARRAY, { type = Type.U32, count = 2 }),
        Field("riverColors", Type.ARRAY, { type = Type.U32, count = 2 }),

        Field("shadowColor", Type.U32),
        Field("fogEnd", Type.F32),
        Field("fogEndColor", Type.U32),
        Field("fogEndHeightColor", Type.U32),
        Field("fogHeightColor", Type.U32),
        Field("fogScaler", Type.F32),
        Field("fogDensity", Type.U8)
    }
}

M.LightParamRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("flags", Type.U8),
        Field("lightSkyboxID", Type.U16),

        Field("glow", Type.F32),
        
        Field("oceanAlphas", Type.ARRAY, { type = Type.F32, count = 2 }),
        Field("riverAlphas", Type.ARRAY, { type = Type.F32, count = 2 })
    }
}

M.LightSkyboxRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("name", Type.STRINGREF),
        Field("model", Type.STRINGREF)
    }
}

M.LiquidTypeRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("name", Type.STRINGREF),

        Field("flags", Type.U16),
        Field("lightID", Type.U16),
        Field("soundBank", Type.U8),
        Field("soundID", Type.U16),
        Field("materialID", Type.U8),
        Field("particleScale", Type.U8),
        Field("particleMovement", Type.U8),
        Field("particleTextureSlot", Type.U8),
        
        Field("minimapColor", Type.U32),
        
        Field("maxDarkenDepth", Type.U32),
        Field("fogDarkenIntensity", Type.F32),
        Field("ambDarkenIntensity", Type.F32),
        Field("dirDarkenIntensity", Type.F32),
        
        Field("textures", Type.ARRAY, { type = Type.STRINGREF, count = 6 }),
        Field("frameCounts", Type.ARRAY, { type = Type.U8, count = 6 }),
        
        Field("unkFloats", Type.ARRAY, { type = Type.F32, count = 16 }),
        Field("unkInts", Type.ARRAY, { type = Type.I32, count = 4 })
    }
}

M.LiquidMaterialRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("flags", Type.U8),
        Field("liquidVertexFormat", Type.U8)
    }
}

M.LiquidObjectRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("liquidTypeID", Type.U16),
        Field("fishable", Type.U8)
    }
}

M.MapRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("nameInternal", Type.STRINGREF),
        Field("name", Type.STRINGREF),

        Field("flags", Type.U32),
        Field("instanceType", Type.U8),
        Field("expansionID", Type.U8),
        Field("maxPlayers", Type.U16)
    }
}

M.SpellRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("name", Type.STRINGREF),
        Field("description", Type.STRINGREF),
        Field("auraDescription", Type.STRINGREF),
        Field("iconID", Type.U32),

        Field("castTime", Type.F32),
        Field("cooldown", Type.F32),
        Field("duration", Type.F32)
    }
}

M.SpellEffectsRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("spellID", Type.U32),
        Field("effectPriority", Type.U8),
        Field("effectType", Type.U8),
        Field("effectValues", Type.ARRAY, { type = Type.I32, count = 3 }),
        Field("effectMiscValues", Type.ARRAY, { type = Type.I32, count = 3 })
    }
}

M.SpellProcDataRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("phaseMask", Type.U32),
        Field("typeMask", Type.U64),
        Field("hitMask", Type.U64),
        Field("flags", Type.U64),
        Field("procsPerMinute", Type.F32),
        Field("chanceToProc", Type.F32),
        Field("internalCooldownMS", Type.U32),
        Field("charges", Type.I32),
    }
}

M.SpellProcLinkRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("spellID", Type.U32),
        Field("effectMask", Type.U64),
        Field("procDataID", Type.U32)
    }
}

M.UnitRaceRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("prefix", Type.STRINGREF),
        Field("nameInternal", Type.STRINGREF),
        Field("name", Type.STRINGREF),

        Field("flags", Type.U8),
        Field("factionID", Type.U32),

        Field("maleDisplayID", Type.U32),
        Field("femaleDisplayID", Type.U32)
    }
}

M.UnitTextureSectionRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("section", Type.U8),
        Field("position", Type.UVEC2),
        Field("size", Type.UVEC2),
    }
}

M.UnitCustomizationOptionRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("name", Type.STRINGREF),
        Field("flags", Type.U32),
    }
}

M.UnitCustomizationMaterialRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("textureSection", Type.U8),
        Field("materialResourcesID", Type.U32),
    }
}

M.UnitCustomizationGeosetRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("geosetType", Type.U8),
        Field("geosetValue", Type.U8),
    }
}

M.UnitRaceCustomizationChoiceRecord =
{
    archetype = Archetype.ClientDB,

    fields = 
    {
        Field("raceID", Type.U8),
        Field("gender", Type.U8),

        Field("customizationOptionID", Type.U32),
        Field("customizationOptionData1", Type.U16),
        Field("customizationOptionData2", Type.U16),
        Field("customizationGeosetID", Type.U32),
        Field("customizationMaterialID1", Type.U32),
        Field("customizationMaterialID2", Type.U32),
        Field("customizationMaterialID3", Type.U32)
    }
}

return M
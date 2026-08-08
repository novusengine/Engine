local Type = require("Type")
local D = require("Definition")

return D.Definitions
{
    D.ClientDB("TextureFileData",
    {
        D.Field("texture", Type.STRINGREF),
        D.Field("materialResourcesID", Type.U32)
    }),

    D.ClientDB("ModelFileData",
    {
        D.Field("model", Type.STRINGREF),
        D.Field("modelResourcesID", Type.U32),
        D.Field("flags", Type.U8)
    }),

    D.ClientDB("AnimationData",
    {
        D.Field("flags", Type.U64),
        D.Field("fallback", Type.U16),
        D.Field("behaviorID", Type.U16),
        D.Field("behaviorTier", Type.U8)
    }),

    D.ClientDB("CameraSave",
    {
        D.Field("name", Type.STRINGREF),
        D.Field("code", Type.STRINGREF)
    }),

    D.ClientDB("CinematicCamera",
    {
        D.Field("soundID", Type.U32),
        D.Field("endPosition", Type.VEC3),
        D.Field("rotation", Type.F32),
        D.Field("model", Type.STRINGREF)
    }),

    D.ClientDB("CinematicSequences",
    {
        D.Field("cameraID", Type.U16)
    }, { recordName = "CinematicSequence" }),

    D.ClientDB("CreatureModelData",
    {
        D.Field("model", Type.STRINGREF),
        D.Field("flags", Type.U32),

        D.Field("boxMin", Type.VEC3),
        D.Field("boxMax", Type.VEC3),

        D.Field("soundID", Type.U16),
        D.Field("sizeClass", Type.U8),
        D.Field("bloodID", Type.U8),

        D.Field("footprintTextureID", Type.U8),
        D.Field("footprintTextureLength", Type.U8),
        D.Field("footprintTextureWidth", Type.U8),
        D.Field("footprintParticleScale", Type.U8),
        D.Field("footstepCameraEffectID", Type.U16),
        D.Field("deathThudCameraEffectID", Type.U16),

        D.Field("collisionBox", Type.VEC2),
        D.Field("mountHeight", Type.F32)
    }),

    D.ClientDB("CreatureDisplayInfo",
    {
        D.Field("modelID", Type.U32),
        D.Field("extendedDisplayInfoID", Type.U32),

        D.Field("soundID", Type.U16),
        D.Field("npcSoundID", Type.U16),

        D.Field("flags", Type.U8),
        D.Field("gender", Type.U8),
        D.Field("sizeClass", Type.I8),
        D.Field("bloodID", Type.U8),

        D.Field("unarmedWeaponType", Type.U8),

        D.Field("creatureModelAlpha", Type.U8),
        D.Field("creatureModelScale", Type.F32),
        D.Field("creaturePetScale", Type.F32),

        D.Field("textureVariations", Type.ARRAY, { type = Type.STRINGREF, count = 4 })
    }),

    D.ClientDB("CreatureDisplayInfoExtra",
    {
        D.Field("flags", Type.U8),

        D.Field("raceID", Type.U8),
        D.Field("gender", Type.U8),
        D.Field("classID", Type.U8),

        D.Field("skinID", Type.U8),
        D.Field("faceID", Type.U8),
        D.Field("hairStyleID", Type.U8),
        D.Field("hairColorID", Type.U8),
        D.Field("facialHairID", Type.U8),

        D.Field("bakedTexture", Type.STRINGREF)
    }),

    D.ClientDB("Cursor",
    {
        D.Field("name", Type.STRINGREF),
        D.Field("texture", Type.STRINGREF)
    }),

    D.ClientDB("Faction",
    {
        D.Field("name", Type.STRINGREF),
        D.Field("flags", Type.U16),
        D.Field("defaultReactionToOthers", Type.U8),
        D.Field("defaultPlayerReactionMin", Type.U8),
        D.Field("defaultPlayerReactionMax", Type.U8),
        D.Field("defaultReputationValue", Type.I32)
    }),

    D.ClientDB("FactionRelation",
    {
        D.Field("sourceFactionID", Type.U16),
        D.Field("targetFactionID", Type.U16),
        D.Field("reaction", Type.U8)
    }),

    D.ClientDB("FactionStanding",
    {
        D.Field("name", Type.STRINGREF),
        D.Field("minimumValue", Type.I32),
        D.Field("reaction", Type.U8),
        D.Field("sortOrder", Type.U16)
    }),

    D.ClientDB("Icon",
    {
        D.Field("texture", Type.STRINGREF)
    }),

    D.ClientDB("ItemDisplayInfo",
    {
        D.Field("flags", Type.U16),
        D.Field("itemRangedDisplayInfoID", Type.U16),

        D.Field("modelResourcesID", Type.ARRAY, { type = Type.U32, count = 2 }),
        D.Field("modelMaterialResourcesID", Type.ARRAY, { type = Type.U32, count = 2 }),
        D.Field("modelGeosetGroups", Type.ARRAY, { type = Type.U8, count = 4 }),
        D.Field("modelGeosetVisIDs", Type.ARRAY, { type = Type.U16, count = 2 })
    }),

    D.ClientDB("ItemDisplayInfoMaterialRes",
    {
        D.Field("displayInfoID", Type.U32),
        D.Field("componentSection", Type.U8),
        D.Field("materialResourcesID", Type.U32)
    }, { recordName = "ItemDisplayInfoMaterialResource" }),

    D.ClientDB("ItemDisplayInfoModelMatRes",
    {
        D.Field("displayInfoID", Type.U32),
        D.Field("modelIndex", Type.U8),
        D.Field("textureType", Type.U8),
        D.Field("materialResourcesID", Type.U32)
    }, { recordName = "ItemDisplayInfoModelMaterialResource" }),

    D.ClientDB("Item",
    {
        D.Field("name", Type.STRINGREF),
        D.Field("description", Type.STRINGREF),

        D.Field("iconID", Type.U32),
        D.Field("displayID", Type.U32),

        D.Field("bind", Type.U8),
        D.Field("rarity", Type.U8),

        D.Field("category", Type.U8),
        D.Field("categoryType", Type.U8),

        D.Field("virtualLevel", Type.U16),
        D.Field("requiredLevel", Type.U16),

        D.Field("durability", Type.U32),
        D.Field("armor", Type.U32),

        D.Field("statTemplateID", Type.U32),
        D.Field("armorTemplateID", Type.U32),
        D.Field("weaponTemplateID", Type.U32),
        D.Field("shieldTemplateID", Type.U32)
    }),

    D.ClientDB("ItemStatTypes",
    {
        D.Field("name", Type.STRINGREF),
        D.Field("description", Type.STRINGREF)
    }, { recordName = "ItemStatType" }),

    D.ClientDB("ItemStatTemplate",
    {
        D.Field("statTypeID", Type.ARRAY, { type = Type.U8, count = 8 }),
        D.Field("value", Type.ARRAY, { type = Type.I32, count = 8 })
    }),

    D.ClientDB("ItemArmorTemplate",
    {
        D.Field("equipType", Type.U8),
        D.Field("bonusArmor", Type.U32)
    }),

    D.ClientDB("ItemShieldTemplate",
    {
        D.Field("bonusArmor", Type.U32),
        D.Field("block", Type.U32)
    }),

    D.ClientDB("ItemWeaponTemplate",
    {
        D.Field("weaponStyle", Type.U8),
        D.Field("damageRange", Type.UVEC2),
        D.Field("speed", Type.F32)
    }),

    D.ClientDB("ItemEffects",
    {
        D.Field("itemID", Type.U32),
        D.Field("effectSlot", Type.U8),
        D.Field("effectType", Type.U8),
        D.Field("effectSpellID", Type.U32)
    }, { recordName = "ItemEffect" }),

    D.ClientDB("Light",
    {
        D.Field("mapID", Type.U16),
        D.Field("position", Type.VEC3),
        D.Field("fallOff", Type.VEC2),
        D.Field("paramIDs", Type.ARRAY, { type = Type.U16, count = 8 })
    }),

    D.ClientDB("LightData",
    {
        D.Field("lightParamID", Type.U16),
        D.Field("timestamp", Type.U32),

        D.Field("diffuseColor", Type.U32),
        D.Field("ambientColor", Type.U32),
        D.Field("skyColors", Type.ARRAY, { type = Type.U32, count = 6 }),
        
        D.Field("sunColor", Type.U32),
        D.Field("sunFogColor", Type.U32),
        D.Field("sunFogStrength", Type.F32),
        D.Field("sunFogAngle", Type.F32),
        
        D.Field("cloudColors", Type.ARRAY, { type = Type.U32, count = 4 }),
        D.Field("cloudDensity", Type.F32),

        D.Field("oceanColors", Type.ARRAY, { type = Type.U32, count = 2 }),
        D.Field("riverColors", Type.ARRAY, { type = Type.U32, count = 2 }),

        D.Field("shadowColor", Type.U32),
        D.Field("fogEnd", Type.F32),
        D.Field("fogEndColor", Type.U32),
        D.Field("fogEndHeightColor", Type.U32),
        D.Field("fogHeightColor", Type.U32),
        D.Field("fogScaler", Type.F32),
        D.Field("fogDensity", Type.U8)
    }),

    D.ClientDB("LightParams",
    {
        D.Field("flags", Type.U8),
        D.Field("lightSkyboxID", Type.U16),

        D.Field("glow", Type.F32),
        
        D.Field("oceanAlphas", Type.ARRAY, { type = Type.F32, count = 2 }),
        D.Field("riverAlphas", Type.ARRAY, { type = Type.F32, count = 2 })
    }, { recordName = "LightParam" }),

    D.ClientDB("LightSkybox",
    {
        D.Field("name", Type.STRINGREF),
        D.Field("model", Type.STRINGREF)
    }),

    D.ClientDB("LiquidType",
    {
        D.Field("name", Type.STRINGREF),

        D.Field("flags", Type.U16),
        D.Field("lightID", Type.U16),
        D.Field("soundBank", Type.U8),
        D.Field("soundID", Type.U16),
        D.Field("materialID", Type.U8),
        D.Field("particleScale", Type.U8),
        D.Field("particleMovement", Type.U8),
        D.Field("particleTextureSlot", Type.U8),
        
        D.Field("minimapColor", Type.U32),
        
        D.Field("maxDarkenDepth", Type.U32),
        D.Field("fogDarkenIntensity", Type.F32),
        D.Field("ambDarkenIntensity", Type.F32),
        D.Field("dirDarkenIntensity", Type.F32),
        
        D.Field("textures", Type.ARRAY, { type = Type.STRINGREF, count = 6 }),
        D.Field("frameCounts", Type.ARRAY, { type = Type.U8, count = 6 }),
        
        D.Field("unkFloats", Type.ARRAY, { type = Type.F32, count = 16 }),
        D.Field("unkInts", Type.ARRAY, { type = Type.I32, count = 4 })
    }),

    D.ClientDB("LiquidMaterial",
    {
        D.Field("flags", Type.U8),
        D.Field("liquidVertexFormat", Type.U8)
    }),

    D.ClientDB("LiquidObject",
    {
        D.Field("liquidTypeID", Type.U16),
        D.Field("fishable", Type.U8)
    }),

    D.ClientDB("Map",
    {
        D.Field("nameInternal", Type.STRINGREF),
        D.Field("name", Type.STRINGREF),

        D.Field("flags", Type.U32),
        D.Field("instanceType", Type.U8),
        D.Field("expansionID", Type.U8),
        D.Field("maxPlayers", Type.U16)
    }),

    D.ClientDB("Spell",
    {
        D.Field("name", Type.STRINGREF),
        D.Field("description", Type.STRINGREF),
        D.Field("auraDescription", Type.STRINGREF),
        D.Field("iconID", Type.U32),

        D.Field("castTime", Type.F32),
        D.Field("cooldown", Type.F32),

        D.Field("targetSelector", Type.U8),
        D.Field("targetShape", Type.U8),
        D.Field("targetRelation", Type.U8),
        D.Field("targetRecipientMask", Type.U8),
        D.Field("rangePolicy", Type.U8),
        D.Field("minimumRange", Type.F32),
        D.Field("maximumRange", Type.F32),
        D.Field("targetRadius", Type.F32),
        D.Field("maximumTargets", Type.U16)
    }),

    D.ClientDB("SpellEffects",
    {
        D.Field("spellID", Type.U32),
        D.Field("effectPriority", Type.U8),
        D.Field("effectType", Type.U8),
        D.Field("parameters", Type.ARRAY, { type = Type.I32, count = 6 })
    }),

    D.ClientDB("SpellAura",
    {
        D.Field("duration", Type.F32),
        D.Field("stacksPerApplication", Type.U16),
        D.Field("maximumStacks", Type.U16),
        D.Field("applicationPolicy", Type.U8),
        D.Field("disposition", Type.U8),
        D.Field("dispelType", Type.U8),
        D.Field("lifecycleFlags", Type.U8)
    }),

    D.ClientDB("SpellAuraConstraintGroup",
    {
        D.Field("name", Type.STRINGREF),
        D.Field("defaultScope", Type.U8),
        D.Field("defaultMaximumApplications", Type.U16),
        D.Field("defaultOverflowBehavior", Type.U8)
    }),

    D.ClientDB("SpellAuraConstraint",
    {
        D.Field("spellID", Type.U32),
        D.Field("groupID", Type.U32),
        D.Field("scope", Type.U8),
        D.Field("maximumApplications", Type.U16),
        D.Field("overflowBehavior", Type.U8),
        D.Field("overrideMask", Type.U8)
    }),

    D.ClientDB("SpellProcData",
    {
        D.Field("ownerSpellID", Type.U32),
        D.Field("name", Type.STRINGREF),
        D.Field("phaseMask", Type.U32),
        D.Field("typeMask", Type.U64),
        D.Field("hitMask", Type.U64),
        D.Field("flags", Type.U64),
        D.Field("procsPerMinute", Type.F32),
        D.Field("chanceToProc", Type.F32),
        D.Field("internalCooldownMS", Type.U32),
        D.Field("charges", Type.I32),
    }),

    D.ClientDB("SpellProcLink",
    {
        D.Field("spellID", Type.U32),
        D.Field("effectMask", Type.U64),
        D.Field("procDataID", Type.U32)
    }),

    D.ClientDB("UnitRace",
    {
        D.Field("prefix", Type.STRINGREF),
        D.Field("nameInternal", Type.STRINGREF),
        D.Field("name", Type.STRINGREF),

        D.Field("flags", Type.U8),
        D.Field("factionID", Type.U16),

        D.Field("maleDisplayID", Type.U32),
        D.Field("femaleDisplayID", Type.U32)
    }),

    D.ClientDB("UnitTextureSection",
    {
        D.Field("section", Type.U8),
        D.Field("position", Type.UVEC2),
        D.Field("size", Type.UVEC2),
    }),

    D.ClientDB("UnitCustomizationOption",
    {
        D.Field("name", Type.STRINGREF),
        D.Field("flags", Type.U32),
    }),

    D.ClientDB("UnitCustomizationMaterial",
    {
        D.Field("textureSection", Type.U8),
        D.Field("materialResourcesID", Type.U32),
    }),

    D.ClientDB("UnitCustomizationGeoset",
    {
        D.Field("geosetType", Type.U8),
        D.Field("geosetValue", Type.U8),
    }),

    D.ClientDB("UnitRaceCustomizationChoice",
    {
        D.Field("raceID", Type.U8),
        D.Field("gender", Type.U8),

        D.Field("customizationOptionID", Type.U32),
        D.Field("customizationOptionData1", Type.U16),
        D.Field("customizationOptionData2", Type.U16),
        D.Field("customizationGeosetID", Type.U32),
        D.Field("customizationMaterialID1", Type.U32),
        D.Field("customizationMaterialID2", Type.U32),
        D.Field("customizationMaterialID3", Type.U32)
    })
}

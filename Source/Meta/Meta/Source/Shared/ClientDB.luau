
type TextureFileData<ClientDB> =
{
    Name : "TextureFileData";
    Fields :
    {
        texture : stringref,
        materialResourcesID : u32
    };
};

type ModelFileData<ClientDB> =
{
    Name : "ModelFileData";
    Fields :
    {
        model : stringref,
        modelResourcesID : u32,
        flags : u8
    };
};

type AnimationData<ClientDB> =
{
    Name : "AnimationData";
    Fields :
    {
        flags : u64,
        fallback : u16,
        behaviorID : u16,
        behaviorTier : u8
    };
};

type CameraSave<ClientDB> =
{
    Name : "CameraSave";
    Fields :
    {
        name : stringref,
        code : stringref
    };
};

type CinematicCamera<ClientDB> =
{
    Name : "CinematicCamera";
    Fields :
    {
        soundID : u32,

        endPosition : vec3,
        rotation : f32,

        model : stringref
    };
};

type CinematicSequence<ClientDB> =
{
    Name : "CinematicSequence";
    Fields :
    {
        cameraID : u16,
    };
};

type CreatureModelData<ClientDB> =
{
    Name : "CreatureModelData";
    Fields :
    {
        model : stringref,
        flags : u32,

        boxMin : vec3,
        boxMax : vec3,

        soundID : u16,
        sizeClass : u8,
        bloodID : u8,

        footprintTextureID : u8,
        footprintTextureLength : u8,
        footprintTextureWidth : u8,
        footprintParticleScale : u8,
        footstepCameraEffectID : u16,
        deathThudCameraEffectID : u16,

        collisionBox : vec2,
        mountHeight : f32
    };
};

type CreatureDisplayInfo<ClientDB> =
{
    Name : "CreatureDisplayInfo";
    Fields :
    {
        modelID : u32,
        extendedDisplayInfoID : u32,

        soundID : u16,
        npcSoundID : u16,

        flags : u8,
        gender : u8,
        sizeClass : i8,
        bloodID : u8,

        unarmedWeaponType : u8,

        creatureModelAlpha : u8,
        creatureModelScale : f32,
        creaturePetScale : f32,

        textureVariations : array<stringref, 4>,
    };
};

type CreatureDisplayInfoExtra<ClientDB> =
{
    Name : "CreatureDisplayInfoExtra";
    Fields :
    {
        flags : u8,

        raceID : u8,
        gender : u8,
        classID : u8,

        skinID : u8,
        faceID : u8,
        hairStyleID : u8,
        hairColorID : u8,
        facialHairID : u8,

        bakedTexture : stringref
    };
};

type Cursor<ClientDB> =
{
    Name : "Cursor";
    Fields :
    {
        name : stringref,
        texture : stringref
    };
};

type Icon<ClientDB> =
{
    Name : "Icon";
    Fields :
    {
        texture : stringref
    };
};

type ItemDisplayInfo<ClientDB> =
{
    Name : "ItemDisplayInfo";
    Fields :
    {
        flags : u16,
        itemRangedDisplayInfoID : u16,

        modelResourcesID : array<u32, 2>,
        modelMaterialResourcesID : array<u32, 2>,
        modelGeosetGroups : array<u8, 4>,
        modelGeosetVisIDs : array<u16, 2>
    };
}

type ItemDisplayInfoMaterialResource<ClientDB> =
{
    Name : "ItemDisplayInfoMaterialResource";
    Fields :
    {
        displayInfoID : u32,
        componentSection : u8,
        materialResourcesID : u32
    };
};

type ItemDisplayInfoModelMaterialResource<ClientDB> =
{
    Name : "ItemDisplayInfoModelMaterialResource";
    Fields :
    {
        displayInfoID : u32,
        modelIndex : u8,
        textureType : u8,
        materialResourcesID : u32
    };
};

type Item<ClientDB> =
{
    Name : "Item";
    Fields :
    {
        name : stringref,
        description : stringref,
        
        iconID : u32,
        displayID : u32,
        
        bind : u8,
        rarity : u8,
        
        category : u8,
        categoryType : u8,
        
        virtualLevel : u16,
        requiredLevel : u16,
        
        durability : u32,
        armor : u32,
        
        statTemplateID : u32,
        armorTemplateID : u32,
        weaponTemplateID : u32,
        shieldTemplateID : u32
    };
};

type ItemStatType<ClientDB> =
{
    Name : "ItemStatType";
    Fields :
    {
        name : stringref,
        description : stringref
    };
};

type ItemStatTemplate<ClientDB> =
{
    Name : "ItemStatTemplate";
    Fields :
    {
        statTypeID : array<u8, 8>,
        value : array<i32, 8>
    };
};

type ItemArmorTemplate<ClientDB> =
{
    Name : "ItemArmorTemplate";
    Fields :
    {
        equipType : u8,
        bonusArmor : u32
    };
};

type ItemShieldTemplate<ClientDB> =
{
    Name : "ItemShieldTemplate";
    Fields :
    {
        bonusArmor : u32,
        block : u32
    };
};

type ItemWeaponTemplate<ClientDB> =
{
    Name : "ItemWeaponTemplate";
    Fields :
    {
        weaponStyle : u8,
        damageRange : uvec2,
        speed : f32
    };
};

type ItemEffect<ClientDB> =
{
    Name : "ItemEffect";
    Fields :
    {
        itemID : u32,
        effectSlot : u8,
        effectType : u8,
        effectSpellID : u32
    };
};

type Light<ClientDB> =
{
    Name : "Light";
    Fields :
    {
        mapID : u16,
        position : vec3,
        fallOff : vec2,
        paramIDs : array<u16, 8>
    };
};

type LightData<ClientDB> =
{
    Name : "LightData";
    Fields :
    {
        lightParamID : u16,
        timestamp : u32,
        
        diffuseColor : u32,
        ambientColor : u32,

        skyTopColor : u32,
        skyMiddleColor : u32,
        skyBand1Color : u32,
        skyBand2Color : u32,
        skySmogColor : u32,
        skyFogColor : u32,

        sunColor : u32,
        sunFogColor : u32,
        sunFogStrength : f32,
        sunFogAngle : f32,

        cloudSunColor : u32,
        cloudEmissiveColor : u32,
        cloudLayer1AmbientColor : u32,
        cloudLayer2AmbientColor : u32,
        cloudDensity : f32,

        oceanShallowColor : u32,
        oceanDeepColor : u32,
        riverShallowColor : u32,
        riverDeepColor : u32,

        shadowColor : u32,
        fogEnd : f32,
        fogEndColor : u32,
        fogEndHeightColor : u32,
        fogHeightColor : u32,
        fogScaler : f32,
        fogDensity : u8
    };
}

type LightParam<ClientDB> =
{
    Name : "LightParam";
    Fields :
    {
        flags : u8,
        lightSkyboxID : u16,

        glow : f32,

        oceanShallowAlpha : f32,
        oceanDeepAlpha : f32,
        riverShallowAlpha : f32,
        riverDeepAlpha : f32
    };
};

type LightSkybox<ClientDB> =
{
    Name : "LightSkybox";
    Fields :
    {
        name : stringref,
        model : stringref
    };
};

type LiquidType<ClientDB> =
{
    Name : "LiquidType";
    Fields :
    {
        name : stringref,

        flags : u16,
        lightID : u16,
        soundBank : u8,
        soundID : u16,
        materialID : u8,
        particleScale : u8,
        particleMovement : u8,
        particleTextureSlot : u8,

        minimapColor : u32,

        maxDarkenDepth : u32,
        fogDarkenIntensity : f32,
        ambDarkenIntensity : f32,
        dirDarkenIntensity : f32,

        textures : array<stringref, 6>,
        frameCounts : array<u8, 6>,

        unkFloats : array<f32, 16>,
        unkInts : array<i32, 4>
    };
};

type LiquidMaterial<ClientDB> =
{
    Name : "LiquidMaterial";
    Fields :
    {
        flags : u8,
        liquidVertexFormat : u8
    };
};

type LiquidObject<ClientDB> =
{
    Name : "LiquidObject";
    Fields :
    {
        liquidTypeID : u16,
        fishable : u8,
    };
};

type Map<ClientDB> =
{
    Name : "Map";
    Fields :
    {
        nameInternal : stringref,
        name : stringref,

        flags : u32,
        instanceType : u8,
        expansionID : u8,
        maxPlayers : u16,
    };
};

type Spell<ClientDB> =
{
    Name : "Spell";
    Fields :
    {
        name : stringref,
        description : stringref,
        auraDescription : stringref,
        iconID : u32,
        castTime : f32,
        cooldown : f32,
        duration : f32
    };
};

type SpellEffects<ClientDB> =
{
    Name : "SpellEffects";
    Fields :
    {
        spellID : u32,
        effectPriority : u8,
        effectType : u8,
        effectValue1 : i32,
        effectValue2 : i32,
        effectValue3 : i32,
        effectMiscValue1 : i32,
        effectMiscValue2 : i32,
        effectMiscValue3 : i32
    };
};

type SpellProcData<ClientDB> =
{
    Name : "SpellProcData";
    Fields :
    {
        phaseMask : u32,
        typeMask : u64,
        hitMask : u64,
        flags : u64,
        procsPerMinute : f32,
        chanceToProc : f32,
        internalCooldownMS : u32,
        charges : i32
    };
};

type SpellProcLink<ClientDB> =
{
    Name : "SpellProcLink";
    Fields :
    {
        spellID : u32,
        effectMask : u64,
        procDataID : u32
    };
};

type UnitRace<ClientDB> =
{
    Name : "UnitRace";
    Fields :
    {
        prefix : stringref,
        nameInternal : stringref,
        name : stringref,

        flags : u8,
        factionID : u32,

        maleDisplayID : u32,
        femaleDisplayID : u32
    };
};

type UnitTextureSection<ClientDB> =
{
    Name : "UnitTextureSection";
    Fields :
    {
        section : u8,

        position : uvec2,
        size : uvec2
    };
};

type UnitCustomizationOption<ClientDB> =
{
    Name : "UnitCustomizationOption";
    Fields :
    {
        name : stringref,
        flags : u32
    };
};

type UnitCustomizationMaterial<ClientDB> =
{
    Name : "UnitCustomizationMaterial";
    Fields :
    {
        textureSection : u8,
        materialResourcesID : u32
    };
};

type UnitCustomizationGeoset<ClientDB> =
{
    Name : "UnitCustomizationGeoset";
    Fields :
    {
        geosetType : u8,
        geosetValue : u8
    };
};

type UnitRaceCustomizationChoice<ClientDB> =
{
    Name : "UnitRaceCustomizationChoice";
    Fields :
    {
        raceID : u8,
        gender : u8,
        customizationOptionID : u32,
        customizationOptionData1 : u16,
        customizationOptionData2 : u16,
        customizationGeosetID : u32,
        customizationMaterialID1 : u32,
        customizationMaterialID2 : u32,
        customizationMaterialID3 : u32
    };
};
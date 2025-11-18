type Help<Command> =
{
    Names : "help";
    Parameters : { };
};

type Ping<Command> =
{
    Names : "ping";
    Parameters : { };
};

type Lua<Command> =
{
    Names : { "lua", "eval" };
    Parameters : { code : string };
};

type ScriptReload<Command> =
{
    Names : "script reload";
    Parameters : { };
};

type DatabaseReload<Command> =
{
    Names : "database reload";
    Parameters : { };
};

type CameraSave<Command> =
{
    Names : "camera save";
    Parameters : { name : string };
};

type CameraLoadByCode<Command> =
{
    Names : "camera loadbycode";
    Parameters : { code : string };
};

type MapClear<Command> =
{
    Names : "map clear";
    Parameters : { };
};

type UnitMorph<Command> =
{
    Names : "unit morph";
    Parameters : { displayID : u32 };
};

type UnitDemorph<Command> =
{
    Names : "unit demorph";
    Parameters : { };
};

type CharacterAdd<Command> =
{
    Names : "character add";
    Parameters : { name : string };
};

type CharacterRemove<Command> =
{
    Names : { "character remove", "character rem" };
    Parameters : { name : string };
};

type CheatFly<Command> =
{
    Names : "cheat fly";
    Parameters : { enable : boolean };
};

type UnitSetRace<Command> =
{
    Names : "unit setrace";
    Parameters : { race : string };
};

type UnitSetGender<Command> =
{
    Names : "unit setgender";
    Parameters : { gender : string };
};

type ItemSync<Command> =
{
    Names : "item sync";
    Parameters : { itemID : u32 };
};

type ItemSyncAll<Command> =
{
    Names : "item sync all";
    Parameters : { };
};

type ItemAdd<Command> =
{
    Names : "item add";
    Parameters : { itemID : u32 };
};

type ItemRemove<Command> =
{
    Names : { "item remove", "item rem" };
    Parameters : { itemID : u32 };
};

type CreatureAdd<Command> =
{
    Names : "creature add";
    Parameters : { creatureTemplateID : u32 };
}

type CreatureRemove<Command> =
{
    Names : { "creature remove", "creature rem" };
    Parameters : { };
}

type CreatureInfo<Command> =
{
    Names : "creature info";
    Parameters : { };
}

type CheatLogin<Command> =
{
    Names : "cheat login";
    Parameters : { accountName : string};
}

type CheatDamage<Command> =
{
    Names : "cheat damage";
    Parameters : { amount : u32 };
};

type CheatKill<Command> =
{
    Names : "cheat kill";
    Parameters : { };
};

type CheatResurrect <Command> =
{
    Names : "cheat resurrect";
    Parameters : { };
};

type CheatCast<Command> =
{
    Names : "cheat cast";
    Parameters : { spellID : u32 };
};

type MapSync<Command> =
{
    Names : "map sync";
    Parameters : { mapID : u32 };
};

type MapSyncAll<Command> =
{
    Names : "map sync all";
    Parameters : { };
};

type GotoAdd<Command> =
{
    Names : "goto add";
    Parameters : { location : string, mapID : u32, x : f32, y : f32, z : f32, orientation : f32 };
};

type GotoAddHere<Command> =
{
    Names : "goto add here";
    Parameters : { location : string };
};

type GotoRemove<Command> =
{
    Names : { "goto remove", "goto rem" };
    Parameters : { location : string };
};

type GotoMap<Command> =
{
    Names : "goto map";
    Parameters : { mapID : u32 };
};

type GotoLocation<Command> =
{
    Names : {"goto location", "goto loc"};
    Parameters : { location : string };
};

type GotoXYZ<Command> =
{
    Names : "goto xyz";
    Parameters : { x : f32, y : f32, z : f32 };
};

type TriggerAdd<Command> =
{
    Names : "trigger add";
    Parameters : { name : string, flags : u16, mapID : u16, positionX : f32, positionY : f32, positionZ : f32, extentsX : f32, extentsY : f32, extentsZ : f32 };
};

type TriggerRemove<Command> =
{
    Names : {"trigger remove", "trigger rem"};
    Parameters : { triggerID : u32 };
};

type SpellSync<Command> =
{
    Names : "spell sync";
    Parameters : { spellID : u32 };
};

type SpellSyncAll<Command> =
{
    Names : "spell sync all";
    Parameters : { };
};

type CreatureAddScript<Command> =
{
    Names : "creature add script";
    Parameters : { scriptName : string };
};

type CreatureRemoveScript<Command> =
{
    Names : "creature remove script";
    Parameters : { };
};
type CheatPathGenerate<Command> =
{
    Names : "cheat path generate";
    Parameters : { };
};
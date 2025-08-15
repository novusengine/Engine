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

type ReloadScripts<Command> =
{
    Names : { "reloadscripts", "reload" };
    Parameters : { };
};

type RefreshDB<Command> =
{
    Names : { "refreshdb", "refresh" };
    Parameters : { };
};

type SaveCamera<Command> =
{
    Names : "savecamera";
    Parameters : { name : string };
};

type LoadCameraByCode<Command> =
{
    Names : "loadcamerabycode";
    Parameters : { code : string };
};

type ClearMap<Command> =
{
    Names : "clearmap";
    Parameters : { };
};

type Morph<Command> =
{
    Names : "morph";
    Parameters : { displayID : u32 };
};

type Demorph<Command> =
{
    Names : "demorph";
    Parameters : { };
};

type CharacterCreate<Command> =
{
    Names : "charcreate";
    Parameters : { name : string };
};

type CharacterDelete<Command> =
{
    Names : "chardelete";
    Parameters : { name : string };
};

type Fly<Command> =
{
    Names : "fly";
    Parameters : { enable : boolean };
};

type SetRace<Command> =
{
    Names : "setrace";
    Parameters : { race : string };
};

type SetGender<Command> =
{
    Names : "setgender";
    Parameters : { gender : string };
};

type SyncItem<Command> =
{
    Names : "syncitem";
    Parameters : { itemID : u32 };
};

type ForceSyncItems<Command> =
{
    Names : "forcesyncitems";
    Parameters : { };
};

type AddItem<Command> =
{
    Names : "additem";
    Parameters : { itemID : u32 };
};

type RemoveItem<Command> =
{
    Names : "removeitem";
    Parameters : { itemID : u32 };
};
#pragma once

#include <array>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include <Base/Types.h>

namespace MetaGen::Game::Command
{
    struct HelpCommand
    {
    public:
        static constexpr u32 NUM_PARAMETERS = 0;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "()";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "help" };
        static constexpr std::array<std::string_view, 0> PARAMETER_NAME_LIST = {  };
        using PARAMETER_TYPE_LIST = std::tuple<>;

        static bool Read(std::vector<std::string>& parameters, HelpCommand& result)
        {
            result = {};
            return true;
        }
    };

    struct PingCommand
    {
    public:
        static constexpr u32 NUM_PARAMETERS = 0;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "()";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "ping" };
        static constexpr std::array<std::string_view, 0> PARAMETER_NAME_LIST = {  };
        using PARAMETER_TYPE_LIST = std::tuple<>;

        static bool Read(std::vector<std::string>& parameters, PingCommand& result)
        {
            result = {};
            return true;
        }
    };

    struct LuaCommand
    {
    public:
        std::string code;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(code : std::string)";
        static constexpr std::array<std::string_view, 2> COMMAND_NAME_LIST = { "lua", "eval" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "code" };
        using PARAMETER_TYPE_LIST = std::tuple<std::string>;

        static bool Read(std::vector<std::string>& parameters, LuaCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                LuaCommand tmp = {};

                tmp.code = std::move(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct ScriptReloadCommand
    {
    public:
        static constexpr u32 NUM_PARAMETERS = 0;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "()";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "script reload" };
        static constexpr std::array<std::string_view, 0> PARAMETER_NAME_LIST = {  };
        using PARAMETER_TYPE_LIST = std::tuple<>;

        static bool Read(std::vector<std::string>& parameters, ScriptReloadCommand& result)
        {
            result = {};
            return true;
        }
    };

    struct DatabaseReloadCommand
    {
    public:
        static constexpr u32 NUM_PARAMETERS = 0;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "()";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "database reload" };
        static constexpr std::array<std::string_view, 0> PARAMETER_NAME_LIST = {  };
        using PARAMETER_TYPE_LIST = std::tuple<>;

        static bool Read(std::vector<std::string>& parameters, DatabaseReloadCommand& result)
        {
            result = {};
            return true;
        }
    };

    struct CameraSaveCommand
    {
    public:
        std::string name;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(name : std::string)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "camera save" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "name" };
        using PARAMETER_TYPE_LIST = std::tuple<std::string>;

        static bool Read(std::vector<std::string>& parameters, CameraSaveCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                CameraSaveCommand tmp = {};

                tmp.name = std::move(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct CameraLoadByCodeCommand
    {
    public:
        std::string code;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(code : std::string)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "camera loadbycode" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "code" };
        using PARAMETER_TYPE_LIST = std::tuple<std::string>;

        static bool Read(std::vector<std::string>& parameters, CameraLoadByCodeCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                CameraLoadByCodeCommand tmp = {};

                tmp.code = std::move(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct MapClearCommand
    {
    public:
        static constexpr u32 NUM_PARAMETERS = 0;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "()";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "map clear" };
        static constexpr std::array<std::string_view, 0> PARAMETER_NAME_LIST = {  };
        using PARAMETER_TYPE_LIST = std::tuple<>;

        static bool Read(std::vector<std::string>& parameters, MapClearCommand& result)
        {
            result = {};
            return true;
        }
    };

    struct MapSyncCommand
    {
    public:
        u32 mapID;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(mapID : u32)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "map sync" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "mapID" };
        using PARAMETER_TYPE_LIST = std::tuple<u32>;

        static bool Read(std::vector<std::string>& parameters, MapSyncCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                MapSyncCommand tmp = {};

                tmp.mapID = std::stoul(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct MapSyncAllCommand
    {
    public:
        static constexpr u32 NUM_PARAMETERS = 0;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "()";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "map sync all" };
        static constexpr std::array<std::string_view, 0> PARAMETER_NAME_LIST = {  };
        using PARAMETER_TYPE_LIST = std::tuple<>;

        static bool Read(std::vector<std::string>& parameters, MapSyncAllCommand& result)
        {
            result = {};
            return true;
        }
    };

    struct CharacterAddCommand
    {
    public:
        std::string name;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(name : std::string)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "character add" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "name" };
        using PARAMETER_TYPE_LIST = std::tuple<std::string>;

        static bool Read(std::vector<std::string>& parameters, CharacterAddCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                CharacterAddCommand tmp = {};

                tmp.name = std::move(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct CharacterRemoveCommand
    {
    public:
        std::string name;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(name : std::string)";
        static constexpr std::array<std::string_view, 2> COMMAND_NAME_LIST = { "character remove", "character rem" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "name" };
        using PARAMETER_TYPE_LIST = std::tuple<std::string>;

        static bool Read(std::vector<std::string>& parameters, CharacterRemoveCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                CharacterRemoveCommand tmp = {};

                tmp.name = std::move(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct CheatLoginCommand
    {
    public:
        std::string accountName;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(accountName : std::string)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "cheat login" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "accountName" };
        using PARAMETER_TYPE_LIST = std::tuple<std::string>;

        static bool Read(std::vector<std::string>& parameters, CheatLoginCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                CheatLoginCommand tmp = {};

                tmp.accountName = std::move(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct CheatFlyCommand
    {
    public:
        bool enable;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(enable : bool)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "cheat fly" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "enable" };
        using PARAMETER_TYPE_LIST = std::tuple<bool>;

        static bool Read(std::vector<std::string>& parameters, CheatFlyCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                CheatFlyCommand tmp = {};

                tmp.enable = (parameters[0].size() > 0 && (parameters[0][0] == '1' || parameters[0] == "true"));

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct CheatDamageCommand
    {
    public:
        u32 amount;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(amount : u32)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "cheat damage" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "amount" };
        using PARAMETER_TYPE_LIST = std::tuple<u32>;

        static bool Read(std::vector<std::string>& parameters, CheatDamageCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                CheatDamageCommand tmp = {};

                tmp.amount = std::stoul(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct CheatKillCommand
    {
    public:
        static constexpr u32 NUM_PARAMETERS = 0;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "()";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "cheat kill" };
        static constexpr std::array<std::string_view, 0> PARAMETER_NAME_LIST = {  };
        using PARAMETER_TYPE_LIST = std::tuple<>;

        static bool Read(std::vector<std::string>& parameters, CheatKillCommand& result)
        {
            result = {};
            return true;
        }
    };

    struct CheatResurrectCommand
    {
    public:
        static constexpr u32 NUM_PARAMETERS = 0;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "()";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "cheat resurrect" };
        static constexpr std::array<std::string_view, 0> PARAMETER_NAME_LIST = {  };
        using PARAMETER_TYPE_LIST = std::tuple<>;

        static bool Read(std::vector<std::string>& parameters, CheatResurrectCommand& result)
        {
            result = {};
            return true;
        }
    };

    struct CheatCastCommand
    {
    public:
        u32 spellID;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(spellID : u32)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "cheat cast" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "spellID" };
        using PARAMETER_TYPE_LIST = std::tuple<u32>;

        static bool Read(std::vector<std::string>& parameters, CheatCastCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                CheatCastCommand tmp = {};

                tmp.spellID = std::stoul(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct CheatMorphCommand
    {
    public:
        u32 displayID;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(displayID : u32)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "cheat morph" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "displayID" };
        using PARAMETER_TYPE_LIST = std::tuple<u32>;

        static bool Read(std::vector<std::string>& parameters, CheatMorphCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                CheatMorphCommand tmp = {};

                tmp.displayID = std::stoul(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct CheatDemorphCommand
    {
    public:
        static constexpr u32 NUM_PARAMETERS = 0;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "()";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "cheat demorph" };
        static constexpr std::array<std::string_view, 0> PARAMETER_NAME_LIST = {  };
        using PARAMETER_TYPE_LIST = std::tuple<>;

        static bool Read(std::vector<std::string>& parameters, CheatDemorphCommand& result)
        {
            result = {};
            return true;
        }
    };

    struct CheatSetRaceCommand
    {
    public:
        std::string race;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(race : std::string)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "cheat setrace" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "race" };
        using PARAMETER_TYPE_LIST = std::tuple<std::string>;

        static bool Read(std::vector<std::string>& parameters, CheatSetRaceCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                CheatSetRaceCommand tmp = {};

                tmp.race = std::move(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct CheatSetGenderCommand
    {
    public:
        std::string gender;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(gender : std::string)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "cheat setgender" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "gender" };
        using PARAMETER_TYPE_LIST = std::tuple<std::string>;

        static bool Read(std::vector<std::string>& parameters, CheatSetGenderCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                CheatSetGenderCommand tmp = {};

                tmp.gender = std::move(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct CheatPathGenerateCommand
    {
    public:
        static constexpr u32 NUM_PARAMETERS = 0;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "()";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "cheat path generate" };
        static constexpr std::array<std::string_view, 0> PARAMETER_NAME_LIST = {  };
        using PARAMETER_TYPE_LIST = std::tuple<>;

        static bool Read(std::vector<std::string>& parameters, CheatPathGenerateCommand& result)
        {
            result = {};
            return true;
        }
    };

    struct ItemSyncCommand
    {
    public:
        u32 itemID;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(itemID : u32)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "item sync" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "itemID" };
        using PARAMETER_TYPE_LIST = std::tuple<u32>;

        static bool Read(std::vector<std::string>& parameters, ItemSyncCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                ItemSyncCommand tmp = {};

                tmp.itemID = std::stoul(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct ItemSyncAllCommand
    {
    public:
        static constexpr u32 NUM_PARAMETERS = 0;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "()";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "item sync all" };
        static constexpr std::array<std::string_view, 0> PARAMETER_NAME_LIST = {  };
        using PARAMETER_TYPE_LIST = std::tuple<>;

        static bool Read(std::vector<std::string>& parameters, ItemSyncAllCommand& result)
        {
            result = {};
            return true;
        }
    };

    struct ItemAddCommand
    {
    public:
        u32 itemID;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(itemID : u32)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "item add" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "itemID" };
        using PARAMETER_TYPE_LIST = std::tuple<u32>;

        static bool Read(std::vector<std::string>& parameters, ItemAddCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                ItemAddCommand tmp = {};

                tmp.itemID = std::stoul(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct ItemRemoveCommand
    {
    public:
        u32 itemID;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(itemID : u32)";
        static constexpr std::array<std::string_view, 2> COMMAND_NAME_LIST = { "item remove", "item rem" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "itemID" };
        using PARAMETER_TYPE_LIST = std::tuple<u32>;

        static bool Read(std::vector<std::string>& parameters, ItemRemoveCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                ItemRemoveCommand tmp = {};

                tmp.itemID = std::stoul(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct CreatureAddCommand
    {
    public:
        u32 creatureTemplateID;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(creatureTemplateID : u32)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "creature add" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "creatureTemplateID" };
        using PARAMETER_TYPE_LIST = std::tuple<u32>;

        static bool Read(std::vector<std::string>& parameters, CreatureAddCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                CreatureAddCommand tmp = {};

                tmp.creatureTemplateID = std::stoul(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct CreatureRemoveCommand
    {
    public:
        static constexpr u32 NUM_PARAMETERS = 0;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "()";
        static constexpr std::array<std::string_view, 2> COMMAND_NAME_LIST = { "creature remove", "creature rem" };
        static constexpr std::array<std::string_view, 0> PARAMETER_NAME_LIST = {  };
        using PARAMETER_TYPE_LIST = std::tuple<>;

        static bool Read(std::vector<std::string>& parameters, CreatureRemoveCommand& result)
        {
            result = {};
            return true;
        }
    };

    struct CreatureInfoCommand
    {
    public:
        static constexpr u32 NUM_PARAMETERS = 0;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "()";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "creature info" };
        static constexpr std::array<std::string_view, 0> PARAMETER_NAME_LIST = {  };
        using PARAMETER_TYPE_LIST = std::tuple<>;

        static bool Read(std::vector<std::string>& parameters, CreatureInfoCommand& result)
        {
            result = {};
            return true;
        }
    };

    struct CreatureAddScriptCommand
    {
    public:
        std::string scriptName;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(scriptName : std::string)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "creature add script" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "scriptName" };
        using PARAMETER_TYPE_LIST = std::tuple<std::string>;

        static bool Read(std::vector<std::string>& parameters, CreatureAddScriptCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                CreatureAddScriptCommand tmp = {};

                tmp.scriptName = std::move(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct CreatureRemoveScriptCommand
    {
    public:
        static constexpr u32 NUM_PARAMETERS = 0;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "()";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "creature remove script" };
        static constexpr std::array<std::string_view, 0> PARAMETER_NAME_LIST = {  };
        using PARAMETER_TYPE_LIST = std::tuple<>;

        static bool Read(std::vector<std::string>& parameters, CreatureRemoveScriptCommand& result)
        {
            result = {};
            return true;
        }
    };

    struct GotoAddCommand
    {
    public:
        std::string location;
        u32 mapID;
        f32 x;
        f32 y;
        f32 z;
        f32 orientation;

    public:
        static constexpr u32 NUM_PARAMETERS = 6;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(location : std::string, mapID : u32, x : f32, y : f32, z : f32, orientation : f32)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "goto add" };
        static constexpr std::array<std::string_view, 6> PARAMETER_NAME_LIST = { "location", "mapID", "x", "y", "z", "orientation" };
        using PARAMETER_TYPE_LIST = std::tuple<std::string, u32, f32, f32, f32, f32>;

        static bool Read(std::vector<std::string>& parameters, GotoAddCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                GotoAddCommand tmp = {};

                tmp.location = std::move(parameters[0]);
                tmp.mapID = std::stoul(parameters[1]);
                tmp.x = std::stof(parameters[2]);
                tmp.y = std::stof(parameters[3]);
                tmp.z = std::stof(parameters[4]);
                tmp.orientation = std::stof(parameters[5]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct GotoAddHereCommand
    {
    public:
        std::string location;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(location : std::string)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "goto add here" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "location" };
        using PARAMETER_TYPE_LIST = std::tuple<std::string>;

        static bool Read(std::vector<std::string>& parameters, GotoAddHereCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                GotoAddHereCommand tmp = {};

                tmp.location = std::move(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct GotoRemoveCommand
    {
    public:
        std::string location;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(location : std::string)";
        static constexpr std::array<std::string_view, 2> COMMAND_NAME_LIST = { "goto remove", "goto rem" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "location" };
        using PARAMETER_TYPE_LIST = std::tuple<std::string>;

        static bool Read(std::vector<std::string>& parameters, GotoRemoveCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                GotoRemoveCommand tmp = {};

                tmp.location = std::move(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct GotoMapCommand
    {
    public:
        u32 mapID;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(mapID : u32)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "goto map" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "mapID" };
        using PARAMETER_TYPE_LIST = std::tuple<u32>;

        static bool Read(std::vector<std::string>& parameters, GotoMapCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                GotoMapCommand tmp = {};

                tmp.mapID = std::stoul(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct GotoLocationCommand
    {
    public:
        std::string location;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(location : std::string)";
        static constexpr std::array<std::string_view, 2> COMMAND_NAME_LIST = { "goto location", "goto loc" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "location" };
        using PARAMETER_TYPE_LIST = std::tuple<std::string>;

        static bool Read(std::vector<std::string>& parameters, GotoLocationCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                GotoLocationCommand tmp = {};

                tmp.location = std::move(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct GotoXYZCommand
    {
    public:
        f32 x;
        f32 y;
        f32 z;

    public:
        static constexpr u32 NUM_PARAMETERS = 3;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(x : f32, y : f32, z : f32)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "goto xyz" };
        static constexpr std::array<std::string_view, 3> PARAMETER_NAME_LIST = { "x", "y", "z" };
        using PARAMETER_TYPE_LIST = std::tuple<f32, f32, f32>;

        static bool Read(std::vector<std::string>& parameters, GotoXYZCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                GotoXYZCommand tmp = {};

                tmp.x = std::stof(parameters[0]);
                tmp.y = std::stof(parameters[1]);
                tmp.z = std::stof(parameters[2]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct TriggerAddCommand
    {
    public:
        std::string name;
        u16 flags;
        u32 mapID;
        f32 posX;
        f32 posY;
        f32 posZ;
        f32 extX;
        f32 extY;
        f32 extZ;

    public:
        static constexpr u32 NUM_PARAMETERS = 9;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(name : std::string, flags : u16, mapID : u32, posX : f32, posY : f32, posZ : f32, extX : f32, extY : f32, extZ : f32)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "trigger add" };
        static constexpr std::array<std::string_view, 9> PARAMETER_NAME_LIST = { "name", "flags", "mapID", "posX", "posY", "posZ", "extX", "extY", "extZ" };
        using PARAMETER_TYPE_LIST = std::tuple<std::string, u16, u32, f32, f32, f32, f32, f32, f32>;

        static bool Read(std::vector<std::string>& parameters, TriggerAddCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                TriggerAddCommand tmp = {};

                tmp.name = std::move(parameters[0]);
                tmp.flags = static_cast<u16>(std::stoul(parameters[1]));
                tmp.mapID = std::stoul(parameters[2]);
                tmp.posX = std::stof(parameters[3]);
                tmp.posY = std::stof(parameters[4]);
                tmp.posZ = std::stof(parameters[5]);
                tmp.extX = std::stof(parameters[6]);
                tmp.extY = std::stof(parameters[7]);
                tmp.extZ = std::stof(parameters[8]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct TriggerRemoveCommand
    {
    public:
        u32 triggerID;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(triggerID : u32)";
        static constexpr std::array<std::string_view, 2> COMMAND_NAME_LIST = { "trigger remove", "trigger rem" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "triggerID" };
        using PARAMETER_TYPE_LIST = std::tuple<u32>;

        static bool Read(std::vector<std::string>& parameters, TriggerRemoveCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                TriggerRemoveCommand tmp = {};

                tmp.triggerID = std::stoul(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct SpellSyncCommand
    {
    public:
        u32 spellID;

    public:
        static constexpr u32 NUM_PARAMETERS = 1;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "(spellID : u32)";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "spell sync" };
        static constexpr std::array<std::string_view, 1> PARAMETER_NAME_LIST = { "spellID" };
        using PARAMETER_TYPE_LIST = std::tuple<u32>;

        static bool Read(std::vector<std::string>& parameters, SpellSyncCommand& result)
        {
            static constexpr u32 NUM_REQUIRED_PARAMETERS = NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL;

            u32 numParams = static_cast<u32>(parameters.size());
            if (numParams < NUM_REQUIRED_PARAMETERS) return false;
            if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false;

            try
            {
                SpellSyncCommand tmp = {};

                tmp.spellID = std::stoul(parameters[0]);

                result = std::move(tmp);
            }
            catch (const std::exception&) { return false; }

            return true;
        }
    };

    struct SpellSyncAllCommand
    {
    public:
        static constexpr u32 NUM_PARAMETERS = 0;
        static constexpr u32 NUM_PARAMETERS_OPTIONAL = 0;

        static constexpr std::string_view COMMAND_HELP_MESSAGE = "()";
        static constexpr std::array<std::string_view, 1> COMMAND_NAME_LIST = { "spell sync all" };
        static constexpr std::array<std::string_view, 0> PARAMETER_NAME_LIST = {  };
        using PARAMETER_TYPE_LIST = std::tuple<>;

        static bool Read(std::vector<std::string>& parameters, SpellSyncAllCommand& result)
        {
            result = {};
            return true;
        }
    };
}
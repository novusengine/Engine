local M = OrderedTable()

local Type = require("Type")
local Component = require("Component")

M.ClientDB =
{
    kind = "clientDB",
    base = Type.STRUCT,
    cppTypeName = function(definition)
        return (definition.options.recordName or definition.name) .. "Record"
    end,
    components = { Component.ClientDBMetaData, Component.Serialization, Component.Debug }
}

M.Packet =
{
    kind = "packet",
    base = Type.STRUCT,
    components = { Component.PacketMetaData, Component.Serialization, Component.Debug }
}

M.Enum =
{
    kind = "enum",
    base = Type.ENUM,
    components = { Component.EnumMetaData }
}

M.LuaEnum =
{
    kind = "luaEnum",
    base = Type.ENUM,
    components = { Component.EnumMetaData }
}

M.LuaEventData =
{
    kind = "luaEvent",
    base = Type.STRUCT,
    components = { Component.LuaEventMetaData, Component.LuaSerialization }
}

M.NetField =
{
    kind = "netField",
    base = Type.ENUM,
    components = { Component.EnumMetaData }
}

M.GameCommand =
{
    kind = "gameCommand",
    base = Type.STRUCT,
    components = { Component.GameCommand }
}

M.DatabaseTable =
{
    kind = "databaseTable",
    base = Type.STRUCT,
    targets = { postgres = true },
    components = {}
}

return M

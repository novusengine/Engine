local M = OrderedTable()

local Type = require("Type")
local Component = require("Component")

M.ClientDB =
{
    base = Type.STRUCT,
    components = { Component.ClientDBMetaData, Component.Serialization, Component.Debug }
}

M.Packet =
{
    base = Type.STRUCT,
    components = { Component.PacketMetaData, Component.Serialization, Component.Debug }
}

M.Enum =
{
    base = Type.ENUM,
    components = { Component.EnumMetaData }
}

M.LuaEnum =
{
    base = Type.ENUM,
    components = { Component.EnumMetaData }
}

M.LuaEventData =
{
    base = Type.STRUCT,
    components = { Component.LuaEventMetaData, Component.LuaSerialization }
}

M.NetField =
{
    base = Type.ENUM,
    components = { Component.EnumMetaData }
}

M.GameCommand =
{
    base = Type.STRUCT,
    components = { Component.GameCommand }
}

return M
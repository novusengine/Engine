local Archetype = require("Archetype")

local M = {}
local definitionSets = setmetatable({}, { __mode = "k" })
local definitionEntries = setmetatable({}, { __mode = "k" })

local function CopyOptions(options)
    local result = {}
    for key, value in pairs(options or {}) do
        result[key] = value
    end
    return result
end

local function Definition(name, archetype, fields, options)
    local result = CopyOptions(options)
    result.archetype = archetype
    result.fields = fields or {}
    local entry = { name, result }
    definitionEntries[entry] = true
    return entry
end

function M.Field(name, fieldType, attributes)
    return { { name = name, type = fieldType }, attributes }
end

function M.Definitions(definitions)
    local result = OrderedTable()
    for _, definition in ipairs(definitions) do
        if not definitionEntries[definition] then
            error("Definitions must be created with a typed Definition constructor")
        end
        local name = definition[1]
        if result[name] ~= nil then
            error("Duplicate definition '" .. tostring(name) .. "'")
        end

        result[name] = definition[2]
    end
    definitionSets[result] = true
    return result
end

function M.IsDefinitionSet(value)
    return definitionSets[value] == true
end

function M.Enum(name, enumType, values, options)
    local fields = {}
    for _, value in ipairs(values or {}) do
        if type(value) == "string" then
            table.insert(fields, M.Field(value))
        elseif type(value[1]) == "table" then
            table.insert(fields, value)
        else
            table.insert(fields, M.Field(value[1], value[2]))
        end
    end

    local result = CopyOptions(options)
    local archetype = result.archetype or Archetype.Enum
    result.type = enumType
    return Definition(name, archetype, fields, result)
end

function M.LuaEnum(name, enumType, values, options)
    local result = CopyOptions(options)
    result.archetype = Archetype.LuaEnum
    return M.Enum(name, enumType, values, result)
end

function M.NetField(name, enumType, values, options)
    local result = CopyOptions(options)
    result.archetype = Archetype.NetField
    return M.Enum(name, enumType, values, result)
end

function M.Struct(name, archetype, fields, options)
    return Definition(name, archetype, fields, options)
end

function M.Packet(name, fields, options)
    return Definition(name, Archetype.Packet, fields, options)
end

function M.ClientDB(name, fields, options)
    return Definition(name, Archetype.ClientDB, fields, options)
end

function M.LuaEvent(name, fields, options)
    return Definition(name, Archetype.LuaEventData, fields, options)
end

function M.GameCommand(name, aliases, fields, options)
    local result = CopyOptions(options)
    result.aliases = aliases
    return Definition(name, Archetype.GameCommand, fields, result)
end

function M.DatabaseTable(name, fields, options)
    return Definition(name, Archetype.DatabaseTable, fields, options)
end

return M

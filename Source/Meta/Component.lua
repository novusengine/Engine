local M = OrderedTable()

local Type = require("Type")
local Output = require("Output")

local clientDBFieldTypes =
{
    [Type.I8] = true, [Type.I16] = true, [Type.I32] = true, [Type.I64] = true,
    [Type.U8] = true, [Type.U16] = true, [Type.U32] = true, [Type.U64] = true,
    [Type.F32] = true, [Type.F64] = true, [Type.STRINGREF] = true,
    [Type.VEC2] = true, [Type.VEC3] = true, [Type.VEC4] = true,
    [Type.IVEC2] = true, [Type.IVEC3] = true, [Type.IVEC4] = true,
    [Type.UVEC2] = true, [Type.UVEC3] = true, [Type.UVEC4] = true
}

--[[
    Available Fields
    - include (table)
        - project (array - strings)
        - system (array - strings)
    - flags (table)
        - supportsEnum (boolean)
        - supportsStruct (boolean)

    Available Functions
    - PreGenerate (self, type)
    - Generate (self, type)
    - PostGenerate (self, type)
    - OnGeneratorComplete (self, srcRootDir, outputRootDir, includeRootDir)
    - ResetGenerationState (self)
--]]

M.Debug =
{
    target = "cpp",
    include =
    {
        system =
        {
            "Base/Types.h",
            "string"
        }
    },

    flags =
    {
        supportsEnum = false,
        supportsStruct = true
    },

    Validate = function(self, definition)
        for _, fieldInfo in ipairs(definition.fields) do
            local fieldAttributes = fieldInfo.attributes
            local shouldDebug = fieldAttributes == nil or fieldAttributes.debug ~= false
            if shouldDebug and fieldInfo.type.ToString == nil then
                error("field '" .. fieldInfo.name .. "' uses debug-unsupported type '" .. fieldInfo.type.name .. "'. Set debug = false on the field or add ToString support to the type")
            end
        end
    end,

    Generate = function(self, type, context)
        local cpp = context.cpp
        cpp:Function({ name = "ToString", returns = Type.STRING, const = true }, function()
            cpp:Variable(Type.STRING, "tmp", cpp:String(""))
            cpp:Statement(cpp:CallMember("tmp", "reserve", { 128 }))
            cpp:BlankLine()

            local debugFields = {}
            for _, fieldTable in ipairs(type.fields) do
                local fieldAttributes = fieldTable.attributes
                if fieldAttributes == nil or fieldAttributes.debug ~= false then
                    table.insert(debugFields, fieldTable)
                end
            end

            local numFields = #debugFields
            for k, fieldTable in ipairs(debugFields) do
                local fieldInfo = fieldTable

                local expression = "tmp += " .. fieldInfo.type:ToString(fieldInfo)

                if k < numFields then
                    expression = expression .. " + " .. cpp:String(", ")
                end
                cpp:Statement(expression)
            end

            if numFields > 0 then
                cpp:BlankLine()
            end
            cpp:Return("tmp")
        end)

        return true
    end
}

M.ClientDBMetaData =
{
    target = "cpp",
    include =
    {
        system =
        {
            "Base/Types.h",
            "FileFormat/Novus/ClientDB/ClientDB.h",
            "string",
            "vector"
        }
    },

    flags =
    {
        supportsEnum = false,
        supportsStruct = true
    },

    Validate = function(self, definition)
        if definition.name:sub(-6) == "Record" then
            error("ClientDB definition names must not include the generated Record suffix")
        end

        local recordName = definition.options.recordName
        if recordName ~= nil and (type(recordName) ~= "string" or recordName == "") then
            error("recordName must be a non-empty string when provided")
        end
        if recordName ~= nil and recordName:sub(-6) == "Record" then
            error("recordName must not include the generated Record suffix")
        end

        for _, fieldInfo in ipairs(definition.fields) do
            local fieldAttributes = fieldInfo.attributes
            local clientDBType = fieldInfo.type
            if fieldInfo.type == Type.ARRAY then
                clientDBType = fieldAttributes.type
            end

            if clientDBFieldTypes[clientDBType] == nil then
                error("field '" .. fieldInfo.name .. "' uses ClientDB-unsupported type '" .. clientDBType.name .. "'")
            end
        end
    end,

    Generate = function(self, type, context)
        local cpp = context.cpp
        local clientDBName = type.model.name
        cpp:Variable(Type.STRING, "NAME", cpp:String(clientDBName), { static = true, inline = true })
        cpp:Variable(Type.U32, "NAME_HASH", context.hash(clientDBName), { static = true, constexpr = true })

        local fields = {}
        for _, fieldInfo in ipairs(type.fields) do
            local fieldType = fieldInfo.type == Type.ARRAY and fieldInfo.attributes.type or fieldInfo.type
            local values = { cpp:String(fieldInfo.name), "::ClientDB::FieldType::" .. fieldType.name }
            if fieldInfo.type == Type.ARRAY then table.insert(values, fieldInfo.attributes.count) end
            table.insert(fields, cpp:InitList(values))
        end
        cpp:Variable(cpp:Template(Type.VECTOR, { "::ClientDB::FieldInfo" }), "FIELD_LIST", cpp:InitList(fields), { static = true, inline = true })

        return true
    end
}

M.Serialization =
{
    target = "cpp",
    include =
    {
        system =
        {
            "Base/Types.h",
            "Base/Memory/Bytebuffer.h",
        }
    },

    flags =
    {
        supportsEnum = false,
        supportsStruct = true
    },

    Validate = function(self, definition)
        for _, fieldInfo in ipairs(definition.fields) do
            local fieldAttributes = fieldInfo.attributes
            local shouldSerialize = fieldAttributes == nil or fieldAttributes.serialize ~= false

            if shouldSerialize then
                if fieldInfo.type.flags ~= nil and fieldInfo.type.flags.serializable == false then
                    error("field '" .. fieldInfo.name .. "' uses explicitly non-serializable type '" .. fieldInfo.type.name .. "'. Set serialize = false on the field or add a complete framing policy to the type")
                end

                if fieldInfo.type.ValidateSerialization ~= nil then
                    fieldInfo.type:ValidateSerialization(fieldInfo, fieldAttributes)
                end

                local requiredOperations = { "SerializeExpr", "DeserializeExpr", "SerializedSizeExpr" }
                for _, operation in ipairs(requiredOperations) do
                    if fieldInfo.type[operation] == nil then
                        error("field '" .. fieldInfo.name .. "' uses type '" .. fieldInfo.type.name .. "' without '" .. operation .. "'. Add the operation or explicitly set serialize = false on the field")
                    end
                end
            end
        end
    end,

    Generate = function(self, type, context)
        local cpp = context.cpp
        local numFields = #type.fields

        -- Serialization
        cpp:Function({ name = "Serialize", returns = Type.BOOL, parameters = {{ name = "buffer", type = Type.BYTEBUFFER, flags = { ptr = true } }}, const = true }, function()
            if numFields == 0 then
                cpp:Return("true")
            else
                cpp:Variable(Type.BOOL, "failed", "false")
                cpp:BlankLine()

                for _, v in ipairs(type.fields) do
                    local fieldInfo = v
                    local fieldAttributes = v.attributes

                    if fieldAttributes == nil or fieldAttributes.serialize ~= false then
                        cpp:Statement("failed |= !buffer->" .. fieldInfo.type:SerializeExpr(fieldInfo, fieldAttributes))
                    end
                end

                cpp:BlankLine()
                cpp:Return("!failed")
            end
        end)

        -- Deserialization
        cpp:Function({ name = "Deserialize", returns = Type.BOOL, parameters = {{ name = "buffer", type = Type.BYTEBUFFER, flags = { ptr = true } }} }, function()
            if numFields == 0 then
                cpp:Return("true")
            else
                cpp:Variable(Type.BOOL, "failed", "false")
                cpp:BlankLine()

                for _, v in ipairs(type.fields) do
                    local fieldInfo = v
                    local fieldAttributes = v.attributes

                    if fieldAttributes == nil or fieldAttributes.serialize ~= false then
                        cpp:Statement("failed |= !buffer->" .. fieldInfo.type:DeserializeExpr(fieldInfo, fieldAttributes))
                    end
                end

                cpp:BlankLine()
                cpp:Return("!failed")
            end
        end)

        cpp:Function({ name = "GetSerializedSize", returns = Type.U32, const = true }, function()
            cpp:Variable(Type.U32, "result", 0)

            if numFields > 0 then
                cpp:BlankLine()
            end

            for _, v in ipairs(type.fields) do
                local fieldInfo = v
                local fieldAttributes = v.attributes

                if fieldAttributes == nil or fieldAttributes.serialize ~= false then
                    cpp:Statement("result += " .. fieldInfo.type:SerializedSizeExpr(fieldInfo, fieldAttributes), fieldInfo.name)
                end
            end

            if numFields > 0 then
                cpp:BlankLine()
            end
            cpp:Return("result")
        end)

        return true
    end
}

M.PacketMetaData =
{
    target = "cpp",
    include =
    {
        system =
        {
            "Base/Types.h",
            "string"
        }
    },

    flags =
    {
        supportsEnum = false,
        supportsStruct = true
    },

    nextPacketID = 1,
    packets = { },

    ResetGenerationState = function(self)
        self.nextPacketID = 1
        self.packets = {}
    end,

    Generate = function(self, type, context)
        local cpp = context.cpp
        cpp:Variable(Type.STRING, "NAME", cpp:String(type.name), { static = true, inline = true })
        cpp:Variable(Type.U32, "NAME_HASH", context.hash(type.name), { static = true, constexpr = true })
        cpp:Variable(Type.U16, "PACKET_ID", self.nextPacketID, { static = true, constexpr = true })

        self.nextPacketID = self.nextPacketID + 1
        table.insert(self.packets, type.name)

        return true
    end,

    OnGeneratorComplete = function(self, srcRootDir, outputRootDir, includeRootDir, context)
        local numPackets = #self.packets
        if numPackets > 0 then
            local outputFilePath = outputRootDir .. "/PacketList.h"
            local includeFilePath = includeRootDir .. "/PacketList.h"
            Output.Write(context, outputFilePath, function()
            local cpp = context.cpp
            cpp:Line("#pragma once")
            cpp:BlankLine()
            cpp:Line("#include <Base/Types.h>")
            cpp:BlankLine()
            cpp:Line("#include <array>")
            cpp:Line("#include <string>")
            cpp:Line("#include <utility>")
            cpp:BlankLine()

            cpp:Block("namespace MetaGen", function()

            -- Write Enum
            local enumName = "PacketListEnum"
            local enumType = Type.U16
            cpp:Block("enum class " .. enumName .. " : " .. enumType.name, function()
                cpp:Line("Invalid = 0u,")
                for k, v in ipairs(self.packets) do
                    cpp:Line(v .. " = " .. k .. ",")
                end
                cpp:Line("Count = " .. numPackets + 1)
            end, ";")

            -- Write Enum Meta
            cpp:Struct(enumName .. "Meta", function()
                cpp:Using("Type", Type.U16.name)
                cpp:BlankLine()

                cpp:Variable(Type.U16, "ENUM_ID", M.EnumMetaData.nextEnumID, { static = true, constexpr = true })
                cpp:Variable(Type.STRING_VIEW, "ENUM_NAME", cpp:String(enumName), { static = true, constexpr = true })
                local fields = { cpp:Call(Type.PAIR.name, { cpp:String("Invalid"), "0u" }) }
                for i, fieldName in ipairs(self.packets) do
                    table.insert(fields, cpp:Call(Type.PAIR.name, { cpp:String(fieldName), tostring(i) .. "u" }))
                end
                table.insert(fields, cpp:Call(Type.PAIR.name, { cpp:String("Count"), tostring(numPackets + 1) .. "u" }))
                local fieldType = cpp:Template(Type.PAIR, { "std::string_view", enumType.name })
                cpp:Variable(cpp:Template(Type.ARRAY, { fieldType, numPackets + 2 }), "ENUM_FIELD_LIST", cpp:InitList(fields), { static = true, constexpr = true })
            end)
            cpp:Statement("DECLARE_GENERIC_BITWISE_OPERATORS(" .. enumName .. ")")

            M.EnumMetaData.nextEnumID = M.EnumMetaData.nextEnumID + 1

            table.insert(M.EnumMetaData.enums, { namespace = context.namespaceBase, name = enumName })
            M.EnumMetaData.enumFilesToInclude[includeFilePath] = 1

            end, nil, false)
            end)
        end
    end
}

M.EnumMetaData =
{
    target = "cpp",
    include =
    {
        system =
        {
            "Base/Types.h",
            "array",
            "string_view",
            "utility"
        }
    },

    flags =
    {
        supportsEnum = true,
        supportsStruct = false
    },

    nextEnumID = 0,
    enums = { },
    enumFilesToInclude = { },

    ResetGenerationState = function(self)
        self.nextEnumID = 0
        self.enums = {}
        self.enumFilesToInclude = {}
    end,

    PostGenerate = function(self, def, context)
        local cpp = context.cpp
        local numFields = #def.fields

        cpp:Struct(def.name .. "Meta", function()
            cpp:Using("Type", def.enumType.name)
            cpp:BlankLine()

            cpp:Variable(Type.U16, "ENUM_ID", self.nextEnumID, { static = true, constexpr = true })
            cpp:Variable(Type.STRING_VIEW, "ENUM_NAME", cpp:String(def.name), { static = true, constexpr = true })
            local fields = {}
            for _, field in ipairs(def.fields) do
                local value = tostring(field.resolvedValue) .. (def.enumType.suffix or "")
                table.insert(fields, cpp:Call(Type.PAIR.name, { cpp:String(field.name), value }))
            end
            local fieldType = cpp:Template(Type.PAIR, { "std::string_view", def.enumType.name })
            cpp:Variable(cpp:Template(Type.ARRAY, { fieldType, numFields }), "ENUM_FIELD_LIST", cpp:InitList(fields), { static = true, constexpr = true })
        end)

        cpp:Statement("DECLARE_GENERIC_BITWISE_OPERATORS(" .. def.name .. ")")

        self.nextEnumID = self.nextEnumID + 1

        table.insert(self.enums, { namespace = def.namespace, name = def.name })
        self.enumFilesToInclude[def.path] = 1

        return true
    end,

    OnGeneratorComplete = function(self, srcRootDir, outputRootDir, includeRootDir, context)
        local numEnums = #self.enums
        if numEnums > 0 then
            local filePath = outputRootDir .. "/EnumTraits.h"
            Output.Write(context, filePath, function()
            local cpp = context.cpp
            cpp:Line("#pragma once")
            cpp:BlankLine()

            local enumIncludeList = {}
            for k, _ in pairs(self.enumFilesToInclude) do
                table.insert(enumIncludeList, k)
            end
            table.sort(enumIncludeList, context.pathSort)

            local numEnumIncludes = #enumIncludeList
            for i, v in ipairs(enumIncludeList) do
                cpp:Line("#include \"" .. v .. "\"")

                if i == numEnumIncludes then
                    cpp:BlankLine()
                end
            end

            cpp:Line("#include <Base/Types.h>")
            cpp:BlankLine()

            local numEnums = #self.enums
            for k, v in ipairs(self.enums) do
                cpp:Line("template <>")
                cpp:Struct("EnumTraits<" .. v.namespace .. "::" .. v.name .. ">", function()
                    cpp:Using("Meta", v.namespace .. "::" .. v.name .. "Meta")
                end, k < numEnums)

                if k < numEnums then
                    cpp:BlankLine()
                end
            end

            end)
        end
    end
}

M.LuaEventMetaData =
{
    target = "cpp",
    include =
    {
        system =
        {
            "Base/Types.h",
            "string",
        }
    },

    flags =
    {
        supportsEnum = false,
        supportsStruct = true
    },

    nextStructID = 1,

    ResetGenerationState = function(self)
        self.nextStructID = 1
    end,

    Generate = function(self, type, context)
        local cpp = context.cpp
        local numFields = #type.fields

        cpp:Variable(Type.STRING, "NAME", cpp:String(type.name), { static = true, inline = true })
        cpp:Variable(Type.U32, "NAME_HASH", context.hash(type.name), { static = true, constexpr = true })
        cpp:Variable(Type.U16, "STRUCT_ID", self.nextStructID, { static = true, constexpr = true })
        cpp:Variable(Type.U16, "NUM_PARAMETERS", numFields, { static = true, constexpr = true })

        self.nextStructID = self.nextStructID + 1

        return true
    end
}

M.LuaSerialization =
{
    target = "cpp",
    include =
    {
        system =
        {
            "Base/Types.h",
            "lua.h"
        }
    },

    flags =
    {
        supportsEnum = false,
        supportsStruct = true
    },

    Validate = function(self, definition)
        for _, fieldInfo in ipairs(definition.fields) do
            local fieldAttributes = fieldInfo.attributes
            local shouldPush = fieldAttributes == nil or fieldAttributes.luaPush ~= false

            if shouldPush and fieldInfo.type.EmitLuaPush == nil then
                error("field '" .. fieldInfo.name .. "' uses Lua-unsupported type '" .. fieldInfo.type.name .. "'. Set luaPush = false on the field or add EmitLuaPush support to the type")
            end
            if shouldPush and (fieldInfo.type == Type.ARRAY or fieldInfo.type == Type.VECTOR) and fieldAttributes.type.EmitLuaPush == nil then
                error("container field '" .. fieldInfo.name .. "' uses Lua-unsupported element type '" .. fieldAttributes.type.name .. "'")
            end
        end
    end,

    Generate = function(self, type, context)
        local cpp = context.cpp
        local numFields = #type.fields

        cpp:Function({ name = "Push", returns = Type.VOID, parameters = {{ name = "state", type = Type.LUASTATE, flags = { ptr = true } }}, const = true }, function()
            cpp:Statement(cpp:Call("lua_newtable", { "state" }))

            if numFields > 0 then
                cpp:BlankLine()
            end

            for i, v in ipairs(type.fields) do
                local fieldInfo = v
                local fieldAttributes = v.attributes

                if (fieldAttributes == nil or fieldAttributes.luaPush ~= false) and fieldInfo.type.EmitLuaPush ~= nil then
                    cpp:Statement(cpp:Call("lua_pushstring", { "state", cpp:String(fieldInfo.name) }))
                    fieldInfo.type:EmitLuaPush(fieldInfo, fieldAttributes, context)
                    if fieldInfo.type ~= Type.ARRAY and fieldInfo.type ~= Type.VECTOR then
                        cpp:BlankLine()
                    end
                    cpp:Statement(cpp:Call("lua_settable", { "state", "-3" }))
                end

                if i < numFields then
                    cpp:BlankLine()
                end
            end
        end)

        return true
    end
}

M.GameCommand =
{
    target = "cpp",
    include =
    {
        system =
        {
            "Base/Types.h",
            "array",
            "numeric",
            "stdexcept",
            "vector",
            "string",
            "tuple",
            "utility"
        }
    },

    flags =
    {
        supportsEnum = false,
        supportsStruct = true
    },

    Validate = function(self, definition)
        if #definition.aliases == 0 then
            error("game command requires at least one alias")
        end

        for _, fieldInfo in ipairs(definition.fields) do
            if fieldInfo.type.CommandReadExpr == nil then
                error("field '" .. fieldInfo.name .. "' uses command-unsupported type '" .. fieldInfo.type.name .. "'")
            end
        end
    end,

    Generate = function(self, type, context)
        local cpp = context.cpp
        local numParameters = #type.fields
        local numParametersOptional = 0

        local numAliases = 0
        if type.aliases ~= nil then
            numAliases = #type.aliases
        end

        cpp:Variable(Type.U32, "NUM_PARAMETERS", numParameters, { static = true, constexpr = true })
        cpp:Variable(Type.U32, "NUM_PARAMETERS_OPTIONAL", numParametersOptional, { static = true, constexpr = true })
        cpp:BlankLine()

        local commandHelpText = "("
        for i, v in ipairs(type.fields) do
            local fieldInfo = v

            commandHelpText = commandHelpText .. fieldInfo.name .. " : " .. fieldInfo.type.name

            if i < numParameters then
                commandHelpText = commandHelpText .. ", "
            end
        end
        commandHelpText = commandHelpText .. ")"
        cpp:Variable(Type.STRING_VIEW, "COMMAND_HELP_MESSAGE", cpp:String(commandHelpText), { static = true, constexpr = true })
        local aliases = {}
        for _, alias in ipairs(type.aliases) do table.insert(aliases, cpp:String(alias:lower())) end
        cpp:Variable(cpp:Template("std::array", { "std::string_view", numAliases }), "COMMAND_NAME_LIST", cpp:InitList(aliases), { static = true, constexpr = true })
        local parameterNames = {}
        local parameterTypes = {}
        for _, fieldInfo in ipairs(type.fields) do
            table.insert(parameterNames, cpp:String(fieldInfo.name))
            table.insert(parameterTypes, fieldInfo.type.name)
        end
        cpp:Variable(cpp:Template("std::array", { "std::string_view", numParameters }), "PARAMETER_NAME_LIST", cpp:InitList(parameterNames), { static = true, constexpr = true })
        cpp:Using("PARAMETER_TYPE_LIST", cpp:Template("std::tuple", parameterTypes))

        local readFuncParameters =
        {
            { name = "parameters", type = Type.VECTOR, subType = Type.STRING, flags = { ref = true } },
            { name = "result", type = type.name, flags = { ref = true }}
        }
        cpp:BlankLine()
        cpp:Function({ name = "Read", returns = Type.BOOL, parameters = readFuncParameters, static = true }, function()
            if numParameters == 0 then
                cpp:Assign("result", "{}")
                cpp:Return("true")
            else
                cpp:Variable(Type.U32, "NUM_REQUIRED_PARAMETERS", "NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL", { static = true, constexpr = true })
                cpp:BlankLine()

                cpp:Variable(Type.U32, "numParams", cpp:Cast(Type.U32, cpp:CallMember("parameters", "size")))
                cpp:Statement("if (numParams < NUM_REQUIRED_PARAMETERS) return false")
                cpp:Statement("if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false")
                cpp:BlankLine()

                cpp:Block("try", function()
                    cpp:Variable(type.name, "tmp", "{}")
                    cpp:BlankLine()

                    for i, v in ipairs(type.fields) do
                        local fieldInfo = v
                        local fieldAttributes = v.attributes
                        cpp:Assign("tmp." .. fieldInfo.name, fieldInfo.type:CommandReadExpr(fieldInfo, fieldAttributes, i - 1, context))

                        if i == numParameters then
                            cpp:BlankLine()
                        end
                    end

                    cpp:Assign("result", cpp:Call("std::move", { "tmp" }))
                end)
                cpp:Line("catch (const std::exception&) { return false; }")
                cpp:BlankLine()

                cpp:Return("true")
            end
        end)

        return true
    end
}

return M

local M = OrderedTable()

local Type = require("Type")

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
    - OnGeneratorComplete (self, srcRootDir, binRootDir)
--]]

M.Debug =
{
    include =
    {
        system =
        {
            "Base/Types.h"
        }
    },

    flags =
    {
        supportsEnum = false,
        supportsStruct = true
    },

    Generate = function(self, type)
        MetaGen:AddFunction("ToString", Type.STRING, nil, { const = true }, function()
            MetaGenBuilder:Type(Type.STRING):Name("tmp"):Assign():String(""):End()
            MetaGenBuilder:Unknown("tmp"):Call("reserve", function()
                MetaGenBuilder:Unknown("128")
            end):End()
            MetaGen:NewLine()

            local numFields = #type.table.fields
            for k, v in ipairs(type.table.fields) do
                local fieldInfo = v[1]

                MetaGenBuilder:Unknown("tmp"):AddAssign():Unknown(fieldInfo.type:ToString(fieldInfo))

                if k < numFields then
                    MetaGenBuilder:Add():String(", ")
                end

                MetaGenBuilder:End()
            end

            if numFields > 0 then
                MetaGen:NewLine()
            end
            MetaGenBuilder:Return():Unknown("tmp"):End()
        end)

        return true
    end
}

M.ClientDBMetaData =
{
    include =
    {
        system =
        {
            "Base/Types.h",
            "FileFormat/Novus/ClientDB/ClientDB.h"
        }
    },

    flags =
    {
        supportsEnum = false,
        supportsStruct = true
    },

    Generate = function(self, type)
        MetaGenBuilder:Static():Inline():Type(Type.STRING):Name("NAME"):Assign():String(type.name):End()
        MetaGenBuilder:Static():Constexpr():Type(Type.U32):Name("NAME_HASH"):Assign():Value(fnv1a_32(type.name)):End()

        MetaGenBuilder:Static():Inline():Type(Type.VECTOR):Template("::ClientDB::FieldInfo"):Name("FIELD_LIST"):Assign():InitList(function()          
            local numFields = #type.table.fields
            MetaGen:AddIndent()

            for k, v in ipairs(type.table.fields) do
                local fieldInfo = v[1]
                local fieldAttributes = v[2]

                MetaGenBuilder:InitList(function()
                    if fieldInfo.type == Type.ARRAY then
                        MetaGenBuilder:String(fieldInfo.name):Comma():Unknown("::ClientDB::FieldType::"):Unknown(fieldAttributes.type.name):Comma():Value(fieldAttributes.count)
                    else
                        MetaGenBuilder:String(fieldInfo.name):Comma():Unknown("::ClientDB::FieldType::"):Unknown(fieldInfo.type.name)
                    end
                end)

                if k < numFields then
                    MetaGenBuilder:Comma()
                end
            end

            MetaGen:SubIndent()
        end):End()

        return true
    end
}

M.Serialization =
{
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

    Generate = function(self, type)
        local numFields = #type.table.fields

        -- Serialization
        MetaGen:AddFunction("Serialize", Type.BOOL, {{ name = "buffer", type = Type.BYTEBUFFER, flags = { ptr = true } }}, { const = true }, function() 
            if numFields == 0 then
                MetaGenBuilder:Return():Unknown("true"):End()
            --elseif isPOD then -- TODO : This needs to calculate if alignment is perfect, otherwise we can't simply serialize the entire struct
            --    MetaGenBuilder:Type(Type.BOOL):Name("result"):Assign():Unknown("buffer"):CallPtr("Put", function()
            --        MetaGenBuilder:Deref():Unknown("this")
            --    end):End()

            --    MetaGenBuilder:Return():Unknown("result"):End()
            else
                MetaGenBuilder:Type(Type.BOOL):Name("failed"):Assign():Value("false"):End()
                MetaGen:NewLine()

                for _, v in ipairs(type.table.fields) do
                    local fieldInfo = v[1]
                    local fieldAttributes = v[2]
                    
                    if fieldInfo.type.Serialize ~= nil then
                        MetaGenBuilder:Unknown("failed"):OrAssign():Not():Unknown("buffer"):Arrow():Unknown(fieldInfo.type:Serialize(fieldInfo, fieldAttributes)):End()
                    end
                end
                
                MetaGen:NewLine()
                
                MetaGenBuilder:Return():Not():Unknown("failed"):End()
            end
        end)

        -- Deserialization
        MetaGen:AddFunction("Deserialize", Type.BOOL, {{ name = "buffer", type = Type.BYTEBUFFER, flags = { ptr = true } }}, nil, function()
            if numFields == 0 then
                MetaGenBuilder:Return():Unknown("true"):End()
            --elseif isPOD then -- TODO : This needs to calculate if alignment is perfect, otherwise we can't simply deserialize the entire struct
            --    MetaGenBuilder:Type(Type.BOOL):Name("result"):Assign():Unknown("buffer"):CallPtr("Get", function()
            --        MetaGenBuilder:Deref():Unknown("this")
            --    end):End()

            --    MetaGenBuilder:Return():Unknown("result"):End()
            else
                MetaGenBuilder:Type(Type.BOOL):Name("failed"):Assign():Value("false"):End()
                MetaGen:NewLine()

                for _, v in ipairs(type.table.fields) do
                    local fieldInfo = v[1]
                    local fieldAttributes = v[2]
                    
                    if fieldInfo.type.Deserialize ~= nil then
                        MetaGenBuilder:Unknown("failed"):OrAssign():Not():Unknown("buffer"):Arrow():Unknown(fieldInfo.type:Deserialize(fieldInfo, fieldAttributes)):End()
                    end
                end
                
                MetaGen:NewLine()
                MetaGenBuilder:Return():Not():Unknown("failed"):End()
            end
        end)

        MetaGen:AddFunction("GetSerializedSize", Type.U32, nil, { const = true }, function()
            MetaGenBuilder:Type(Type.U32):Name("result"):Assign():Value(0):End()

            if numFields > 0 then
                MetaGen:NewLine()
            end

            for _, v in ipairs(type.table.fields) do
                local fieldInfo = v[1]
                local fieldAttributes = v[2]
                
                if fieldInfo.type.GetSerializedSize ~= nil then
                    MetaGenBuilder:Unknown("result"):AddAssign():Unknown(fieldInfo.type:GetSerializedSize(fieldInfo, fieldAttributes)):SemiColon():Comment(fieldInfo.name):NewLine():Flush()
                end
            end

            if numFields > 0 then
                MetaGen:NewLine()
            end
            
            MetaGenBuilder:Return():Unknown("result"):End()
        end)

        return true
    end
}

M.PacketMetaData =
{
    include =
    {
        system =
        {
            "Base/Types.h",
            "FileFormat/Novus/ClientDB/ClientDB.h"
        }
    },

    flags =
    {
        supportsEnum = false,
        supportsStruct = true
    },

    nextPacketID = 1,
    packets = { },

    Generate = function(self, type)
        MetaGenBuilder:Static():Inline():Type(Type.STRING):Name("NAME"):Assign():String(type.name):End()
        MetaGenBuilder:Static():Constexpr():Type(Type.U32):Name("NAME_HASH"):Assign():Value(fnv1a_32(type.name)):End()
        MetaGenBuilder:Static():Constexpr():Type(Type.U16):Name("PACKET_ID"):Assign():Value(self.nextPacketID):End()

        self.nextPacketID = self.nextPacketID + 1
        table.insert(self.packets, type.name)

        return true
    end,

    OnGeneratorComplete = function(self, srcRootDir, binRootDir)
        local numPackets = #self.packets
        if numPackets > 0 then
            local filePath = binRootDir .. "/PacketList.h"
            local file = assert(io.open(filePath, "w"))
            
            MetaGen.currentFile = file

            MetaGen:Write("#pragma once\n\n")
            MetaGen:Write("#include <Base/Types.h>\n\n")
            MetaGen:Write("#include <array>\n")
            MetaGen:Write("#include <string>\n")
            MetaGen:Write("#include <utility>\n\n")

            MetaGen:Write("namespace MetaGen\n")
            MetaGen:Write("{\n")
            MetaGen:AddIndent()

            -- Write Enum
            local enumName = "PacketListEnum"
            local enumType = Type.U16
            MetaGen:Write("enum class " .. enumName .. " : " .. enumType.name)
            MetaGen:NewLine()
            MetaGen:Write("{\n")
            MetaGen:AddIndent()
            
            MetaGen:Write("Invalid = 0u,\n")
            for k, v in ipairs(self.packets) do
                local text = v .. " = " .. k .. ",\n"
                MetaGen:Write(text)
            end
            MetaGen:Write("Count = " .. numPackets + 1 .. "\n")

            MetaGen:SubIndent()
            MetaGen:Write("};\n")

            -- Write Enum Meta
            MetaGenBuilder:Struct(enumName .. "Meta"):Scope(function()
                MetaGenBuilder:Unknown("using Type"):Assign():Type(Type.U16):End()
                MetaGenBuilder:NewLine()

                MetaGenBuilder:Static():Constexpr():Type(Type.U16):Name("ENUM_ID"):Assign():Value(M.EnumMetaData.nextEnumID):End()
                MetaGenBuilder:Static():Constexpr():Type(Type.STRING_VIEW):Name("ENUM_NAME"):Assign():String(enumName):End()
                MetaGenBuilder:Static():Constexpr():Type(Type.ARRAY):ArrayTemplate("std::pair<std::string_view, " .. enumType.name .. ">", numPackets + 2):Name("ENUM_FIELD_LIST"):Assign():InitList(function()
                    MetaGenBuilder:Type(Type.PAIR):Parameters(function()
                        MetaGenBuilder:String("Invalid"):Comma():Value(0):Unknown("u")
                    end):Comma()

                    for i, fieldName in ipairs(self.packets) do
                        MetaGenBuilder:Type(Type.PAIR):Parameters(function()
                            MetaGenBuilder:String(fieldName):Comma():Value(i):Unknown("u")
                        end):Comma()
                    end

                    MetaGenBuilder:Type(Type.PAIR):Parameters(function()
                        MetaGenBuilder:String("Count"):Comma():Value(numPackets + 1):Unknown("u")
                    end)
                end):SemiColon():Flush()
            end):End()
            MetaGenBuilder:Unknown("DECLARE_GENERIC_BITWISE_OPERATORS(" .. enumName .. ")"):End()

            M.EnumMetaData.nextEnumID = M.EnumMetaData.nextEnumID

            table.insert(M.EnumMetaData.enums, { namespace = MetaGen.namespaceBase, name = enumName })
            M.EnumMetaData.enumFilesToInclude[filePath] = 1

            MetaGen:SubIndent()
            MetaGen:Write("}")
            file:close()
        end
    end
}

M.EnumMetaData =
{
    include =
    {
        system =
        {
            "Base/Types.h",
            "array"
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

    PostGenerate = function(self, def)
        local numFields = #def.table.fields

        MetaGenBuilder:Struct(def.name .. "Meta"):Scope(function()
            MetaGenBuilder:Unknown("using Type"):Assign():Type(def.table.type):End()
            MetaGenBuilder:NewLine()

            MetaGenBuilder:Static():Constexpr():Type(Type.U16):Name("ENUM_ID"):Assign():Value(self.nextEnumID):End()
            MetaGenBuilder:Static():Constexpr():Type(Type.STRING_VIEW):Name("ENUM_NAME"):Assign():String(def.name):End()
            MetaGenBuilder:Static():Constexpr():Type(Type.ARRAY):ArrayTemplate("std::pair<std::string_view, " .. def.table.type.name .. ">", numFields):Name("ENUM_FIELD_LIST"):Assign():InitList(function()
                for i, fieldTable in ipairs(def.table.fields) do
                    local field = fieldTable[1]

                    MetaGenBuilder:Type(Type.PAIR):Parameters(function()
                        MetaGenBuilder:String(field.name):Comma():Value(field.type)
                        if def.table.type.suffix ~= nil and type(field.type) == "number" then
                            MetaGenBuilder:Unknown(def.table.type.suffix)
                        end
                    end)

                    if i < numFields then
                        MetaGenBuilder:Comma()
                    end
                end
            end):SemiColon():Flush()
        end):End()

        MetaGenBuilder:Unknown("DECLARE_GENERIC_BITWISE_OPERATORS(" .. def.name .. ")"):End()

        self.nextEnumID = self.nextEnumID + 1

        local relPath = def.path:sub(#MetaGen.binRootDir + 2)
        local dir = relPath:match("(.*/)")
        local namespace = MetaGen.namespaceBase
        if dir then
            namespace = namespace .. "." .. dir:gsub("/", "."):sub(1, -2) -- remove trailing slash
        end

        local cppNamespace = namespace:gsub("%.", "::")

        table.insert(self.enums, { namespace = cppNamespace, name = def.name })
        self.enumFilesToInclude[def.path] = 1

        return true
    end,

    OnGeneratorComplete = function(self, srcRootDir, binRootDir)
        local numEnums = #self.enums
        if numEnums > 0 then
            local filePath = binRootDir .. "/EnumTraits.h"
            local file = assert(io.open(filePath, "w"))
            
            MetaGen.currentFile = file

            MetaGen:Write("#pragma once\n\n")

            local enumIncludeList = {}
            for k, _ in pairs(self.enumFilesToInclude) do
                table.insert(enumIncludeList, k)
            end
            table.sort(enumIncludeList, PathCompareSort)

            local numEnumIncludes = #enumIncludeList
            for i, v in ipairs(enumIncludeList) do
                MetaGen:Write("#include \"" .. v .. "\"\n")

                if i == numEnumIncludes then
                    MetaGen:NewLine()
                end
            end

            MetaGen:Write("#include <Base/Types.h>\n\n")

            local numEnums = #self.enums
            for k, v in ipairs(self.enums) do
                MetaGen:Write("template <>\n")
                MetaGen:Write("struct EnumTraits<" .. v.namespace .. "::" .. v.name .. ">\n")
                MetaGen:Write("{\n")

                MetaGen:AddIndent()
                MetaGen:Write("using Meta = " .. v.namespace .. "::" .. v.name .. "Meta;\n")
                MetaGen:SubIndent()

                MetaGen:Write("};")

                if k < numEnums then
                    MetaGen:Write("\n\n")
                end
            end

            file:close()
        end
    end
}

M.LuaEventMetaData =
{
    include =
    {
        system =
        {
            "Base/Types.h",
        }
    },

    flags =
    {
        supportsEnum = false,
        supportsStruct = true
    },

    nextStructID = 1,

    Generate = function(self, type)
        local numFields = #type.table.fields

        MetaGenBuilder:Static():Inline():Type(Type.STRING):Name("NAME"):Assign():String(type.name):End()
        MetaGenBuilder:Static():Constexpr():Type(Type.U32):Name("NAME_HASH"):Assign():Value(fnv1a_32(type.name)):End()
        MetaGenBuilder:Static():Constexpr():Type(Type.U16):Name("STRUCT_ID"):Assign():Value(self.nextStructID):End()
        MetaGenBuilder:Static():Constexpr():Type(Type.U16):Name("NUM_PARAMETERS"):Assign():Value(numFields):End()

        self.nextStructID = self.nextStructID + 1

        return true
    end
}

M.LuaSerialization =
{
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
    
    Generate = function(self, type)
        local numFields = #type.table.fields

        MetaGen:AddFunction("Push", Type.VOID, {{ name = "state", type = Type.LUASTATE, flags = { ptr = true } }}, { const = true }, function() 
            MetaGenBuilder:Unknown("lua_newtable(state)"):End()

            if numFields > 0 then
                MetaGen:NewLine()
            end

            for i, v in ipairs(type.table.fields) do
                local fieldInfo = v[1]
                local fieldAttributes = v[2]
                
                if fieldInfo.type.Push ~= nil then
                    MetaGenBuilder:Unknown("lua_pushstring(state, "):String(fieldInfo.name):Unknown(")"):End()
                    fieldInfo.type:Push(fieldInfo, fieldAttributes)
                    if fieldInfo.type ~= Type.ARRAY and fieldInfo.type ~= Type.VECTOR then
                        MetaGenBuilder:NewLine()
                    end
                    MetaGenBuilder:Unknown("lua_settable(state, -3)"):End()
                end

                if i < numFields then
                    MetaGenBuilder:NewLine()
                end
            end
        end)

        return true
    end
}

M.GameCommand =
{
    include =
    {
        system =
        {
            "Base/Types.h",
            "array",
            "numeric",
            "vector",
            "string",
            "tuple"
        }
    },

    flags =
    {
        supportsEnum = false,
        supportsStruct = true
    },

    Generate = function(self, type)
        local numParameters = #type.table.fields
        local numParametersOptional = 0

        local numAliases = 0
        if type.table.aliases ~= nil then
            numAliases = #type.table.aliases
        end

        MetaGenBuilder:Static():Constexpr():Type(Type.U32):Name("NUM_PARAMETERS"):Assign():Value(numParameters):End()
        MetaGenBuilder:Static():Constexpr():Type(Type.U32):Name("NUM_PARAMETERS_OPTIONAL"):Assign():Value(numParametersOptional):End()
        MetaGenBuilder:NewLine()

        local commandHelpText = "("
        for i, v in ipairs(type.table.fields) do
            local fieldInfo = v[1]
            
            commandHelpText = commandHelpText .. fieldInfo.name .. " : " .. fieldInfo.type.name

            if i < numParameters then
                commandHelpText = commandHelpText .. ", "
            end
        end
        commandHelpText = commandHelpText .. ")"
        MetaGenBuilder:Static():Constexpr():Type(Type.STRING_VIEW):Name("COMMAND_HELP_MESSAGE"):Assign():String(commandHelpText):End()
        MetaGenBuilder:Static():Constexpr():Unknown("std::array<std::string_view, " .. tostring(numAliases) .. ">"):Name("COMMAND_NAME_LIST"):Assign():InitList(function()
            for i, v in ipairs(type.table.aliases) do
                MetaGenBuilder:String(v:lower())

                if i < numAliases then
                    MetaGenBuilder:Comma()
                end
            end
        end):End()
        MetaGenBuilder:Static():Constexpr():Unknown("std::array<std::string_view, " .. tostring(numParameters) .. ">"):Name("PARAMETER_NAME_LIST"):Assign():InitList(function()
            for i, v in ipairs(type.table.fields) do
                local fieldInfo = v[1]
                MetaGenBuilder:String(fieldInfo.name)

                if i < numParameters then
                    MetaGenBuilder:Comma()
                end
            end
        end):End()
        MetaGenBuilder:Unknown("using PARAMETER_TYPE_LIST"):Assign():Unknown("std::tuple<")
        for i, v in ipairs(type.table.fields) do
            local fieldInfo = v[1]
            MetaGenBuilder:Unknown(fieldInfo.type.name)

            if i < numParameters then
                MetaGenBuilder:Comma()
            end
        end
        MetaGenBuilder:Unknown(">"):End()

        local readFuncParameters =
        {
            { name = "parameters", type = Type.VECTOR, subType = Type.STRING, flags = { ref = true } },
            { name = "result", type = type.name, flags = { ref = true }}
        }
        MetaGen:NewLine()
        MetaGen:AddFunction("Read", Type.BOOL, readFuncParameters, { static = true }, function() 
            if numParameters == 0 then
                MetaGenBuilder:Unknown("result"):Assign():Unknown("{}"):End()
                MetaGenBuilder:Return():Unknown("true"):End()
            else
                MetaGenBuilder:Static():Constexpr():Type(Type.U32):Name("NUM_REQUIRED_PARAMETERS"):Assign():Unknown("NUM_PARAMETERS - NUM_PARAMETERS_OPTIONAL"):End()         
                MetaGenBuilder:NewLine()

                MetaGenBuilder:Type(Type.U32):Name("numParams"):Assign():Unknown("static_cast<u32>(parameters.size())"):End()
                MetaGenBuilder:Unknown("if (numParams < NUM_REQUIRED_PARAMETERS) return false"):End()
                MetaGenBuilder:Unknown("if (numParams > NUM_REQUIRED_PARAMETERS + NUM_PARAMETERS_OPTIONAL) return false"):End()
                MetaGenBuilder:NewLine()

                MetaGenBuilder:Unknown("try"):Scope(function()          
                    MetaGenBuilder:Unknown(type.name):Name("tmp"):Assign():Unknown("{}"):End()
                    MetaGenBuilder:NewLine()

                    for i, v in ipairs(type.table.fields) do
                        local fieldInfo = v[1]
                        local fieldAttributes = v[2]

                        MetaGenBuilder:Unknown("tmp." .. fieldInfo.name):Assign()
                        if fieldInfo.type.CommandRead == nil then
                            error("Game Command : '" .. type.name .. "' has field '" .. fieldInfo.name .. "' with unsupported type '" .. fieldInfo.type.name .. "'")
                        end
                        fieldInfo.type:CommandRead(fieldInfo, fieldAttributes, i - 1)
                        MetaGenBuilder:End()

                        if i == numParameters then
                            MetaGenBuilder:NewLine()
                        end
                    end

                    MetaGenBuilder:Unknown("result"):Assign():Unknown("std::move(tmp);"):Flush()
                end):NewLine():Flush()
                MetaGenBuilder:Unknown("catch (const std::exception&) { return false; }"):NewLine():NewLine():Flush()
                
                MetaGenBuilder:Unknown("return true"):End()
            end
        end)

        return true
    end
}

return M

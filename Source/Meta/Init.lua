local _ = require("OrderedTable")
local Type = require("Type")
local Archetype = require("Archetype")
local Component = require("Component")

local info = debug.getinfo(1,'S')
local scriptPath = info.source:sub(2)
local root = scriptPath:match("(.*/)")

-- This is restored to the original inside of Meta.lua which includes this file
package.path = root .. "?.lua;" .. root .. "?/Init.lua;" .. package.path

MetaGen =
{
    sourceRootDir = "",
    binRootDir = "",
    namespaceBase = "",

    currentIndent = 0,
    currentFile = nil
}

function MetaGen:Write(msg, indent)
    local textIndent = self.currentIndent
    if type(indent) == "number" then
        textIndent = textIndent + indent
    end

    local spacesText = ""
    for i = 1, textIndent do
        spacesText = spacesText .. "    "
    end

    msg = spacesText .. msg
    self.currentFile:write(msg)
end

function MetaGen:GetIndent()
    return self.currentIndent
end

function MetaGen:AddIndent()
    self.currentIndent = self.currentIndent + 1
end
function MetaGen:SubIndent()
    self.currentIndent = self.currentIndent - 1
end
function MetaGen:NewLine()
    self.currentFile:write("\n")
end

function MetaGen:AddFunction(name, returnType, parameters, flags, callback)
    local retType = returnType or Type.VOID
    local params = parameters or {}
    local funcFlags = flags or {}

    local funcText = ""
    local paramText = ""
    local numParams = #params
    for i, v in ipairs(params) do
        local paramFlags = v.flags or {}
        local isArray = v.type == Type.ARRAY
        local isVector = v.type == Type.VECTOR
        local isPtr = paramFlags.ptr or false
        local isRef = paramFlags.ref or false

        local typeName = ""
        if type(v.type) == "string" then
            typeName = v.type
        else
            typeName = v.type.name
        end
        
        paramText = paramText .. typeName
        
        if isArray or isVector then
            local subTypeName = ""
            if type(v.subType) == "string" then
                subTypeName = v.subType
            else
                subTypeName = v.subType.name
            end

            paramText = paramText .. "<" .. subTypeName
            if isArray then
                paramText = paramText .. ", " .. tostring(v.count)
            end

            paramText = paramText .. ">"
        end

        if isPtr then
            paramText = paramText .. "*"
        elseif isRef then
            paramText = paramText .. "&"
        end

        paramText = paramText .. " " .. v.name

        if i < numParams then
            paramText = paramText .. ", "
        end
    end
    
    local isStatic = funcFlags.static or false
    local isConst = funcFlags.const or false

    if isStatic then
        funcText = funcText .. "static "
    end

    funcText = funcText .. retType.name .. " " .. name .. "(" .. paramText .. ")"
    if isConst then
        funcText = funcText .. " const"
    end

    funcText = funcText .. "\n"
    self:Write(funcText)
    
    self:Write("{\n")
    self:AddIndent()

    if callback then
        callback()
    end

    self:SubIndent()
    MetaGen:Write("}\n")
end

MetaGenBuilder =
{
    result = "",
    scopeLevel = 0
}
function MetaGenBuilder:Write(msg)
    local textIndent = MetaGen.currentIndent

    for i = 1, textIndent do
        self.result = self.result .. "    " .. msg
    end
end
function MetaGenBuilder:Name(name)
    self.result = self.result .. " " .. name
    return MetaGenBuilder
end

function MetaGenBuilder:Type(type)
    self.result = self.result .. type.name
    return MetaGenBuilder
end

function MetaGenBuilder:Struct(name)
    self.result = self.result .. "struct " .. name
    return MetaGenBuilder
end

function MetaGenBuilder:Scope(func)
    self.result = self.result .. "\n{\n"
    self:Flush()
    self.scopeLevel = self.scopeLevel + 1

    func()
    self:Flush()
    self.scopeLevel = self.scopeLevel - 1

    self.result = self.result .. "}"
    return MetaGenBuilder
end

function MetaGenBuilder:AddIndent()
    local textIndent = self.scopeLevel

    for i = 1, textIndent do
        self.result = self.result .. "    "
    end

    return MetaGenBuilder
end

function MetaGenBuilder:NewLine()
    self.result = self.result .. "\n"
    return MetaGenBuilder
end

function MetaGenBuilder:Unknown(name)
    self.result = self.result .. name
    return MetaGenBuilder
end

function MetaGenBuilder:Static()
    self.result = self.result .. "static "
    return MetaGenBuilder
end

function MetaGenBuilder:Inline()
    self.result = self.result .. "inline "
    return MetaGenBuilder
end

function MetaGenBuilder:Const()
    self.result = self.result .. "const "
    return MetaGenBuilder
end

function MetaGenBuilder:Constexpr()
    self.result = self.result .. "constexpr "
    return MetaGenBuilder
end

function MetaGenBuilder:Ref()
    self.result = self.result .. "&"
    return MetaGenBuilder
end

function MetaGenBuilder:Ptr()
    self.result = self.result .. "*"
    return MetaGenBuilder
end

function MetaGenBuilder:Add()
    self.result = self.result .. " + "
    return MetaGenBuilder
end

function MetaGenBuilder:Sub()
    self.result = self.result .. " - "
    return MetaGenBuilder
end

function MetaGenBuilder:Mul()
    self.result = self.result .. " * "
    return MetaGenBuilder
end

function MetaGenBuilder:Div()
    self.result = self.result .. " / "
    return MetaGenBuilder
end

function MetaGenBuilder:Assign()
    self.result = self.result .. " = "
    return MetaGenBuilder
end

function MetaGenBuilder:AddAssign()
    self.result = self.result .. " += "
    return MetaGenBuilder
end

function MetaGenBuilder:SubAssign()
    self.result = self.result .. " -= "
    return MetaGenBuilder
end

function MetaGenBuilder:MulAssign()
    self.result = self.result .. " *= "
    return MetaGenBuilder
end

function MetaGenBuilder:DivAssign()
    self.result = self.result .. " /= "
    return MetaGenBuilder
end

function MetaGenBuilder:OrAssign()
    self.result = self.result .. " |= "
    return MetaGenBuilder
end

function MetaGenBuilder:Return()
    self.result = self.result .. "return "
    return MetaGenBuilder
end

function MetaGenBuilder:Template(type)
    self.result = self.result .. "<" .. type .. ">"
    return MetaGenBuilder
end

function MetaGenBuilder:ArrayTemplate(...)
    local arg = { ... }
    local numArgs = #arg

    self.result = self.result .. "<"
    
    for i, v in ipairs(arg) do
        self.result = self.result .. tostring(v)

        if i < numArgs then
            self.result = self.result .. ", "
        end
    end

    self.result = self.result .. ">"

    return MetaGenBuilder
end

function MetaGenBuilder:InitList(func)
    self.result = self.result .. "{ "
    func()
    self.result = self.result .. " }"
    return MetaGenBuilder
end

function MetaGenBuilder:Comma()
    self.result = self.result .. ", "
    return MetaGenBuilder
end

function MetaGenBuilder:SemiColon()
    self.result = self.result .. ";"
    return MetaGenBuilder
end

function MetaGenBuilder:Comment(text)
    self.result = self.result .. " // " .. text
    return MetaGenBuilder
end

function MetaGenBuilder:Value(val)
    if type(val) == "number" then
        self.result = self.result .. tostring(val)
    elseif type(val) == "string" then
        self.result = self.result .. val
    end
    
    return MetaGenBuilder
end

function MetaGenBuilder:String(val)
    if type(val) == "number" then
        self.result = self.result .. "\"" .. tostring(val) .. "\""
    elseif type(val) == "string" then
        self.result = self.result .. "\"" .. val .. "\""
    end
    
    return MetaGenBuilder
end

function MetaGenBuilder:Deref()
    self.result = self.result .. "*"
    return MetaGenBuilder
end

function MetaGenBuilder:Arrow()
    self.result = self.result .. "->"
    return MetaGenBuilder
end

function MetaGenBuilder:Dot()
    self.result = self.result .. "."
    return MetaGenBuilder
end

function MetaGenBuilder:Not()
    self.result = self.result .. "!"
    return MetaGenBuilder
end

function MetaGenBuilder:Parameters(func)
    self.result = self.result .. "("
    func()
    self.result = self.result .. ")"
    return MetaGenBuilder
end

function MetaGenBuilder:Call(name, func)
    self.result = self.result .. "." .. name .. "("
    func()
    self.result = self.result .. ")"
    return MetaGenBuilder
end

function MetaGenBuilder:CallPtr(name, func)
    self.result = self.result .. "->" .. name .. "("
    func()
    self.result = self.result .. ")"
    return MetaGenBuilder
end

function MetaGenBuilder:End()
    self.result = self.result .. ";\n"

    for line, nl in string.gmatch(self.result, "([^\n]*)(\n?)") do
        if line ~= "" then
            MetaGen:Write(line .. nl, self.scopeLevel)
        else
            MetaGen:NewLine()
        end
    end

    self.result = ""
    return MetaGenBuilder
end

function MetaGenBuilder:Flush()
    for line, nl in string.gmatch(self.result, "([^\n]*)(\n?)") do
        if line ~= "" then
            MetaGen:Write(line .. nl, self.scopeLevel)
        else
            MetaGen:NewLine()
        end
    end

    self.result = ""
    return MetaGenBuilder
end

local function GetFileTimestamp(path)
    local info = os.stat(path)
    if info == nil then return 0 end

    return info.mtime
end

local function GetCacheFile(binRoot)
    local cacheFile =
    {
        files = {}
    }
    
    local path = binRoot .. "/Cache.mcache"
    local file = io.open(path, "rb")
    if file == nil then return cacheFile end

    local numFiles = string.unpack("<I4", file:read(4))
    for i = 1, numFiles do
        local timestamp = string.unpack("<I8", file:read(8))
        local pathLength = string.unpack("<I4", file:read(4))
        local path = file:read(pathLength)

        cacheFile.files[path] = timestamp
    end

    return cacheFile
end

local function FindDefinitionFiles(root)
    local pattern = root .. "/**.lua"
    return os.matchfiles(pattern)
end

local function GetPathToFileFromNamespace(root, namespace)
    local parts = {}
    for p in namespace:gmatch("[^:]+") do
        table.insert(parts, p)
    end

    -- Skip base namespace
    if #parts >= 1 then
        table.remove(parts, 1)
    end

    local leaf = parts[#parts] or "MetaGen"

    local folderPath = root
    for i = 1, #parts do
        folderPath = folderPath .. "/" .. parts[i]
    end

    local filePath = folderPath .. "/" .. leaf .. ".h"

    return folderPath, filePath
end

function PathCompareSort(a, b)
    local aa = {}
    for part in a:gmatch("[^/]+") do
        aa[#aa + 1] = part:lower()
    end

    local bb = {}
    for part in b:gmatch("[^/]+") do
        bb[#bb + 1] = part:lower()
    end

    -- Compare directory levels only (ignore filename)
    local adepth = #aa - 1
    local bdepth = #bb - 1

    -- Compare each directory level alphabetically
    local mind = math.min(adepth, bdepth)
    for i = 1, mind do
        if aa[i] ~= bb[i] then
            return aa[i] < bb[i]
        end
    end

    -- All shared directories match -> shorter directory path wins
    if adepth ~= bdepth then
        return adepth < bdepth
    end

    -- Same directory -> finally compare filenames
    return aa[#aa] < bb[#bb]
end

function Warn(text)
    print("\27[33m** Warn: " .. text .. "\27[39m")
end

function DeferWarning(list, type, msg)
    table.insert(list, { type = type, msg = msg })
end

function Field(name, type, attributes)
    return { { name = name, type = type }, attributes }
end

function fnv1a_32(str)
    local hash = 2166136261
    local prime = 16777619
    local uintMask = 0xFFFFFFFF

    for i = 1, #str do
        hash = ((hash ~ string.byte(str, i)))
        hash = ((hash * prime))
    end

    hash = ((hash ~ 0))
    hash = ((hash * prime) & uintMask)

    return hash
end

local sourceRootDir = "Definitions"
local binRootDir = "MetaGen"
local namespaceBase = "MetaGen"

MetaGen.sourceRootDir = sourceRootDir
MetaGen.binRootDir = binRootDir
MetaGen.namespaceBase = namespaceBase
MetaGen.cacheFile = GetCacheFile(binRootDir)
MetaGen.files = FindDefinitionFiles(sourceRootDir)

local cacheFile = MetaGen.cacheFile
local files = MetaGen.files
local filePathExist = {}

local requiresRebuild = false
for _, path in ipairs(files) do
    -- Check if file is not found in the cache
    local cachedTimestamp = cacheFile.files[path]
    if cachedTimestamp == nil then
        requiresRebuild = true
        break
    end

    -- Check Timestamp stored
    local fileTimestamp = GetFileTimestamp(path)
    if fileTimestamp ~= cachedTimestamp then
        requiresRebuild = true
        break
    end

    filePathExist[path] = 1
end

for k, v in pairs(cacheFile.files) do
    -- Check if cached file have been removed
    if filePathExist[k] == nil then
        requiresRebuild = true
        break
    end
end

if not requiresRebuild then
    print("MetaGen : No changes detected, skipping")
    return
else
    print("MetaGen : Building Files")
    os.rmdir(binRootDir)
end

local defineList = OrderedTable()

for _, path in ipairs(files) do
    local chunk = assert(loadfile(path))
    local defines = chunk()
    if (not defines) then goto NextFile end

    local relPath = path:sub(#sourceRootDir + 2)
    local dir = relPath:match("(.*/)")
    local namespace = namespaceBase
    if dir then
        namespace = namespace .. "." .. dir:gsub("/", "."):sub(1, -2) -- remove trailing slash
    end

    local cppNamespace = namespace:gsub("%.", "::")

    defineList[cppNamespace] = defineList[cppNamespace] or OrderedTable()
    local namespaceDefineList = defineList[cppNamespace]

    for name, def in pairs(defines) do
        if namespaceDefineList[name] then
            error("Duplicate definition in namespace ('" .. cppNamespace .. "') : " .. name)
        end

        namespaceDefineList[name] = def
    end
    
    ::NextFile::
end

-- DefineList == All Namespaces (Each have a table of definitions) --
local warningList = {}

for k, v in pairs(defineList) do
    local warnings = {}
    local folderPath, filePath = GetPathToFileFromNamespace(binRootDir, k)

    os.mkdir(folderPath)

    local file = assert(io.open(filePath, "w"))
    MetaGen.currentFile = file

    local numNamespaceTypes = 0
    local namespaceTypes = OrderedTable()
    local namespaceIncludes = OrderedTable()

    namespaceIncludes.project = OrderedTable()
    namespaceIncludes.numProjectIncludes = 0
    namespaceIncludes.system = OrderedTable()
    namespaceIncludes.numSystemIncludes = 0

    for typeName, typeTable in pairs(v) do
        local skipGen = false

        local baseType = nil
        local isBaseType = false
        local archeType = typeTable.archetype

        if archeType == nil then
            DeferWarning(warnings, typeName, "'archetype' must be set --Skipped")
            skipGen = true
        else
            if type(archeType) == "table" then
                baseType = archeType.base
            elseif type(archeType) == "number" then
                baseType = archeType
                isBaseType = true
            end

            if baseType == nil or (baseType ~= Type.ENUM and baseType ~= Type.STRUCT) then
                DeferWarning(warnings, typeName, "'archetype' or 'archetype.base' must be either Type.Enum or Type.Struct --Skipped")
                skipGen = true
            end
        end

        if typeTable.fields == nil then
            DeferWarning(warnings, typeName, "'fields' must be set --Skipped")
            skipGen = true
        end

        if skipGen then goto NextType end

        typeTable.numComponents = 0
        typeTable.components = {}
        local components = {}
        local componentsSeen = {}
        
        if typeTable.components ~= nil then
            for _, compTable in ipairs(typeTable.components) do
                if componentsSeen[compTable] == nil then
                    componentsSeen[compTable] = 1

                    table.insert(components, compTable)
                end
            end
        end

        if not isBaseType and archeType.components ~= nil then
            for _, compTable in ipairs(archeType.components) do
                if componentsSeen[compTable] == nil then
                    componentsSeen[compTable] = 1

                    table.insert(components, compTable)
                end
            end

            typeTable.components = components
            typeTable.numComponents = #components
        end

        for i, compTable in ipairs(components) do
            local includes = compTable.include
            if includes ~= nil then
                if includes.project ~= nil then
                    for _, include in ipairs(includes.project) do
                        namespaceIncludes.project[include] = 1   
                        namespaceIncludes.numProjectIncludes = namespaceIncludes.numProjectIncludes + 1
                    end
                end

                if includes.system ~= nil then
                    for _, include in ipairs(includes.system) do
                        namespaceIncludes.system[include] = 1
                        namespaceIncludes.numSystemIncludes = namespaceIncludes.numSystemIncludes + 1
                    end
                end
            end

            local componentSupportsBaseType = false
            local flags = compTable.flags

            if baseType == Type.ENUM and flags.supportsEnum then
                componentSupportsBaseType = true
            elseif baseType == Type.STRUCT and flags.supportsStruct then
                componentSupportsBaseType = true
            end

            if not componentSupportsBaseType then
                error("Type '" .. typeName .. "' uses component that is unsupported for its base type")
            end
        end

        numNamespaceTypes = numNamespaceTypes + 1
        namespaceTypes[typeName] = { path = filePath, name = typeName, type = baseType, table = typeTable }
        ::NextType::
    end

    MetaGen:Write("#pragma once\n\n")

    -- Write Includes
    local hasProjectIncludes = namespaceIncludes.numProjectIncludes > 0
    local hasSystemIncludes = namespaceIncludes.numSystemIncludes > 0
    local hasIncludes = hasProjectIncludes or hasSystemIncludes

    if hasProjectIncludes then
        local includes = {}
        for k, v in pairs(namespaceIncludes.project) do
            table.insert(includes, k)
        end
        table.sort(includes, PathCompareSort)

        for k, v in ipairs(includes) do
            MetaGen:Write("#include \"" .. v .. "\"\n")
        end

        if hasSystemIncludes then
            MetaGen:NewLine()
        end
    end

    if hasSystemIncludes then
        local includes = {}
        for k, v in pairs(namespaceIncludes.system) do
            table.insert(includes, k)
        end
        table.sort(includes, PathCompareSort)

        for k, v in ipairs(includes) do
            MetaGen:Write("#include <" .. v .. ">\n")
        end
    end

    if hasIncludes then
        MetaGen:NewLine()
    end

    MetaGen:Write("namespace " .. k .. "\n{\n")
    MetaGen:AddIndent()
    
    local typeCounter = 1
    for k, v in pairs(namespaceTypes) do
        for compIndex, compTable in ipairs(v.table.components) do
            if compTable.PreGenerate ~= nil then
                local result = compTable:PreGenerate(v)
                if result and compIndex < v.table.numComponents then
                    MetaGen:NewLine()
                end
            end
        end

        local numFields = #v.table.fields

        if v.type == Type.ENUM then
            MetaGen:Write("enum class " .. k .. " : " .. v.table.type.name .. "\n")
            MetaGen:Write("{\n")
            
            MetaGen:AddIndent()

            v.minVal = v.table.type.min
            v.maxVal = v.table.type.max
            v.nextVal = 0
            local valueSeen = false

            local fieldNameToVal = {}

            for i, fieldTable in ipairs(v.table.fields) do
                local field = fieldTable[1]
                local value = field.type or v.nextVal -- Reused field (Type for structs, Value for Enum)

                if type(value) ~= "string" then
                    if valueSeen and value < v.nextVal - 1 then
                        error("Enum '" .. k .. "' Field '" .. field.name .. "' is using value '" .. tostring(value) .. "' but next expected value is at least '" .. v.nextVal .. "'")
                    end

                    v.table.type:Validate(value)
                    
                    field.type = value
                    v.nextVal = value + 1
                    valueSeen = true
                end

                fieldNameToVal[field.name] = value
            end

            for i, fieldTable in ipairs(v.table.fields) do
                local field = fieldTable[1]
                local value = field.type

                if type(value) == "string" then
                    local realValue = fieldNameToVal[value]
                    if realValue ~= nil then
                        field.type = fieldNameToVal[value]
                    end
                end

                local text = field.name .. " = " .. tostring(value)

                if i < numFields then
                    text = text .. ",\n"
                end

                MetaGen:Write(text)
            end

            if v.table.numComponents then
                MetaGen:NewLine()

                for compIndex, compTable in ipairs(v.table.components) do
                    if compTable.Generate ~= nil then
                        local result = compTable:Generate(v)
                        if result and compIndex < v.table.numComponents then
                            MetaGen:NewLine()
                        end
                    end
                end
            end

            MetaGen:SubIndent()
            MetaGen:Write("};\n")
        elseif v.type == Type.STRUCT then
            MetaGen:Write("struct " .. k .. "\n")
            MetaGen:Write("{\n")
            
            if numFields > 0 then
                MetaGen:Write("public:\n")
            end

            MetaGen:AddIndent()

            v.totalSize = 0

            for _, fieldTable in ipairs(v.table.fields) do
                local field = fieldTable[1]
                local attributes = fieldTable[2]
                local hasDefault = attributes ~= nil and attributes.default ~= nil

                v.totalSize = v.totalSize + (field.type.size or 0)

                local text = field.type.name

                if field.type == Type.ARRAY then
                    text = text .. "<" .. attributes.type.name .. ", " .. attributes.count .. "> " .. field.name
                elseif field.type == Type.VECTOR then
                    text = text .. "<" .. attributes.type.name .. "> " .. field.name
                else
                    text = text .. " " .. field.name
                end

                if hasDefault then
                    local default = attributes.default

                    if field.type == Type.STRING then
                        text = text .. " = \"" .. tostring(default) .. "\";\n"
                    else
                        text = text .. " = " .. default .. ";\n"
                    end

                    MetaGen:Write(text)
                else
                    MetaGen:Write(text .. ";\n")
                end
            end

            if v.table.numComponents then
                MetaGen:SubIndent()
                
                if numFields > 0 then
                    MetaGen:NewLine()
                end

                MetaGen:Write("public:\n")
                MetaGen:AddIndent()

                for compIndex, compTable in ipairs(v.table.components) do
                    if compTable.Generate ~= nil then
                        local result = compTable:Generate(v)
                        if result and compIndex < v.table.numComponents then
                            MetaGen:NewLine()
                        end
                    end
                end
            end

            MetaGen:SubIndent()
            MetaGen:Write("};\n")
        end

        for compIndex, compTable in ipairs(v.table.components) do
            if compTable.PostGenerate ~= nil then
                local result = compTable:PostGenerate(v)
                if result and compIndex < v.table.numComponents then
                    MetaGen:NewLine()
                end
            end
        end

        if typeCounter < numNamespaceTypes then
            MetaGen:NewLine()
        end

        typeCounter = typeCounter + 1
    end

    MetaGen:SubIndent()
    file:write("}")
    
    file:close()
    
    MetaGen.currentFile = nil
    warningList[k] = warnings
end

for k, v in pairs(Component) do
    if v.OnGeneratorComplete ~= nil then
        v:OnGeneratorComplete(sourceRootDir, binRootDir)
    end
end

for k, v in pairs(warningList) do
    for _, warning in ipairs(v) do
        local text = "Namespace '" .. k .. "'"
        if warning.type ~= nil then
            text = text .. " " .. tostring(warning.type)
        end

        text = text .. " - " .. warning.msg
        Warn(text)
    end
end

-- Create Cache File
local filePath = binRootDir .. "/Cache.mcache"
local file = assert(io.open(filePath, "wb"))

local numFiles = #files
file:write(string.pack("<I4", numFiles))

for _, path in ipairs(files) do
    local pathLength = #path
    local fileTimestamp = GetFileTimestamp(path)
    
    file:write(string.pack("<I8", fileTimestamp))
    file:write(string.pack("<I4", pathLength))
    file:write(path)
end

file:close()
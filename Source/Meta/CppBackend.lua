local Type = require("Type")
local Component = require("Component")
local Output = require("Output")

local M = {}

local cppKeywords = {}
local cppKeywordList = "alignas alignof and and_eq asm atomic_cancel atomic_commit atomic_noexcept auto bitand bitor bool break case catch char char8_t char16_t char32_t class compl concept const consteval constexpr constinit const_cast continue co_await co_return co_yield decltype default delete do double dynamic_cast else enum explicit export extern false float for friend goto if inline int long mutable namespace new noexcept not not_eq nullptr operator or or_eq private protected public reflexpr register reinterpret_cast requires return short signed sizeof static static_assert static_cast struct switch synchronized template this thread_local throw true try typedef typeid typename union unsigned using virtual void volatile wchar_t while xor xor_eq"
for keyword in cppKeywordList:gmatch("%S+") do cppKeywords[keyword] = true end

local function IsIdentifier(value)
    return type(value) == "string" and value:match("^[A-Za-z_][A-Za-z0-9_]*$") ~= nil and not cppKeywords[value]
end

local function DefinitionLabel(definition)
    local location = definition.sourcePath
    if definition.sourceLine ~= nil then location = location .. ":" .. tostring(definition.sourceLine) end
    return definition.namespace .. "::" .. tostring(definition.name) .. " (" .. location .. ")"
end

local function Fail(definition, message)
    error("MetaGen C++ validation failed for " .. DefinitionLabel(definition) .. ": " .. message, 0)
end

local function GetTypeName(definition)
    local typeNameResolver = type(definition.archetype) == "table" and definition.archetype.cppTypeName or nil
    if typeNameResolver == nil then return definition.name end
    if type(typeNameResolver) ~= "function" then
        Fail(definition, "archetype cppTypeName must be a function")
    end

    local succeeded, typeName = pcall(typeNameResolver, definition)
    if not succeeded then
        Fail(definition, "failed to resolve generated type name: " .. tostring(typeName))
    end
    if type(typeName) ~= "string" or typeName == "" then
        Fail(definition, "generated type name must be a non-empty string")
    end
    return typeName
end

local function ValidateIdentifier(definition, value, description)
    if not IsIdentifier(value) then
        Fail(definition, description .. " '" .. tostring(value) .. "' is not a valid C++ identifier")
    end
end

local function QuoteString(value)
    local text = tostring(value)
    text = text:gsub("\\", "\\\\")
    text = text:gsub("\"", "\\\"")
    text = text:gsub("\0", "\\0")
    text = text:gsub("\a", "\\a")
    text = text:gsub("\b", "\\b")
    text = text:gsub("\f", "\\f")
    text = text:gsub("\n", "\\n")
    text = text:gsub("\r", "\\r")
    text = text:gsub("\t", "\\t")
    text = text:gsub("\v", "\\v")
    return "\"" .. text .. "\""
end

local function GetPathToFileFromNamespace(root, namespace)
    local parts = {}
    for part in namespace:gmatch("[^:]+") do table.insert(parts, part) end
    if #parts >= 1 then table.remove(parts, 1) end

    local leaf = parts[#parts] or "MetaGen"
    local folderPath = root
    for _, part in ipairs(parts) do folderPath = folderPath .. "/" .. part end
    return folderPath, folderPath .. "/" .. leaf .. ".h"
end

local function AddIncludes(destination, includes)
    if includes == nil then return end
    for _, category in ipairs({ "project", "system" }) do
        if includes[category] ~= nil then
            for _, include in ipairs(includes[category]) do
                if destination[category][include] == nil then
                    destination[category][include] = 1
                    destination["num" .. category:sub(1, 1):upper() .. category:sub(2) .. "Includes"] =
                        destination["num" .. category:sub(1, 1):upper() .. category:sub(2) .. "Includes"] + 1
                end
            end
        end
    end
end

function M.SelectNamespaces(model)
    local result = OrderedTable()
    for namespaceName, namespaceModel in pairs(model.namespaces) do
        for _, definition in pairs(namespaceModel.definitions) do
            if definition.targets.cpp then
                result[namespaceName] = namespaceModel
                break
            end
        end
    end
    return result
end

local function HasComponent(definition, expected)
    for _, component in ipairs(definition.components) do
        if component == expected then return true end
    end
    return false
end

function M.Validate(model, definitions, context)
    if context == nil or context.writer == nil or context.cpp == nil then
        error("MetaGen C++ backend requires a writer and C++ emitter")
    end
    if type(context.pathSort) ~= "function" or type(context.hash) ~= "function" then
        error("MetaGen C++ backend requires path ordering and hashing functions")
    end

    local generatedTypes = {}
    for _, definition in ipairs(definitions) do
        if definition.baseType ~= Type.ENUM and definition.baseType ~= Type.STRUCT then
            Fail(definition, "definition kind '" .. definition.kind .. "' is unsupported")
        end

        local namespaceParts = {}
        for part in definition.namespace:gmatch("[^:]+") do table.insert(namespaceParts, part) end
        if table.concat(namespaceParts, "::") ~= definition.namespace then
            Fail(definition, "namespace '" .. definition.namespace .. "' is not a valid C++ namespace")
        end
        for _, part in ipairs(namespaceParts) do
            ValidateIdentifier(definition, part, "namespace part")
        end

        ValidateIdentifier(definition, definition.name, "definition name")
        local typeName = GetTypeName(definition)
        ValidateIdentifier(definition, typeName, "generated type name")
        local qualifiedTypeName = definition.namespace .. "::" .. typeName
        if generatedTypes[qualifiedTypeName] ~= nil then
            Fail(definition, "generated type name '" .. qualifiedTypeName .. "' is already used by " .. DefinitionLabel(generatedTypes[qualifiedTypeName]))
        end
        generatedTypes[qualifiedTypeName] = definition

        for _, field in ipairs(definition.fields) do
            ValidateIdentifier(definition, field.name, definition.baseType == Type.ENUM and "enum field name" or "field name")
        end

        for _, component in ipairs(definition.components) do
            local shouldValidate = component.target == nil or component.target == "cpp"
            if shouldValidate and component.Validate ~= nil then
                local succeeded, validationError = pcall(component.Validate, component, definition)
                if not succeeded then
                    Fail(definition, "component validation failed: " .. tostring(validationError))
                end
            end
        end
    end
end

function M.PlanOutputs(model, definitions, context)
    local outputs = {}
    for namespaceName in pairs(M.SelectNamespaces(model)) do
        local _, filePath = GetPathToFileFromNamespace("", namespaceName)
        local relativePath = filePath:gsub("^/", "")
        table.insert(outputs, relativePath)
    end

    local hasPackets = false
    local hasEnums = false
    for _, definition in ipairs(definitions) do
        hasPackets = hasPackets or HasComponent(definition, Component.PacketMetaData)
        hasEnums = hasEnums or HasComponent(definition, Component.EnumMetaData)
    end
    if hasPackets then table.insert(outputs, "PacketList.h") end
    if hasPackets or hasEnums then table.insert(outputs, "EnumTraits.h") end

    table.sort(outputs)
    return outputs
end

function M.ResetGenerationState(context)
    local writer = context and context.writer or nil
    if writer ~= nil then
        writer.currentIndent = 0
        writer.currentFile = nil
    end

    for _, component in pairs(Component) do
        if component.ResetGenerationState ~= nil then
            component:ResetGenerationState()
        end
    end
end

function M.Emit(model, definitions, context)
    M.ResetGenerationState(context)

    local stagingRootDir = context.stagingRootDir
    local binRootDir = context.binRootDir
    local pathSort = context.pathSort
    local writer = context.writer
    local cpp = context.cpp

    for namespaceName, namespaceModel in pairs(M.SelectNamespaces(model)) do
        local folderPath, filePath = GetPathToFileFromNamespace(stagingRootDir, namespaceName)
        local _, includeFilePath = GetPathToFileFromNamespace(binRootDir, namespaceName)
        local makeDirectory = context.makeDirectory or os.mkdir
        makeDirectory(folderPath)

        Output.Write(context, filePath, function()
        local numNamespaceTypes = 0
        local namespaceTypes = OrderedTable()
        local namespaceIncludes = { project = OrderedTable(), numProjectIncludes = 0, system = OrderedTable(), numSystemIncludes = 0 }

        for _, definition in pairs(namespaceModel.definitions) do
            if definition.targets.cpp then
                local typeName = GetTypeName(definition)
                for _, component in ipairs(definition.components) do AddIncludes(namespaceIncludes, component.include) end
                if definition.baseType == Type.ENUM then
                    AddIncludes(namespaceIncludes, definition.enumType.include)
                else
                    for _, field in ipairs(definition.fields) do
                        AddIncludes(namespaceIncludes, field.type.include)
                        if field.attributes ~= nil and field.attributes.type ~= nil then AddIncludes(namespaceIncludes, field.attributes.type.include) end
                    end
                end
                numNamespaceTypes = numNamespaceTypes + 1
                namespaceTypes[typeName] =
                {
                    path = includeFilePath,
                    namespace = definition.namespace,
                    name = typeName,
                    type = definition.baseType,
                    model = definition,
                    components = definition.components,
                    numComponents = #definition.components,
                    fields = definition.fields,
                    enumType = definition.enumType,
                    aliases = definition.aliases
                }
            end
        end

        cpp:Line("#pragma once")
        cpp:BlankLine()
        local hasProjectIncludes = namespaceIncludes.numProjectIncludes > 0
        local hasSystemIncludes = namespaceIncludes.numSystemIncludes > 0
        for _, category in ipairs({ "project", "system" }) do
            local includes = {}
            for include in pairs(namespaceIncludes[category]) do table.insert(includes, include) end
            table.sort(includes, pathSort)
            for _, include in ipairs(includes) do
                if category == "project" then cpp:Line("#include \"" .. include .. "\"")
                else cpp:Line("#include <" .. include .. ">") end
            end
            if category == "project" and hasProjectIncludes and hasSystemIncludes then cpp:BlankLine() end
        end
        if hasProjectIncludes or hasSystemIncludes then cpp:BlankLine() end

        cpp:Line("namespace " .. namespaceName)
        cpp:Line("{")
        writer:AddIndent()
        local typeCounter = 1
        for typeName, generatedType in pairs(namespaceTypes) do
            for componentIndex, component in ipairs(generatedType.components) do
                if component.PreGenerate ~= nil then
                    local wrote = component:PreGenerate(generatedType, context)
                    if wrote and componentIndex < generatedType.numComponents then cpp:BlankLine() end
                end
            end

            local numFields = #generatedType.fields
            if generatedType.type == Type.ENUM then
                cpp:Line("enum class " .. typeName .. " : " .. generatedType.enumType.name)
                cpp:Line("{")
                writer:AddIndent()
                generatedType.minVal, generatedType.maxVal, generatedType.nextVal = generatedType.enumType.min, generatedType.enumType.max, 0
                for index, fieldModel in ipairs(generatedType.model.fields) do
                    local emittedValue = fieldModel.declaredValue or fieldModel.resolvedValue
                    generatedType.nextVal = fieldModel.resolvedValue + 1
                    if index < numFields then cpp:Line(fieldModel.name .. " = " .. tostring(emittedValue) .. ",")
                    else writer:Write(fieldModel.name .. " = " .. tostring(emittedValue)) end
                end
                if generatedType.numComponents > 0 then
                    cpp:BlankLine()
                    for componentIndex, component in ipairs(generatedType.components) do
                        if component.Generate ~= nil then
                            local wrote = component:Generate(generatedType, context)
                            if wrote and componentIndex < generatedType.numComponents then cpp:BlankLine() end
                        end
                    end
                end
                writer:SubIndent()
                cpp:Line("};")
            elseif generatedType.type == Type.STRUCT then
                cpp:Line("struct " .. typeName)
                cpp:Line("{")
                if numFields > 0 then cpp:Line("public:") end
                writer:AddIndent()
                generatedType.totalSize = 0
                for _, field in ipairs(generatedType.fields) do
                    local attributes = field.attributes
                    generatedType.totalSize = generatedType.totalSize + (field.type.size or 0)
                    local text = field.type.name
                    if field.type == Type.ARRAY then text = text .. "<" .. attributes.type.name .. ", " .. attributes.count .. "> " .. field.name
                    elseif field.type == Type.VECTOR then text = text .. "<" .. attributes.type.name .. "> " .. field.name
                    else text = text .. " " .. field.name end
                    if attributes ~= nil and attributes.default ~= nil then
                        text = text .. " = " .. (field.type == Type.STRING and QuoteString(attributes.default) or tostring(attributes.default))
                    end
                    cpp:Statement(text)
                end
                if generatedType.numComponents > 0 then
                    writer:SubIndent()
                    if numFields > 0 then cpp:BlankLine() end
                    cpp:Line("public:")
                    writer:AddIndent()
                    for componentIndex, component in ipairs(generatedType.components) do
                        if component.Generate ~= nil then
                            local wrote = component:Generate(generatedType, context)
                            if wrote and componentIndex < generatedType.numComponents then cpp:BlankLine() end
                        end
                    end
                end
                writer:SubIndent()
                cpp:Line("};")
            end

            for componentIndex, component in ipairs(generatedType.components) do
                if component.PostGenerate ~= nil then
                    local wrote = component:PostGenerate(generatedType, context)
                    if wrote and componentIndex < generatedType.numComponents then cpp:BlankLine() end
                end
            end
            if typeCounter < numNamespaceTypes then cpp:BlankLine() end
            typeCounter = typeCounter + 1
        end
        writer:SubIndent()
        writer:Write("}")
        end)
    end

    for _, component in pairs(Component) do
        if component.OnGeneratorComplete ~= nil then
            component:OnGeneratorComplete(context.sourceRootDir, stagingRootDir, binRootDir, context)
        end
    end

    M.ResetGenerationState(context)
end

return M

local Type = require("Type")
local Definition = require("Definition")

local M = {}
local typeDescriptors = setmetatable({}, { __mode = "k" })
for _, value in pairs(Type) do
    if type(value) == "table" then typeDescriptors[value] = true end
end

local function IsTypeDescriptor(value)
    return typeDescriptors[value] == true
end

local function DefinitionLabel(definition)
    local sourceLocation = definition.sourcePath
    if definition.sourceLine ~= nil then
        sourceLocation = sourceLocation .. ":" .. tostring(definition.sourceLine)
    end

    return definition.namespace .. "::" .. tostring(definition.name) .. " (" .. sourceLocation .. ")"
end

local function Fail(definition, message)
    error("MetaGen validation failed for " .. DefinitionLabel(definition) .. ": " .. message, 0)
end

local function ValidateName(definition, value, description)
    if type(value) ~= "string" or value == "" then
        Fail(definition, description .. " must be a non-empty string")
    end
end

local function ValidateArray(definition, value, description)
    if type(value) ~= "table" then
        Fail(definition, description .. " must be an array")
    end

    local count = 0
    local maxIndex = 0
    for key, _ in pairs(value) do
        if type(key) ~= "number" or key < 1 or key % 1 ~= 0 then
            Fail(definition, description .. " must only contain positive integer keys")
        end
        count = count + 1
        maxIndex = math.max(maxIndex, key)
    end

    if count ~= maxIndex then
        Fail(definition, description .. " must not contain holes")
    end
end

local function CopyValue(value, seen)
    if type(value) ~= "table" then return value end

    -- Type descriptors are shared immutable values. PostgreSQL type descriptors
    -- use the same convention; authoring option tables are copied below them.
    if IsTypeDescriptor(value) or value.__metagenPostgresType == true then return value end

    seen = seen or {}
    if seen[value] ~= nil then return seen[value] end

    local result = {}
    seen[value] = result
    for key, item in pairs(value) do
        result[CopyValue(key, seen)] = CopyValue(item, seen)
    end
    return result
end

local function CopyTable(value)
    if value == nil then return nil end
    return CopyValue(value)
end

local function CopyDefinitionOptions(raw)
    local result = {}
    for key, value in pairs(raw) do
        if key ~= "archetype" and key ~= "fields" and key ~= "components" and
            key ~= "targets" and key ~= "kind" and key ~= "type" and key ~= "aliases" and
            key ~= "persistentId" then
            result[key] = CopyValue(value)
        end
    end
    return result
end

local function ResolveBaseType(definition)
    local archetype = definition.raw.archetype
    if archetype == nil then
        Fail(definition, "'archetype' must be set")
    end

    if type(archetype) == "table" then
        definition.baseType = archetype.base
        definition.isBaseType = false
    elseif type(archetype) == "number" then
        definition.baseType = archetype
        definition.isBaseType = true
    end

    if definition.baseType ~= Type.ENUM and definition.baseType ~= Type.STRUCT then
        Fail(definition, "'archetype' or 'archetype.base' must be Type.ENUM or Type.STRUCT")
    end

    definition.archetype = archetype
    definition.kind = definition.raw.kind or (type(archetype) == "table" and archetype.kind) or
        (definition.baseType == Type.ENUM and "enum" or "struct")
    if type(definition.kind) ~= "string" or definition.kind == "" then
        Fail(definition, "definition kind must be a non-empty string")
    end

    local configuredTargets = definition.raw.targets or (type(archetype) == "table" and archetype.targets) or { cpp = true }
    if type(configuredTargets) ~= "table" then
        Fail(definition, "'targets' must be a table of backend names to booleans")
    end

    definition.targets = {}
    local numTargets = 0
    for targetName, enabled in pairs(configuredTargets) do
        if type(targetName) ~= "string" or type(enabled) ~= "boolean" then
            Fail(definition, "'targets' entries must map backend names to booleans")
        end
        definition.targets[targetName] = enabled
        if enabled then
            numTargets = numTargets + 1
        end
    end
    if numTargets == 0 then
        Fail(definition, "definition must enable at least one target")
    end
end

local function ComposeComponents(definition)
    local components = {}
    local componentsSeen = {}

    local function AddComponents(source)
        if source == nil then return end

        ValidateArray(definition, source, "'components'")

        for _, component in ipairs(source) do
            if type(component) ~= "table" or type(component.flags) ~= "table" then
                Fail(definition, "component entries must provide a flags table")
            end

            local supported = (definition.baseType == Type.ENUM and component.flags.supportsEnum) or
                (definition.baseType == Type.STRUCT and component.flags.supportsStruct)
            if not supported then
                Fail(definition, "component does not support this definition kind")
            end

            if componentsSeen[component] == nil then
                componentsSeen[component] = true
                table.insert(components, component)
            end
        end
    end

    AddComponents(definition.raw.components)
    if not definition.isBaseType then
        AddComponents(definition.archetype.components)
    end

    definition.components = components
end


local function ValidateFieldAttributes(definition, field, attributes)
    if attributes ~= nil and type(attributes) ~= "table" then
        Fail(definition, "attributes for field '" .. field.name .. "' must be a table")
    end

    if attributes ~= nil and attributes.serialize ~= nil and type(attributes.serialize) ~= "boolean" then
        Fail(definition, "field '" .. field.name .. "' attribute 'serialize' must be boolean")
    end
    if attributes ~= nil and attributes.luaPush ~= nil and type(attributes.luaPush) ~= "boolean" then
        Fail(definition, "field '" .. field.name .. "' attribute 'luaPush' must be boolean")
    end
    if attributes ~= nil and attributes.debug ~= nil and type(attributes.debug) ~= "boolean" then
        Fail(definition, "field '" .. field.name .. "' attribute 'debug' must be boolean")
    end

    if field.type == Type.ARRAY or field.type == Type.VECTOR then
        if type(attributes) ~= "table" or type(attributes.type) ~= "table" or type(attributes.type.name) ~= "string" then
            Fail(definition, "container field '" .. field.name .. "' requires a valid element type")
        end
    end

    if field.type == Type.ARRAY then
        if type(attributes.count) ~= "number" or attributes.count < 1 or attributes.count % 1 ~= 0 then
            Fail(definition, "array field '" .. field.name .. "' requires a positive integer count")
        end
    end
end

local function ValidateStruct(definition)
    local fieldsSeen = {}

    for index, fieldTable in ipairs(definition.raw.fields) do
        if type(fieldTable) ~= "table" or type(fieldTable[1]) ~= "table" then
            Fail(definition, "field #" .. tostring(index) .. " must be created with Field")
        end

        local field = fieldTable[1]
        local attributes = fieldTable[2]
        ValidateName(definition, field.name, "field name")

        if fieldsSeen[field.name] then
            Fail(definition, "duplicate field '" .. field.name .. "'")
        end
        fieldsSeen[field.name] = true

        if type(field.type) ~= "table" or type(field.type.name) ~= "string" then
            Fail(definition, "field '" .. field.name .. "' requires a valid type")
        end

        ValidateFieldAttributes(definition, field, attributes)
        table.insert(definition.fields,
        {
            index = index,
            name = field.name,
            type = field.type,
            attributes = CopyTable(attributes)
        })
    end
end

local function ValidateEnum(definition)
    local enumType = definition.raw.type
    if type(enumType) ~= "table" or enumType.kind ~= "integer" or type(enumType.Validate) ~= "function" then
        Fail(definition, "enum 'type' must be an integer MetaGen type")
    end

    definition.enumType = enumType

    local fieldsSeen = {}
    local valuesByName = {}
    local nextValue = 0
    local valueSeen = false

    for index, fieldTable in ipairs(definition.raw.fields) do
        if type(fieldTable) ~= "table" or type(fieldTable[1]) ~= "table" then
            Fail(definition, "enum field #" .. tostring(index) .. " must be created with Field")
        end

        local field = fieldTable[1]
        ValidateName(definition, field.name, "enum field name")
        if fieldsSeen[field.name] then
            Fail(definition, "duplicate enum field '" .. field.name .. "'")
        end
        fieldsSeen[field.name] = true

        local declaredValue = field.type
        local resolvedValue = declaredValue
        if declaredValue == nil then
            resolvedValue = nextValue
        elseif type(declaredValue) == "string" then
            resolvedValue = valuesByName[declaredValue]
            if resolvedValue == nil then
                Fail(definition, "enum field '" .. field.name .. "' references unknown or later field '" .. declaredValue .. "'")
            end
        elseif type(declaredValue) ~= "number" then
            Fail(definition, "enum field '" .. field.name .. "' value must be a number or earlier field name")
        end

        if type(declaredValue) ~= "string" then
            if valueSeen and resolvedValue < nextValue - 1 then
                Fail(definition, "enum field '" .. field.name .. "' value " .. tostring(resolvedValue) .. " is below the next expected value " .. tostring(nextValue))
            end
            valueSeen = true
        end

        local validated, validationError = pcall(enumType.Validate, enumType, resolvedValue)
        if not validated then
            Fail(definition, "enum field '" .. field.name .. "' is invalid: " .. tostring(validationError))
        end

        valuesByName[field.name] = resolvedValue
        if type(declaredValue) ~= "string" then
            nextValue = resolvedValue + 1
        end
        table.insert(definition.fields,
        {
            index = index,
            name = field.name,
            declaredValue = declaredValue,
            resolvedValue = resolvedValue
        })
    end
end

local function ValidateAliases(definition, aliasesSeen)
    local aliases = definition.raw.aliases
    definition.aliases = {}
    if aliases == nil then return end
    ValidateArray(definition, aliases, "'aliases'")

    local localAliases = {}
    for index, alias in ipairs(aliases) do
        if type(alias) ~= "string" or alias == "" then
            Fail(definition, "alias #" .. tostring(index) .. " must be a non-empty string")
        end

        local normalizedAlias = alias:lower()
        if localAliases[normalizedAlias] then
            Fail(definition, "duplicate alias '" .. alias .. "'")
        end
        localAliases[normalizedAlias] = true
        table.insert(definition.aliases, alias)

        local previousDefinition = aliasesSeen[normalizedAlias]
        if previousDefinition ~= nil then
            Fail(definition, "alias '" .. alias .. "' is already used by " .. DefinitionLabel(previousDefinition))
        end
        aliasesSeen[normalizedAlias] = definition
    end
end

local function ValidatePersistentIds(definition, persistentIdsSeen)
    if definition.kind ~= "databaseTable" then return end

    definition.persistentId = definition.raw.persistentId

    local function RegisterPersistentId(value, description)
        if type(value) ~= "string" or value == "" or value:match("^[A-Za-z0-9_.%-]+$") == nil then
            Fail(definition, description .. " must be a non-empty persistent identifier containing only letters, digits, '.', '_', or '-'")
        end

        local previousDefinition = persistentIdsSeen[value]
        if previousDefinition ~= nil then
            Fail(definition, description .. " '" .. value .. "' is already used by " .. DefinitionLabel(previousDefinition))
        end
        persistentIdsSeen[value] = definition
    end

    RegisterPersistentId(definition.persistentId, "table persistentId")
    for _, field in ipairs(definition.fields) do
        local attributes = field.attributes
        if attributes == nil then
            Fail(definition, "database field '" .. field.name .. "' requires attributes containing a persistentId")
        end
        RegisterPersistentId(attributes.persistentId, "field '" .. field.name .. "' persistentId")
    end
end

local function ValidateDefinition(definition, aliasesSeen, persistentIdsSeen)
    ValidateName(definition, definition.name, "definition name")
    ResolveBaseType(definition)

    ValidateArray(definition, definition.raw.fields, "'fields'")

    definition.fields = {}
    if definition.baseType == Type.ENUM then
        ValidateEnum(definition)
    else
        ValidateStruct(definition)
    end

    ComposeComponents(definition)
    ValidateAliases(definition, aliasesSeen)
    ValidatePersistentIds(definition, persistentIdsSeen)

    definition.options = CopyDefinitionOptions(definition.raw)
    definition.raw = nil
end

local function StableKey(value)
    if type(value) == "string" then return "string:" .. value end
    if type(value) == "number" then return "number:" .. tostring(value) end
    return type(value) .. ":" .. tostring(value)
end

local function StableValue(value, visiting)
    if type(value) ~= "table" then
        if type(value) == "function" then return "function" end
        return type(value) .. ":" .. tostring(value)
    end

    if IsTypeDescriptor(value) then
        return "type:" .. tostring(value.name or value.kind or "descriptor")
    end
    if value.__metagenPostgresType == true then
        return "postgresType:" .. tostring(value.name or value.sql or "descriptor")
    end

    visiting = visiting or {}
    if visiting[value] then return "<cycle>" end
    visiting[value] = true

    local keys = {}
    for key in pairs(value) do table.insert(keys, key) end
    table.sort(keys, function(a, b) return StableKey(a) < StableKey(b) end)

    local parts = { "table{" }
    for _, key in ipairs(keys) do
        table.insert(parts, StableValue(key, visiting))
        table.insert(parts, "=")
        table.insert(parts, StableValue(value[key], visiting))
        table.insert(parts, ";")
    end
    table.insert(parts, "}")
    visiting[value] = nil
    return table.concat(parts)
end

function M.Fingerprint(model)
    local parts = {}
    for _, definition in ipairs(model.definitions) do
        table.insert(parts, "definition=" .. definition.namespace .. "::" .. definition.name)
        table.insert(parts, "kind=" .. definition.kind)
        table.insert(parts, "base=" .. tostring(definition.baseType))
        table.insert(parts, "persistentId=" .. tostring(definition.persistentId))
        table.insert(parts, "options=" .. StableValue(definition.options))

        local targetNames = {}
        for name in pairs(definition.targets) do table.insert(targetNames, name) end
        table.sort(targetNames)
        for _, name in ipairs(targetNames) do table.insert(parts, "target=" .. name .. ":" .. tostring(definition.targets[name])) end

        for _, alias in ipairs(definition.aliases) do table.insert(parts, "alias=" .. alias) end
        for _, component in ipairs(definition.components) do table.insert(parts, "component=" .. tostring(component)) end
        for _, field in ipairs(definition.fields) do
            table.insert(parts, "field=" .. field.name)
            table.insert(parts, "fieldType=" .. StableValue(field.type))
            table.insert(parts, "declared=" .. StableValue(field.declaredValue))
            table.insert(parts, "resolved=" .. StableValue(field.resolvedValue))
            if field.attributes ~= nil then
                local attributeNames = {}
                for name in pairs(field.attributes) do table.insert(attributeNames, name) end
                table.sort(attributeNames)
                for _, name in ipairs(attributeNames) do
                    table.insert(parts, "attribute=" .. name .. ":" .. StableValue(field.attributes[name]))
                end
            end
        end
    end
    return table.concat(parts, "\n")
end

function M.AssertUnchanged(model, phase)
    if M.Fingerprint(model) ~= model.semanticFingerprint then
        error("MetaGen backend mutated the normalized model during " .. phase, 0)
    end
end

function M.BuildFromSources(sources)
    local model =
    {
        definitions = {},
        definitionsByKind = {},
        definitionsByTarget = {},
        namespaces = OrderedTable()
    }
    local aliasesSeen = {}
    local persistentIdsSeen = {}

    for _, source in ipairs(sources) do
        if type(source.defines) ~= "table" then
            error("MetaGen validation failed for " .. tostring(source.path) .. ": definition file must return a table", 0)
        end
        if not Definition.IsDefinitionSet(source.defines) then
            error("MetaGen validation failed for " .. tostring(source.path) .. ": definition file must return D.Definitions created with the Definition constructors", 0)
        end
        if type(source.namespace) ~= "string" or source.namespace == "" then
            error("MetaGen validation failed for " .. tostring(source.path) .. ": namespace must be a non-empty string", 0)
        end
        local namespaceModel = model.namespaces[source.namespace]
        if namespaceModel == nil then
            namespaceModel =
            {
                name = source.namespace,
                definitions = OrderedTable()
            }
            model.namespaces[source.namespace] = namespaceModel
        end

        for name, rawDefinition in pairs(source.defines) do
            if namespaceModel.definitions[name] ~= nil then
                error("MetaGen validation failed: duplicate definition '" .. tostring(name) .. "' in namespace '" .. source.namespace .. "'", 0)
            end
            if type(rawDefinition) ~= "table" then
                error("MetaGen validation failed for " .. source.namespace .. "::" .. tostring(name) .. ": definition must be a table", 0)
            end

            local definition =
            {
                name = name,
                namespace = source.namespace,
                sourcePath = source.path,
                sourceLine = source.definitionLines and source.definitionLines[name] or nil,
                raw = rawDefinition
            }

            ValidateDefinition(definition, aliasesSeen, persistentIdsSeen)
            namespaceModel.definitions[name] = definition
            table.insert(model.definitions, definition)

            model.definitionsByKind[definition.kind] = model.definitionsByKind[definition.kind] or {}
            table.insert(model.definitionsByKind[definition.kind], definition)

            for targetName, enabled in pairs(definition.targets) do
                if enabled then
                    model.definitionsByTarget[targetName] = model.definitionsByTarget[targetName] or {}
                    table.insert(model.definitionsByTarget[targetName], definition)
                end
            end
        end
    end

    model.semanticFingerprint = M.Fingerprint(model)
    return model
end

local function LoadSources(files, sourceRootDir, namespaceBase, sources)
    for _, path in ipairs(files) do
        local chunk, loadError = loadfile(path)
        if chunk == nil then
            error("MetaGen failed to load definition file '" .. path .. "': " .. tostring(loadError), 0)
        end

        local succeeded, defines = pcall(chunk)
        if not succeeded then
            error("MetaGen failed to execute definition file '" .. path .. "': " .. tostring(defines), 0)
        end
        if defines ~= nil then
            local definitionLines = {}
            local sourceFile = io.open(path, "r")
            if sourceFile ~= nil then
                local readSucceeded, readError = xpcall(function()
                    local lineNumber = 0
                    for line in sourceFile:lines() do
                        lineNumber = lineNumber + 1
                        local definitionName = line:match("^%s*[A-Za-z_][A-Za-z0-9_]*%.([A-Za-z_][A-Za-z0-9_]*)%s*=")
                        if definitionName == nil then
                            definitionName = line:match("^%s*[A-Za-z_][A-Za-z0-9_]*%.[A-Za-z_][A-Za-z0-9_]*%(%s*\"([A-Za-z_][A-Za-z0-9_]*)\"")
                        end
                        if definitionName ~= nil then
                            definitionLines[definitionName] = lineNumber
                        end
                    end
                end, debug.traceback)
                local closed, closeError = pcall(function() sourceFile:close() end)
                if not readSucceeded then error("MetaGen failed to read definition file '" .. path .. "': " .. tostring(readError), 0) end
                if not closed then error("MetaGen failed to close definition file '" .. path .. "': " .. tostring(closeError), 0) end
            end

            local relativePath = path:sub(#sourceRootDir + 2)
            local directory = relativePath:match("(.*/)")
            local namespace = namespaceBase
            if directory then
                namespace = namespace .. "." .. directory:gsub("/", "."):sub(1, -2)
            end

            table.insert(sources,
            {
                path = path,
                namespace = namespace:gsub("%.", "::"),
                definitionLines = definitionLines,
                defines = defines
            })
        end
    end

end

function M.LoadSourceSets(sourceSets)
    local sources = {}
    for _, sourceSet in ipairs(sourceSets) do
        LoadSources(sourceSet.files, sourceSet.root, sourceSet.namespace, sources)
    end
    return M.BuildFromSources(sources)
end

function M.LoadFiles(files, sourceRootDir, namespaceBase)
    return M.LoadSourceSets({ { files = files, root = sourceRootDir, namespace = namespaceBase } })
end

return M

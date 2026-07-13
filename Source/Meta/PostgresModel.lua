local Type = require("Type")
local Postgres = require("Postgres")

local M = {}

local bundleOrder = { "auth", "character", "world", "shared" }
local bundleRanks = {}
for index, bundle in ipairs(bundleOrder) do bundleRanks[bundle] = index end

local cppKeywords = {}
for keyword in ([[
    alignas alignof and and_eq asm atomic_cancel atomic_commit atomic_noexcept auto bitand bitor bool break case catch
    char char8_t char16_t char32_t class compl concept const consteval constexpr constinit const_cast continue co_await
    co_return co_yield decltype default delete do double dynamic_cast else enum explicit export extern false float for
    friend goto if inline int long mutable namespace new noexcept not not_eq nullptr operator or or_eq private protected
    public reflexpr register reinterpret_cast requires return short signed sizeof static static_assert static_cast struct
    switch synchronized template this thread_local throw true try typedef typeid typename union unsigned using virtual
    void volatile wchar_t while xor xor_eq
]]):gmatch("%S+") do
    cppKeywords[keyword] = true
end

local foreignKeyActions =
{
    noaction = "noAction",
    restrict = "restrict",
    cascade = "cascade",
    setnull = "setNull",
    setdefault = "setDefault"
}

local schemaObjectKeys =
{
    primaryKey = { kind = true, name = true, fields = true, persistentId = true },
    unique = { kind = true, name = true, fields = true, persistentId = true },
    foreignKey =
    {
        kind = true, name = true, fields = true, persistentId = true,
        referencedTable = true, referencedFields = true, referencedDatabase = true,
        onUpdate = true, onDelete = true
    },
    check = { kind = true, name = true, fields = true, persistentId = true, operator = true, value = true },
    rawCheck = { kind = true, name = true, fields = true, persistentId = true, rawSql = true },
    index = { kind = true, name = true, fields = true, persistentId = true, unique = true },
    query = { kind = true, name = true, fields = true, persistentId = true, cardinality = true, orderBy = true },
    update = { kind = true, name = true, fields = true, persistentId = true, updateFields = true, cardinality = true },
    delete = { kind = true, name = true, fields = true, persistentId = true, cardinality = true },
    upsert = { kind = true, name = true, fields = true, persistentId = true, insertFields = true, updateFields = true },
    ["function"] = { kind = true, name = true, fields = true, persistentId = true, functionName = true }
}

local tableOptionKeys =
{
    database = true,
    schema = true,
    table = true,
    primaryKey = true,
    constraints = true,
    indexes = true,
    queries = true,
    operations = true,
    orderBy = true
}

local function DefinitionLabel(definition)
    local sourceLocation = definition.sourcePath
    if definition.sourceLine ~= nil then sourceLocation = sourceLocation .. ":" .. tostring(definition.sourceLine) end
    return definition.namespace .. "::" .. tostring(definition.name) .. " (" .. sourceLocation .. ")"
end

local function Fail(definition, message)
    error("MetaGen PostgreSQL validation failed for " .. DefinitionLabel(definition) .. ": " .. message, 0)
end

local function FailField(definition, field, message)
    Fail(definition, "field '" .. tostring(field.name) .. "': " .. message)
end

local function ValidateArray(definition, value, description)
    if type(value) ~= "table" then Fail(definition, description .. " must be an array") end

    local count = 0
    local maximum = 0
    for key in pairs(value) do
        if type(key) ~= "number" or key < 1 or key % 1 ~= 0 then
            Fail(definition, description .. " must only contain positive integer keys")
        end
        count = count + 1
        maximum = math.max(maximum, key)
    end
    if count ~= maximum then Fail(definition, description .. " must not contain holes") end
end

local function ValidateIdentifier(definition, value, description)
    if type(value) ~= "string" or value == "" then
        Fail(definition, description .. " must be a non-empty PostgreSQL identifier")
    end
    if #value > 63 then
        Fail(definition, description .. " '" .. value .. "' exceeds PostgreSQL's 63-byte identifier limit")
    end
    if value:match("^[A-Za-z_][A-Za-z0-9_]*$") == nil then
        Fail(definition, description .. " '" .. value .. "' is not a valid PostgreSQL identifier")
    end
end

local function ValidateLogicalName(definition, value, description)
    if type(value) ~= "string" or value == "" then
        Fail(definition, description .. " must be a non-empty name")
    end
    if #value > 63 or value:match("^[A-Za-z_][A-Za-z0-9_]*$") == nil then
        Fail(definition, description .. " '" .. tostring(value) .. "' is not a valid generated name")
    end
    if cppKeywords[value] then Fail(definition, description .. " '" .. value .. "' is a reserved C++ keyword") end
end

local function RegisterPersistentId(definition, seen, value, description)
    if type(value) ~= "string" or value == "" or value:match("^[A-Za-z0-9_.%-]+$") == nil then
        Fail(definition, description .. " must be a non-empty persistent identifier containing only letters, digits, '.', '_', or '-'")
    end

    if seen[value] ~= nil then
        Fail(definition, description .. " '" .. value .. "' is already used by " .. seen[value])
    end
    seen[value] = description .. " in " .. DefinitionLabel(definition)
end

local function ValidateSchemaObject(definition, object, expectedKind, persistentIds)
    if type(object) ~= "table" or object.kind ~= expectedKind then
        Fail(definition, expectedKind .. " entries must be created with the typed PostgreSQL constructor")
    end
    if expectedKind == "query" or expectedKind == "update" or expectedKind == "delete" then
        ValidateLogicalName(definition, object.name, expectedKind .. " name")
    else
        ValidateIdentifier(definition, object.name, expectedKind .. " name")
    end
    for key in pairs(object) do
        if not schemaObjectKeys[expectedKind][key] then
            Fail(definition, expectedKind .. " '" .. object.name .. "' uses unsupported option '" .. tostring(key) .. "'")
        end
    end
    RegisterPersistentId(definition, persistentIds, object.persistentId, expectedKind .. " persistentId")
    if expectedKind == "rawCheck" then
        if type(object.rawSql) ~= "string" or object.rawSql == "" then
            Fail(definition, "rawCheck '" .. tostring(object.name) .. "' must provide a non-empty SQL expression")
        end
    elseif object.rawSql ~= nil or object.sql ~= nil or object.expression ~= nil then
        Fail(definition, expectedKind .. " '" .. object.name .. "' cannot contain raw SQL")
    end
end

local function GetList(definition, options, key, description)
    local value = options[key]
    if value == nil then return {} end
    ValidateArray(definition, value, description)
    return value
end

local function IsFiniteNumber(value)
    return type(value) == "number" and value == value and value ~= math.huge and value ~= -math.huge
end

local function ValidateValue(definition, field, value, description)
    local postgresType = field.postgresType
    local semanticType = field.semanticType
    local prefix = description or "value"

    if postgresType.category == "integer" then
        if type(value) ~= "number" or value % 1 ~= 0 then
            FailField(definition, field, prefix .. " must be an integer")
        end
        if value < postgresType.min or value > postgresType.max then
            FailField(definition, field, prefix .. " is outside the PostgreSQL " .. postgresType.name .. " range")
        end
        if semanticType.kind == "integer" and (value < semanticType.min or value > semanticType.max) then
            FailField(definition, field, prefix .. " is outside the C++ field type range")
        end
    elseif postgresType.category == "boolean" then
        if type(value) ~= "boolean" then FailField(definition, field, prefix .. " must be a boolean") end
    elseif postgresType.category == "float" then
        if not IsFiniteNumber(value) then FailField(definition, field, prefix .. " must be a finite number") end
    elseif postgresType.category == "string" then
        if type(value) ~= "string" then FailField(definition, field, prefix .. " must be a string") end
        if value:find("\0", 1, true) ~= nil then FailField(definition, field, prefix .. " must not contain NUL") end
        local length = utf8.len(value)
        if length == nil then FailField(definition, field, prefix .. " must contain valid UTF-8") end
        if postgresType.length ~= nil then
            if length > postgresType.length then
                FailField(definition, field, prefix .. " exceeds varchar(" .. tostring(postgresType.length) .. ")")
            end
        end
    else
        FailField(definition, field, prefix .. " is not supported for PostgreSQL type '" .. postgresType.name .. "'")
    end
end

local function ResolveStorageType(definition, field, postgresType)
    if not Postgres.IsType(postgresType) then
        FailField(definition, field, "postgres must contain a typed PostgreSQL column descriptor")
    end

    local semanticType = field.type
    if postgresType.category == "integer" and semanticType.kind == "integer" then
        if semanticType.min >= postgresType.min and semanticType.max <= postgresType.max then
            return nil
        end

        if semanticType.min == 0 and semanticType.bits == postgresType.bits then
            return
            {
                min = 0,
                max = postgresType.max,
                reason = "negative values and unsigned values above the PostgreSQL signed maximum are not representable"
            }
        end

        FailField(definition, field, "C++ type '" .. semanticType.name .. "' does not fit PostgreSQL type '" .. postgresType.name .. "'")
    elseif postgresType.category == "boolean" and semanticType == Type.BOOL then
        return nil
    elseif postgresType.category == "float" and semanticType.kind == "float" then
        if semanticType.bits == postgresType.bits then return nil end
        FailField(definition, field, "C++ type '" .. semanticType.name .. "' must match PostgreSQL type '" .. postgresType.name .. "' exactly")
    elseif postgresType.category == "string" and semanticType == Type.STRING then
        return nil
    elseif postgresType.category == "binary" and semanticType == Type.BYTEBUFFER then
        return nil
    end

    FailField(definition, field, "C++ type '" .. tostring(semanticType.name) .. "' is incompatible with PostgreSQL type '" .. postgresType.name .. "'")
end

local function ResolveFieldReferences(definition, tableModel, fields, description, allowIndexOrder, allowParameters, allowDuplicates)
    ValidateArray(definition, fields, description)
    local resolved = {}
    local seen = {}

    for index, reference in ipairs(fields) do
        local fieldName = reference
        local direction = "asc"
        local parameterType = nil

        if type(reference) == "table" and reference.kind == "indexColumn" then
            if not allowIndexOrder then Fail(definition, description .. " does not allow index ordering descriptors") end
            fieldName = reference.field
            direction = reference.direction
        elseif type(reference) == "table" and reference.kind == "queryParameter" then
            if not allowParameters then Fail(definition, description .. " does not allow query parameter descriptors") end
            fieldName = reference.field
            parameterType = reference.type
        elseif type(reference) ~= "string" then
            Fail(definition, description .. " entry #" .. tostring(index) .. " must name a table field")
        end

        if direction ~= "asc" and direction ~= "desc" then
            Fail(definition, description .. " entry #" .. tostring(index) .. " has an invalid ordering direction")
        end

        if type(fieldName) ~= "string" or fieldName == "" then
            Fail(definition, description .. " entry #" .. tostring(index) .. " has an invalid field name")
        end
        if seen[fieldName] and not allowDuplicates then Fail(definition, description .. " contains duplicate field '" .. fieldName .. "'") end
        seen[fieldName] = true

        local field = tableModel.fieldByName[fieldName]
        if field == nil then Fail(definition, description .. " references unknown field '" .. fieldName .. "'") end
        if parameterType ~= nil and parameterType ~= field.semanticType then
            Fail(definition, description .. " parameter '" .. fieldName .. "' declares a type different from the table field")
        end

        table.insert(resolved,
        {
            field = field,
            name = field.name,
            direction = direction,
            parameterType = parameterType or field.semanticType
        })
    end
    return resolved
end

local function FieldSetSignature(fields)
    local names = {}
    for _, field in ipairs(fields) do table.insert(names, field.name) end
    table.sort(names)
    return table.concat(names, "\0")
end

local function FieldListSignature(fields)
    local values = {}
    for _, field in ipairs(fields) do
        table.insert(values, field.name .. ":" .. tostring(field.direction or "asc"))
    end
    return table.concat(values, ",")
end

local function HasUniqueKey(tableModel, fields)
    local signature = FieldSetSignature(fields)
    for _, uniqueKey in ipairs(tableModel.uniqueKeys) do
        if uniqueKey.signature == signature and uniqueKey.nonNull then return true end
    end
    return false
end

local function HasAnyUniqueKey(tableModel, fields)
    local signature = FieldSetSignature(fields)
    for _, uniqueKey in ipairs(tableModel.uniqueKeys) do
        if uniqueKey.signature == signature then return true end
    end
    return false
end

local function NormalizeForeignKeyAction(definition, object, key)
    local value = object[key]
    if value == nil then return "noAction" end
    if type(value) ~= "string" then Fail(definition, "foreign key " .. key .. " must be a typed action name") end
    local normalized = value:lower():gsub("[%s_]", "")
    if foreignKeyActions[normalized] == nil then
        Fail(definition, "foreign key " .. key .. " uses unsupported action '" .. value .. "'")
    end
    return foreignKeyActions[normalized]
end

local function ResolveReferencedTable(definition, reference, tablesByDefinition, tablesByPersistentId, tablesByName)
    if type(reference) ~= "string" or reference == "" then
        Fail(definition, "foreign key referencedTable must be a persistent ID or definition name")
    end

    local tableModel = tablesByPersistentId[reference] or tablesByDefinition[reference]
    if tableModel == nil then
        local candidates = tablesByName[reference]
        if candidates ~= nil and #candidates == 1 then tableModel = candidates[1] end
        if candidates ~= nil and #candidates > 1 then
            Fail(definition, "foreign key referencedTable '" .. reference .. "' is ambiguous")
        end
    end
    if tableModel == nil then Fail(definition, "foreign key references unknown table '" .. reference .. "'") end
    return tableModel
end

local function CompareTables(a, b)
    if bundleRanks[a.bundle] ~= bundleRanks[b.bundle] then return bundleRanks[a.bundle] < bundleRanks[b.bundle] end
    if a.schema ~= b.schema then return a.schema < b.schema end
    if a.tableName ~= b.tableName then return a.tableName < b.tableName end
    return a.definitionName < b.definitionName
end

local function CompareTableNames(a, b)
    return a.definitionName < b.definitionName
end

local function EncodeCanonicalValue(value)
    if value == nil then return "nil" end
    if type(value) == "boolean" then return value and "bool:1" or "bool:0" end
    if type(value) == "number" then return "number:" .. tostring(value) end
    if type(value) == "string" then return "string:" .. tostring(#value) .. ":" .. value end
    if type(value) ~= "table" then error("Unsupported canonical value type '" .. type(value) .. "'", 0) end

    local parts = { "array:", tostring(#value), ":" }
    for _, item in ipairs(value) do
        local encoded = EncodeCanonicalValue(item)
        table.insert(parts, tostring(#encoded) .. ":" .. encoded)
    end
    return table.concat(parts)
end

local function AddCanonical(parts, kind, ...)
    local line = {}
    local function AddValue(value)
        local encoded = EncodeCanonicalValue(value)
        line[#line + 1] = tostring(#encoded) .. ":" .. encoded
    end
    AddValue(kind)
    for index = 1, select("#", ...) do AddValue(select(index, ...)) end
    parts[#parts + 1] = table.concat(line)
end

local function BuildCanonical(model)
    local parts = {}
    AddCanonical(parts, "format", "metagen-postgres-v1")
    for _, tableModel in ipairs(model.tables) do
        AddCanonical(parts, "table", tableModel.bundle, tableModel.schema, tableModel.tableName, tableModel.persistentId)
        for _, field in ipairs(tableModel.fields) do
            AddCanonical(parts, "field", field.persistentId, field.name, field.columnName, field.postgresType.name,
                field.nullable, field.identity, field.hasDefault, field.defaultValue)
        end
        if tableModel.primaryKey ~= nil then
            AddCanonical(parts, "primaryKey", tableModel.primaryKey.persistentId, tableModel.primaryKey.name,
                FieldListSignature(tableModel.primaryKey.fields))
        end
        for _, constraint in ipairs(tableModel.constraints) do
            AddCanonical(parts, "constraint", constraint.kind, constraint.persistentId, constraint.name)
            if constraint.fields ~= nil then AddCanonical(parts, "constraintFields", FieldListSignature(constraint.fields)) end
            if constraint.target ~= nil then
                AddCanonical(parts, "constraintTarget", constraint.target.persistentId,
                    FieldListSignature(constraint.targetFields), constraint.onUpdate, constraint.onDelete)
            elseif constraint.operator ~= nil then
                AddCanonical(parts, "constraintExpression", constraint.operator, constraint.value)
            elseif constraint.rawSql ~= nil then
                AddCanonical(parts, "constraintRaw", constraint.rawSql)
            end
        end
        for _, index in ipairs(tableModel.indexes) do
            AddCanonical(parts, "index", index.persistentId, index.name, index.unique, FieldListSignature(index.fields))
        end
        for _, query in ipairs(tableModel.queries) do
            AddCanonical(parts, "query", query.persistentId, query.name, query.cardinality, FieldListSignature(query.fields),
                FieldListSignature(query.orderBy))
            for _, field in ipairs(query.fields) do AddCanonical(parts, "queryType", field.name, field.parameterType.name) end
        end
        for _, operation in ipairs(tableModel.operations) do
            AddCanonical(parts, "operation", operation.kind, operation.persistentId, operation.name,
                FieldListSignature(operation.fields))
            if operation.updateFields ~= nil then AddCanonical(parts, "operationFields", FieldListSignature(operation.updateFields)) end
            if operation.insertFields ~= nil then AddCanonical(parts, "operationInsertFields", FieldListSignature(operation.insertFields)) end
            if operation.functionName ~= nil then AddCanonical(parts, "operationFunction", operation.functionName) end
        end
    end
    return table.concat(parts, "\n")
end

local function VisitTable(tableModel, state, ordered)
    if state[tableModel] == 1 then
        Fail(tableModel.definition, "foreign key dependency cycle includes table '" .. tableModel.definitionName .. "'")
    end
    if state[tableModel] == 2 then return end

    state[tableModel] = 1
    local dependencies = {}
    for dependency in pairs(tableModel.dependencies) do table.insert(dependencies, dependency) end
    table.sort(dependencies, CompareTableNames)
    for _, dependency in ipairs(dependencies) do VisitTable(dependency, state, ordered) end
    state[tableModel] = 2
    table.insert(ordered, tableModel)
end

function M.Build(model, definitions)
    definitions = definitions or (model and model.definitionsByTarget and model.definitionsByTarget.postgres) or {}

    local result =
    {
        version = 1,
        tables = {},
        bundles = {},
        tablesByDefinition = {},
        tablesByPersistentId = {}
    }
    local persistentIds = {}
    local tableModels = {}
    local tablesByPhysicalName = {}
    local tablesByName = {}
    local indexesByPhysicalName = {}
    local cppTableNames = {}

    local function RegisterIndexRelation(definition, tableModel, name, description)
        local physicalKey = tableModel.schema .. "\0" .. name
        local tableOwner = tablesByPhysicalName[physicalKey]
        if tableOwner ~= nil then
            Fail(definition, description .. " name '" .. name .. "' conflicts with table " .. DefinitionLabel(tableOwner.definition))
        end
        local indexOwner = indexesByPhysicalName[physicalKey]
        if indexOwner ~= nil then
            Fail(definition, description .. " name '" .. name .. "' is already used by " .. indexOwner.description ..
                " in " .. DefinitionLabel(indexOwner.definition))
        end
        indexesByPhysicalName[physicalKey] = { definition = definition, description = description }
    end

    for _, definition in ipairs(definitions) do
        if definition.kind ~= "databaseTable" or definition.baseType ~= Type.STRUCT then
            Fail(definition, "PostgreSQL target only supports DatabaseTable definitions")
        end
        if #definition.fields == 0 then Fail(definition, "database table must declare at least one field") end
        ValidateLogicalName(definition, definition.name, "definition name")

        local options = definition.options or {}
        for key in pairs(options) do
            if not tableOptionKeys[key] then Fail(definition, "table uses unsupported PostgreSQL option '" .. tostring(key) .. "'") end
        end
        local bundle = options.database
        if bundleRanks[bundle] == nil then
            Fail(definition, "database bundle must be one of auth, character, world, or shared")
        end
        ValidateIdentifier(definition, options.schema, "schema")
        ValidateIdentifier(definition, options.table, "table")

        local definitionName = definition.namespace .. "::" .. definition.name
        local cppTableKey = bundle .. "\0" .. definition.name
        if cppTableNames[cppTableKey] ~= nil then
            Fail(definition, "generated table name '" .. definition.name .. "' is already used by " ..
                DefinitionLabel(cppTableNames[cppTableKey]))
        end
        cppTableNames[cppTableKey] = definition
        local physicalKey = options.schema .. "\0" .. options.table
        if tablesByPhysicalName[physicalKey] ~= nil then
            Fail(definition, "physical table '" .. options.schema .. "." .. options.table .. "' is already declared by " .. DefinitionLabel(tablesByPhysicalName[physicalKey].definition))
        end

        RegisterPersistentId(definition, persistentIds, definition.persistentId, "table persistentId")
        local tableModel =
        {
            definition = definition,
            definitionName = definitionName,
            name = definition.name,
            persistentId = definition.persistentId,
            bundle = bundle,
            schema = options.schema,
            tableName = options.table,
            fields = {},
            fieldByName = {},
            fieldByColumnName = {},
            constraints = {},
            indexes = {},
            queries = {},
            operations = {},
            orderBy = {},
            uniqueKeys = {},
            dependencies = {}
        }
        tablesByPhysicalName[physicalKey] = tableModel
        result.tablesByDefinition[definitionName] = tableModel
        result.tablesByPersistentId[definition.persistentId] = tableModel
        local tableModelsByName = tablesByName[definition.name] or {}
        tableModelsByName[#tableModelsByName + 1] = tableModel
        tablesByName[definition.name] = tableModelsByName
        tableModels[#tableModels + 1] = tableModel
    end

    table.sort(tableModels, CompareTables)
    for _, tableModel in ipairs(tableModels) do
        local definition = tableModel.definition
        for _, field in ipairs(definition.fields) do
            ValidateLogicalName(definition, field.name, "field name")
            local attributes = field.attributes
            local column = attributes and attributes.postgres
            if type(column) ~= "table" or column.kind ~= "column" then
                FailField(definition, field, "requires postgres = Postgres.Column(...) metadata")
            end
            for option in pairs(column) do
                if option ~= "kind" and option ~= "name" and option ~= "type" and option ~= "nullable" and
                    option ~= "identity" and option ~= "default" and option ~= "defaultSql" and option ~= "rawSql" and option ~= "sql" then
                    FailField(definition, field, "uses unsupported PostgreSQL column option '" .. tostring(option) .. "'")
                end
            end
            ValidateIdentifier(definition, column.name, "column name")
            if tableModel.fieldByColumnName[column.name] ~= nil then
                FailField(definition, field, "physical column '" .. column.name .. "' is duplicated")
            end
            if column.nullable == nil or type(column.nullable) ~= "boolean" then
                FailField(definition, field, "Postgres.Column nullable must be explicitly boolean")
            end
            if column.identity ~= nil and column.identity ~= "always" and column.identity ~= "byDefault" then
                FailField(definition, field, "identity must be 'always' or 'byDefault'")
            end
            if column.defaultSql ~= nil or column.rawSql ~= nil or column.sql ~= nil then
                FailField(definition, field, "raw SQL is not allowed in ordinary column metadata")
            end
            if not Postgres.IsType(column.type) then
                FailField(definition, field, "postgres must use a typed PostgreSQL storage type")
            end
            if type(column.type.name) ~= "string" or type(column.type.sql) ~= "string" or type(column.type.category) ~= "string" then
                FailField(definition, field, "uses an invalid PostgreSQL storage type descriptor")
            end
            if column.identity ~= nil and column.type.category ~= "integer" then
                FailField(definition, field, "identity is only supported for integer PostgreSQL types")
            end

            local fieldModel =
            {
                source = field,
                name = field.name,
                persistentId = attributes.persistentId,
                semanticType = field.type,
                columnName = column.name,
                postgresType = column.type,
                nullable = column.nullable,
                identity = column.identity,
                defaultValue = column.default,
                hasDefault = column.default ~= nil
            }
            if attributes.default ~= nil then
                if fieldModel.hasDefault then FailField(definition, field, "default must be declared either on the field or on Postgres.Column, not both") end
                fieldModel.defaultValue = attributes.default
                fieldModel.hasDefault = true
            end

            fieldModel.rangeRestriction = ResolveStorageType(definition, field, column.type)
            if fieldModel.hasDefault then ValidateValue(definition, fieldModel, fieldModel.defaultValue, "default") end
            if column.identity ~= nil and fieldModel.hasDefault then
                FailField(definition, field, "identity columns cannot also declare a default")
            end

            RegisterPersistentId(definition, persistentIds, fieldModel.persistentId, "field '" .. field.name .. "' persistentId")
            table.insert(tableModel.fields, fieldModel)
            tableModel.fieldByName[field.name] = fieldModel
            tableModel.fieldByColumnName[column.name] = fieldModel
        end
    end

    for _, tableModel in ipairs(tableModels) do
        local definition = tableModel.definition
        local options = definition.options or {}
        local constraintNames = {}
        local indexNames = {}
        local queryNames = {}
        local operationNames = {}
        local descriptorNames =
        {
            Record = true,
            Insert = true,
            ByPrimaryKey = true,
            PrimaryKey = true,
            InsertParameters = true,
            Decode = true,
            BUNDLE = true,
            SCHEMA_NAME = true,
            TABLE_NAME = true,
            PERSISTENT_ID = true,
            QUALIFIED_NAME = true,
            SELECT_COLUMNS = true,
            SELECT_ALL_SQL = true,
            PRIMARY_KEY_PREDICATE = true,
            SELECT_BY_PRIMARY_KEY_SQL = true,
            INSERT_SQL = true
        }

        tableModel.orderBy = ResolveFieldReferences(definition, tableModel, options.orderBy or {}, "table order", true, false)

        if options.primaryKey ~= nil then
            local primaryKey = options.primaryKey
            ValidateSchemaObject(definition, primaryKey, "primaryKey", persistentIds)
            local fields = ResolveFieldReferences(definition, tableModel, primaryKey.fields, "primary key fields", false, false)
            if #fields == 0 then Fail(definition, "primary key must contain at least one field") end
            for _, field in ipairs(fields) do
                if field.field.nullable then FailField(definition, field.field.source, "primary key columns cannot be nullable") end
            end
            tableModel.primaryKey = { name = primaryKey.name, persistentId = primaryKey.persistentId, fields = fields }
            table.insert(tableModel.uniqueKeys, { signature = FieldSetSignature(fields), nonNull = true })
            constraintNames[primaryKey.name] = true
            RegisterIndexRelation(definition, tableModel, primaryKey.name, "primary key")
        end

        for _, constraint in ipairs(GetList(definition, options, "constraints", "constraints")) do
            if type(constraint) ~= "table" or (constraint.kind ~= "unique" and constraint.kind ~= "foreignKey" and constraint.kind ~= "check" and constraint.kind ~= "rawCheck") then
                Fail(definition, "constraints must contain Unique, ForeignKey, Check, or RawCheck descriptors")
            end
            ValidateSchemaObject(definition, constraint, constraint.kind, persistentIds)
            if constraintNames[constraint.name] then Fail(definition, "constraint name '" .. constraint.name .. "' is duplicated") end
            constraintNames[constraint.name] = true

            if constraint.kind == "unique" then
                local fields = ResolveFieldReferences(definition, tableModel, constraint.fields, "unique constraint fields", false, false)
                if #fields == 0 then Fail(definition, "unique constraint must contain at least one field") end
                local nonNull = true
                for _, field in ipairs(fields) do nonNull = nonNull and not field.field.nullable end
                table.insert(tableModel.constraints, { kind = "unique", name = constraint.name, persistentId = constraint.persistentId, fields = fields })
                table.insert(tableModel.uniqueKeys, { signature = FieldSetSignature(fields), nonNull = nonNull })
                RegisterIndexRelation(definition, tableModel, constraint.name, "unique constraint")
            elseif constraint.kind == "foreignKey" then
                local fields = ResolveFieldReferences(definition, tableModel, constraint.fields, "foreign key fields", false, false)
                local target = ResolveReferencedTable(definition, constraint.referencedTable, result.tablesByDefinition, result.tablesByPersistentId, tablesByName)
                if constraint.referencedDatabase ~= nil and constraint.referencedDatabase ~= target.bundle then
                    Fail(definition, "foreign key explicitly references a different logical database bundle")
                end
                if target.bundle ~= tableModel.bundle then
                    Fail(definition, "foreign keys across logical database bundles are forbidden")
                end
                local targetFields = ResolveFieldReferences(definition, target, constraint.referencedFields, "foreign key referenced fields", false, false)
                if #fields == 0 then Fail(definition, "foreign key must contain at least one field") end
                if #fields ~= #targetFields then Fail(definition, "foreign key local and referenced field counts differ") end
                for index, field in ipairs(fields) do
                    local targetField = targetFields[index]
                    if field.field.postgresType.name ~= targetField.field.postgresType.name then
                        Fail(definition, "foreign key field '" .. field.name .. "' has a different PostgreSQL type from referenced field '" .. targetField.name .. "'")
                    end
                end
                local onUpdate = NormalizeForeignKeyAction(definition, constraint, "onUpdate")
                local onDelete = NormalizeForeignKeyAction(definition, constraint, "onDelete")
                if onUpdate == "setNull" or onDelete == "setNull" then
                    for _, field in ipairs(fields) do
                        if not field.field.nullable then
                            FailField(definition, field.field.source, "foreign key SET NULL actions require nullable local columns")
                        end
                    end
                end
                if onUpdate == "setDefault" or onDelete == "setDefault" then
                    for _, field in ipairs(fields) do
                        if not field.field.nullable and not field.field.hasDefault and field.field.identity == nil then
                            FailField(definition, field.field.source, "foreign key SET DEFAULT actions require a default or nullable local column")
                        end
                    end
                end
                tableModel.dependencies[target] = true
                table.insert(tableModel.constraints,
                {
                    kind = "foreignKey",
                    name = constraint.name,
                    persistentId = constraint.persistentId,
                    fields = fields,
                    target = target,
                    targetFields = targetFields,
                    onUpdate = onUpdate,
                    onDelete = onDelete
                })
            elseif constraint.kind == "check" then
                local fields = ResolveFieldReferences(definition, tableModel, constraint.fields, "check constraint fields", false, false)
                local operators = { equal = true, notEqual = true, greaterThan = true, greaterEqual = true, lessThan = true, lessEqual = true, isTrue = true, isFalse = true, ["in"] = true }
                if not operators[constraint.operator] then Fail(definition, "check constraint uses unsupported typed operator '" .. tostring(constraint.operator) .. "'") end
                if #fields ~= 1 then Fail(definition, "typed check constraints must reference exactly one field") end
                if constraint.operator == "in" then
                    ValidateArray(definition, constraint.value, "check constraint values")
                    if #constraint.value == 0 then Fail(definition, "check constraint values must not be empty") end
                    for _, value in ipairs(constraint.value) do ValidateValue(definition, fields[1].field, value, "check constraint value") end
                elseif constraint.operator ~= "isTrue" and constraint.operator ~= "isFalse" then
                    ValidateValue(definition, fields[1].field, constraint.value, "check constraint value")
                else
                    if fields[1].field.semanticType ~= Type.BOOL then
                        Fail(definition, "isTrue/isFalse check constraints require a boolean field")
                    end
                    if constraint.value ~= nil then Fail(definition, "isTrue/isFalse check constraints must not provide a value") end
                end
                table.insert(tableModel.constraints,
                {
                    kind = "check",
                    name = constraint.name,
                    persistentId = constraint.persistentId,
                    fields = fields,
                    operator = constraint.operator,
                    value = constraint.value
                })
            else
                table.insert(tableModel.constraints,
                {
                    kind = "rawCheck",
                    name = constraint.name,
                    persistentId = constraint.persistentId,
                    rawSql = constraint.rawSql
                })
            end
        end

        for _, index in ipairs(GetList(definition, options, "indexes", "indexes")) do
            ValidateSchemaObject(definition, index, "index", persistentIds)
            if indexNames[index.name] then Fail(definition, "index name '" .. index.name .. "' is duplicated") end
            indexNames[index.name] = true
            RegisterIndexRelation(definition, tableModel, index.name, "index")
            if index.unique ~= nil and type(index.unique) ~= "boolean" then Fail(definition, "index '" .. index.name .. "' unique must be boolean") end
            local fields = ResolveFieldReferences(definition, tableModel, index.fields, "index fields", true, false)
            if #fields == 0 then Fail(definition, "index must contain at least one field") end
            local unique = index.unique == true
            table.insert(tableModel.indexes, { name = index.name, persistentId = index.persistentId, fields = fields, unique = unique })
            if unique then
                local nonNull = true
                for _, field in ipairs(fields) do nonNull = nonNull and not field.field.nullable end
                table.insert(tableModel.uniqueKeys, { signature = FieldSetSignature(fields), nonNull = nonNull })
            end
        end

        for _, query in ipairs(GetList(definition, options, "queries", "queries")) do
            ValidateSchemaObject(definition, query, "query", persistentIds)
            if queryNames[query.name] then Fail(definition, "query name '" .. query.name .. "' is duplicated") end
            queryNames[query.name] = true
            if descriptorNames[query.name] then Fail(definition, "generated descriptor name '" .. query.name .. "' is reserved or duplicated") end
            descriptorNames[query.name] = true
            local cardinality = query.cardinality or "zeroOrMore"
            if cardinality ~= "zeroOrMore" and cardinality ~= "zeroOrOne" and cardinality ~= "exactlyOne" then
                Fail(definition, "query '" .. query.name .. "' cardinality must be zeroOrMore, zeroOrOne, or exactlyOne")
            end
            local fields = ResolveFieldReferences(definition, tableModel, query.fields, "query fields", false, true)
            local orderBy = ResolveFieldReferences(definition, tableModel, query.orderBy or {}, "query order", true, false)
            if #fields == 0 then Fail(definition, "query must contain at least one field") end
            if cardinality ~= "zeroOrMore" and not HasUniqueKey(tableModel, fields) then
                Fail(definition, "query '" .. query.name .. "' claims at most one row without a non-null primary or unique key")
            end
            table.insert(tableModel.queries, { name = query.name, persistentId = query.persistentId, fields = fields,
                cardinality = cardinality, orderBy = orderBy })
        end

        for _, operation in ipairs(GetList(definition, options, "operations", "operations")) do
            if type(operation) ~= "table" or (operation.kind ~= "update" and operation.kind ~= "delete" and
                operation.kind ~= "upsert" and operation.kind ~= "function") then
                Fail(definition, "operations must contain Update, Delete, Upsert, or Function descriptors")
            end
            ValidateSchemaObject(definition, operation, operation.kind, persistentIds)
            if operationNames[operation.name] then Fail(definition, "operation name '" .. operation.name .. "' is duplicated") end
            operationNames[operation.name] = true
            if descriptorNames[operation.name] then Fail(definition, "generated descriptor name '" .. operation.name .. "' is reserved or duplicated") end
            descriptorNames[operation.name] = true
            local allowParameters = operation.kind == "function"
            local keyFields = ResolveFieldReferences(definition, tableModel, operation.fields, "operation key fields", false,
                allowParameters, operation.kind == "function")
            if #keyFields == 0 then Fail(definition, "operation must contain at least one key field") end
            local cardinality = operation.cardinality or "exactlyOne"
            if cardinality ~= "exactlyOne" and cardinality ~= "zeroOrMore" then
                Fail(definition, "operation '" .. operation.name .. "' cardinality must be exactlyOne or zeroOrMore")
            end
            if operation.kind ~= "function" and operation.kind ~= "upsert" and cardinality == "exactlyOne" and not HasUniqueKey(tableModel, keyFields) then
                Fail(definition, "operation '" .. operation.name .. "' does not use a non-null primary or unique key")
            end

            local updateFields = nil
            if operation.kind == "update" then
                updateFields = ResolveFieldReferences(definition, tableModel, operation.updateFields, "update fields", false, false)
                if #updateFields == 0 then Fail(definition, "update operation must change at least one field") end
                for _, updateField in ipairs(updateFields) do
                    for _, keyField in ipairs(keyFields) do
                        if updateField.name == keyField.name then Fail(definition, "update operation cannot update its key field '" .. updateField.name .. "'") end
                    end
                end
            elseif operation.kind == "upsert" then
                if not HasUniqueKey(tableModel, keyFields) then
                    Fail(definition, "upsert operation '" .. operation.name .. "' conflict fields are not a non-null primary or unique key")
                end
                updateFields = ResolveFieldReferences(definition, tableModel, operation.updateFields, "upsert update fields", false, false)
            end
            local insertFields = nil
            if operation.kind == "upsert" then
                insertFields = ResolveFieldReferences(definition, tableModel, operation.insertFields, "upsert insert fields", false, false)
                if #insertFields == 0 then Fail(definition, "upsert operation must insert at least one field") end
            end
            table.insert(tableModel.operations,
            {
                kind = operation.kind,
                name = operation.name,
                persistentId = operation.persistentId,
                fields = keyFields,
                updateFields = updateFields,
                insertFields = insertFields,
                cardinality = cardinality,
                functionName = operation.functionName
            })
        end
    end

    for _, tableModel in ipairs(tableModels) do
        for _, constraint in ipairs(tableModel.constraints) do
            if constraint.kind == "foreignKey" and not HasAnyUniqueKey(constraint.target, constraint.targetFields) then
                Fail(tableModel.definition, "foreign key '" .. constraint.name .. "' must reference a primary key or unique key")
            end
        end
    end

    local orderedTables = {}
    local state = {}
    for _, tableModel in ipairs(tableModels) do VisitTable(tableModel, state, orderedTables) end
    result.tables = orderedTables

    for _, tableModel in ipairs(orderedTables) do
        local bundle = result.bundles[tableModel.bundle]
        if bundle == nil then
            bundle = { name = tableModel.bundle, tables = {}, schemas = {} }
            result.bundles[tableModel.bundle] = bundle
        end
        table.insert(bundle.tables, tableModel)
    end

    local orderedBundles = {}
    for _, bundleName in ipairs(bundleOrder) do
        if result.bundles[bundleName] ~= nil then table.insert(orderedBundles, result.bundles[bundleName]) end
    end
    result.bundles = orderedBundles

    for _, bundle in ipairs(result.bundles) do
        local schemasByName = {}
        for _, tableModel in ipairs(bundle.tables) do
            local schema = schemasByName[tableModel.schema]
            if schema == nil then
                schema = { name = tableModel.schema, tables = {} }
                schemasByName[tableModel.schema] = schema
                table.insert(bundle.schemas, schema)
            end
            table.insert(schema.tables, tableModel)
        end
        table.sort(bundle.schemas, function(a, b) return a.name < b.name end)
    end

    result.canonical = BuildCanonical(result)

    -- The returned view contains only backend-owned normalized data. Source
    -- definitions are used above for diagnostics and are not retained.
    for _, tableModel in ipairs(result.tables) do
        tableModel.definition = nil
        for _, field in ipairs(tableModel.fields) do field.source = nil end
    end
    return result
end

return M

local D = require("Definition")
local P = require("Postgres")

local M = { Definition = D, Postgres = P, Type = require("Type") }

function M.Camel(value)
    return (value:gsub("_(%w)", function(character) return character:upper() end))
end

function M.Pascal(value)
    value = M.Camel(value)
    return value:sub(1, 1):upper() .. value:sub(2)
end

function M.Column(name, semanticType, postgresType, options)
    options = options or {}
    return { name = name, semanticType = semanticType, postgresType = postgresType, nullable = options.nullable,
        identity = options.identity, default = options.default, persistentId = options.persistentId,
        fieldName = options.fieldName }
end

function M.Unique(tableName, name, columns, persistentId)
    local fields = {}
    for _, column in ipairs(columns) do fields[#fields + 1] = M.Camel(column) end
    return P.Unique(name, fields, { persistentId = persistentId or "postgres." .. tableName .. "." .. name })
end

function M.ForeignKey(tableName, name, columns, target, targetColumns, onDelete, persistentId)
    local fields, referencedFields = {}, {}
    for _, column in ipairs(columns) do fields[#fields + 1] = M.Camel(column) end
    for _, column in ipairs(targetColumns) do referencedFields[#referencedFields + 1] = M.Camel(column) end
    return P.ForeignKey(name, fields, M.Pascal(target), referencedFields,
        { persistentId = persistentId or "postgres." .. tableName .. "." .. name, onDelete = onDelete })
end

function M.Table(bundle, tableName, columns, options)
    options = options or {}
    local tableId, fields = options.persistentId or "postgres." .. tableName, {}
    for _, column in ipairs(columns) do
        local attributes = { persistentId = column.persistentId or tableId .. "." .. column.name,
            postgres = P.Column(column.name, column.postgresType,
                { nullable = column.nullable == true, identity = column.identity and "byDefault" or nil }) }
        if column.default ~= nil then attributes.default = column.default end
        fields[#fields + 1] = D.Field(column.fieldName or M.Camel(column.name), column.semanticType, attributes)
    end
    local primaryKey
    if options.noPrimaryKey ~= true then
        primaryKey = P.PrimaryKey(tableName .. "_pkey", { "id" }, { persistentId = tableId .. ".pk" })
    end
    return D.DatabaseTable(options.name or M.Pascal(tableName), fields, { persistentId = tableId, database = bundle,
        schema = "public", table = tableName, primaryKey = primaryKey, constraints = options.constraints, indexes = options.indexes,
        queries = options.queries, operations = options.operations, orderBy = options.orderBy })
end

function M.Upsert(tableName, fields, updateFields)
    return P.Upsert("Set", fields, { "id" }, updateFields, { persistentId = "postgres." .. tableName .. ".set" })
end

M.Identity = { identity = true }
M.Nullable = { nullable = true }
function M.Default(value) return { default = value } end

return M

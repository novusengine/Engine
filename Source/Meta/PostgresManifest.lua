local Sha256 = require("Sha256")

local M = {}

local function Frame(value)
    value = tostring(value)
    return tostring(#value) .. ":" .. value
end

local function References(references)
    local parts = {}
    for _, reference in ipairs(references or {}) do
        parts[#parts + 1] = (reference.persistentId or reference.field.persistentId) .. ":" .. tostring(reference.direction or "")
    end
    return table.concat(parts, ",")
end

local function Value(value)
    if type(value) ~= "table" then return type(value) .. ":" .. tostring(value) end
    local parts = {}
    for _, item in ipairs(value) do parts[#parts + 1] = Value(item) end
    return "array:[" .. table.concat(parts, ",") .. "]"
end

local function SnapshotCanonical(value)
    local kind = type(value)
    if kind ~= "table" then return Frame(kind) .. Frame(tostring(value)) end
    local keys = {}
    for key in pairs(value) do if key ~= "model" then keys[#keys + 1] = key end end
    table.sort(keys, function(left, right)
        if type(left) ~= type(right) then return type(left) < type(right) end
        return left < right
    end)
    local parts = { Frame("table") }
    for _, key in ipairs(keys) do parts[#parts + 1] = SnapshotCanonical(key) .. SnapshotCanonical(value[key]) end
    return table.concat(parts)
end

local function Canonical(bundle, schemas, tables)
    return Frame("metagen-postgres-manifest-v3") .. Frame(bundle) ..
        SnapshotCanonical({ schemas = schemas, tables = tables }) .. "\n"
end

function M.Build(bundle)
    local tables = {}
    local schemas = {}
    for _, schema in ipairs(bundle.schemas) do schemas[schema.name] = true end
    for _, tableModel in ipairs(bundle.tables) do
        local tableSnapshot =
        {
            persistentId = tableModel.persistentId,
            schema = tableModel.schema,
            tableName = tableModel.tableName,
            model = tableModel,
            fields = {},
            indexes = {},
            constraints = {},
            primaryKey = nil
        }
        for fieldOrder, field in ipairs(tableModel.fields) do
            tableSnapshot.fields[field.persistentId] =
            {
                persistentId = field.persistentId,
                columnName = field.columnName,
                postgresType = { name = field.postgresType.name, sql = field.postgresType.sql },
                nullable = field.nullable,
                identity = field.identity,
                hasDefault = field.hasDefault,
                defaultValue = field.defaultValue,
                order = fieldOrder
            }
        end
        local function SnapshotReferences(references)
            local result = {}
            for _, reference in ipairs(references or {}) do
                result[#result + 1] =
                {
                    persistentId = reference.field.persistentId,
                    columnName = reference.field.columnName,
                    direction = reference.direction
                }
            end
            return result
        end
        if tableModel.primaryKey ~= nil then
            tableSnapshot.primaryKey =
            {
                persistentId = tableModel.primaryKey.persistentId,
                name = tableModel.primaryKey.name,
                fields = SnapshotReferences(tableModel.primaryKey.fields),
                signature = tableModel.primaryKey.name .. "|" .. References(tableModel.primaryKey.fields)
            }
        end
        for _, index in ipairs(tableModel.indexes) do
            tableSnapshot.indexes[index.persistentId] =
            {
                persistentId = index.persistentId,
                name = index.name,
                unique = index.unique,
                fields = SnapshotReferences(index.fields),
                signature = index.name .. "|" .. tostring(index.unique) .. "|" .. References(index.fields)
            }
        end
        for _, constraint in ipairs(tableModel.constraints) do
            tableSnapshot.constraints[constraint.persistentId] =
            {
                persistentId = constraint.persistentId,
                kind = constraint.kind,
                name = constraint.name,
                signature = constraint.kind .. "|" .. constraint.name .. "|" .. References(constraint.fields) .. "|" ..
                    tostring(constraint.operator or "") .. "|" .. Value(constraint.value) .. "|" .. tostring(constraint.rawSql or "") .. "|" ..
                    tostring(constraint.target and constraint.target.persistentId or "") .. "|" .. References(constraint.targetFields) .. "|" ..
                    tostring(constraint.onUpdate or "") .. "|" .. tostring(constraint.onDelete or "")
            }
        end
        tables[tableModel.persistentId] = tableSnapshot
    end
    local canonical = Canonical(bundle.name, schemas, tables)
    return { format = 3, bundle = bundle.name, canonical = canonical, hash = Sha256.Hash(canonical), schemas = schemas, tables = tables }
end

function M.Empty(bundle)
    local schemas, tables = {}, {}
    local canonical = Canonical(bundle, schemas, tables)
    return { format = 3, bundle = bundle, canonical = canonical, hash = Sha256.Hash(canonical), schemas = schemas, tables = tables }
end


function M.Validate(manifest)
    local canonical = Canonical(manifest.bundle, manifest.schemas, manifest.tables)
    assert(manifest.hash == Sha256.Hash(canonical), "committed PostgreSQL manifest hash mismatch")
    return true
end

return M

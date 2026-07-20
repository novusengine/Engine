local PostgresManifest = require("PostgresManifest")
local PostgresSql = require("PostgresSql")
local Sha256 = require("Sha256")

local M = {}

local function Frame(value)
    value = tostring(value)
    return tostring(#value) .. ":" .. value
end

local widening =
{
    smallint = { integer = true, bigint = true, real = true, ["double precision"] = true },
    integer = { bigint = true, ["double precision"] = true },
    bigint = {},
    real = { ["double precision"] = true }
}
local operationOrder =
{
    createSchema = 5,
    createTable = 10,
    moveTable = 20,
    renameTable = 30,
    renameColumn = 40,
    addColumn = 50,
    widenColumn = 60,
    dropNotNull = 65,
    addIndex = 70
}

local function Qualified(snapshot)
    return PostgresSql.QuoteIdentifier(snapshot.schema) .. "." .. PostgresSql.QuoteIdentifier(snapshot.tableName)
end

local function Add(result, kind, persistentId, sql)
    result.nextOperationOrder = (result.nextOperationOrder or 0) + 1
    result.operations[#result.operations + 1] =
        { kind = kind, persistentId = persistentId, sql = sql, order = result.nextOperationOrder }
end

local function Hazard(result, kind, persistentId, message)
    result.hazards[#result.hazards + 1] = { kind = kind, persistentId = persistentId, message = message }
end

local function SamePrimaryKey(left, right)
    if left == nil or right == nil then return left == right end
    if left.persistentId ~= right.persistentId or left.signature ~= right.signature or #left.fields ~= #right.fields then return false end
    for index, reference in ipairs(left.fields) do
        local leftId = reference.persistentId or reference.field.persistentId
        local rightReference = right.fields[index]
        local rightId = rightReference.persistentId or rightReference.field.persistentId
        if leftId ~= rightId then return false end
    end
    return true
end

local function IsWidening(oldType, newType)
    if widening[oldType.name] and widening[oldType.name][newType.name] then return true end
    local oldLength = oldType.name:match("^varchar%((%d+)%)$")
    local newLength = newType.name:match("^varchar%((%d+)%)$")
    if oldLength and newLength then return tonumber(newLength) >= tonumber(oldLength) end
    return oldLength ~= nil and newType.name == "text"
end

local function DiffTable(result, oldTable, newTable)
    local oldQualified = Qualified(oldTable)
    if oldTable.schema ~= newTable.schema or oldTable.tableName ~= newTable.tableName then
        if oldTable.schema ~= newTable.schema then
            Add(result, "moveTable", newTable.persistentId, "ALTER TABLE " .. oldQualified .. " SET SCHEMA " ..
                PostgresSql.QuoteIdentifier(newTable.schema) .. ";")
            oldQualified = PostgresSql.QuoteIdentifier(newTable.schema) .. "." .. PostgresSql.QuoteIdentifier(oldTable.tableName)
        end
        if oldTable.tableName ~= newTable.tableName then
            Add(result, "renameTable", newTable.persistentId, "ALTER TABLE " .. oldQualified .. " RENAME TO " ..
                PostgresSql.QuoteIdentifier(newTable.tableName) .. ";")
        end
    end

    if not SamePrimaryKey(oldTable.primaryKey, newTable.primaryKey) then
        Hazard(result, "primaryKeyChange", newTable.persistentId, "primary-key changes require explicit handling")
    end

    local targetQualified = Qualified(newTable)
    for fieldId, oldField in pairs(oldTable.fields) do
        local newField = newTable.fields[fieldId]
        if newField == nil then
            Hazard(result, "dropColumn", fieldId, "dropping column '" .. oldField.columnName .. "' requires explicit handling")
        else
            if oldField.order ~= newField.order then
                Hazard(result, "columnOrderChange", fieldId, "changing physical column order requires an explicit table rebuild")
            end
            if oldField.columnName ~= newField.columnName then
                Add(result, "renameColumn", fieldId, "ALTER TABLE " .. targetQualified .. " RENAME COLUMN " ..
                    PostgresSql.QuoteIdentifier(oldField.columnName) .. " TO " .. PostgresSql.QuoteIdentifier(newField.columnName) .. ";")
            end
            if oldField.postgresType.name ~= newField.postgresType.name then
                if IsWidening(oldField.postgresType, newField.postgresType) then
                    Add(result, "widenColumn", fieldId, "ALTER TABLE " .. targetQualified .. " ALTER COLUMN " ..
                        PostgresSql.QuoteIdentifier(newField.columnName) .. " TYPE " .. newField.postgresType.sql .. ";")
                else
                    Hazard(result, "unsafeTypeChange", fieldId, "type change from " .. oldField.postgresType.name ..
                        " to " .. newField.postgresType.name .. " requires explicit handling")
                end
            end
            if oldField.nullable and not newField.nullable then
                Hazard(result, "makeNotNull", fieldId, "nullable-to-not-null requires an explicit data-fill step")
            elseif not oldField.nullable and newField.nullable then
                Add(result, "dropNotNull", fieldId, "ALTER TABLE " .. targetQualified .. " ALTER COLUMN " ..
                    PostgresSql.QuoteIdentifier(newField.columnName) .. " DROP NOT NULL;")
            end
            if oldField.identity ~= newField.identity then Hazard(result, "identityChange", fieldId, "identity changes require explicit handling") end
            if oldField.hasDefault ~= newField.hasDefault or tostring(oldField.defaultValue) ~= tostring(newField.defaultValue) then
                Hazard(result, "defaultChange", fieldId, "default changes require explicit handling")
            end
        end
    end
    for fieldId, newField in pairs(newTable.fields) do
        if oldTable.fields[fieldId] == nil then
            if not newField.nullable and not newField.hasDefault and newField.identity == nil then
                Hazard(result, "addRequiredColumn", fieldId, "new non-null column requires a default or explicit data-fill step")
            else
                Add(result, "addColumn", fieldId, "ALTER TABLE " .. targetQualified .. " ADD COLUMN " ..
                    PostgresSql.ColumnDefinition(newField) .. ";")
            end
        end
    end
    for indexId, index in pairs(newTable.indexes) do
        if oldTable.indexes[indexId] == nil then
            Add(result, "addIndex", indexId, PostgresSql.IndexSql(newTable.model, index))
        elseif oldTable.indexes[indexId].signature ~= index.signature then
            Hazard(result, "changeIndex", indexId, "index changes require explicit handling")
        end
    end
    for indexId, index in pairs(oldTable.indexes) do
        if newTable.indexes[indexId] == nil then Hazard(result, "dropIndex", indexId, "dropping index '" .. index.name .. "' requires explicit handling") end
    end
    for constraintId in pairs(newTable.constraints) do
        if oldTable.constraints[constraintId] == nil then
            Hazard(result, "addConstraint", constraintId, "constraints over existing data require explicit validation")
        elseif oldTable.constraints[constraintId].signature ~= newTable.constraints[constraintId].signature then
            Hazard(result, "changeConstraint", constraintId, "constraint changes require explicit validation")
        end
    end
    for constraintId in pairs(oldTable.constraints) do
        if newTable.constraints[constraintId] == nil then Hazard(result, "dropConstraint", constraintId, "dropping constraints requires explicit handling") end
    end
end

local function Sort(items)
    table.sort(items, function(left, right)
        local leftOrder, rightOrder = operationOrder[left.kind] or 100, operationOrder[right.kind] or 100
        if leftOrder ~= rightOrder then return leftOrder < rightOrder end
        if left.kind == "createTable" and right.kind == "createTable" and left.order ~= right.order then return left.order < right.order end
        if left.persistentId == right.persistentId then return left.kind < right.kind end
        return left.persistentId < right.persistentId
    end)
end

function M.Diff(oldManifest, bundle)
    assert(oldManifest.bundle == bundle.name, "PostgreSQL manifest bundle mismatch")
    local target = PostgresManifest.Build(bundle)
    local result = { bundle = bundle.name, parentHash = oldManifest.hash, targetHash = target.hash, operations = {}, hazards = {} }
    for schema in pairs(oldManifest.schemas or {}) do
        if not target.schemas[schema] then Hazard(result, "dropSchema", schema, "dropping schema '" .. schema .. "' requires explicit handling") end
    end
    for schema in pairs(target.schemas) do
        if not (oldManifest.schemas or {})[schema] then
            Add(result, "createSchema", schema, "CREATE SCHEMA IF NOT EXISTS " .. PostgresSql.QuoteIdentifier(schema) .. ";")
        end
    end
    for tableId, oldTable in pairs(oldManifest.tables) do
        if target.tables[tableId] == nil then Hazard(result, "dropTable", tableId, "dropping table '" .. oldTable.tableName .. "' requires explicit handling") end
    end
    for _, tableModel in ipairs(bundle.tables) do
        local tableId = tableModel.persistentId
        local newTable = target.tables[tableId]
        local oldTable = oldManifest.tables[tableId]
        if oldTable == nil then
            Add(result, "createTable", tableId, PostgresSql.TableSql(newTable.model))
            for indexId, index in pairs(newTable.indexes) do Add(result, "addIndex", indexId, PostgresSql.IndexSql(newTable.model, index)) end
        else
            DiffTable(result, oldTable, newTable)
        end
    end
    Sort(result.operations)
    Sort(result.hazards)
    result.targetManifest = target
    return result
end

local function ContentHash(artifact)
    local parts = { Frame("metagen-postgres-migration-v1"), Frame(artifact.id), Frame(artifact.name), Frame(artifact.bundle),
        Frame(artifact.parentHash), Frame(artifact.targetHash), Frame(artifact.transactional) }
    for _, operation in ipairs(artifact.operations or {}) do
        parts[#parts + 1] = Frame(operation.kind) .. Frame(operation.persistentId) .. Frame(operation.sql)
    end
    parts[#parts + 1] = Frame(artifact.sql)
    return Sha256.Hash(table.concat(parts))
end

function M.BuildArtifact(diff, id, name)
    assert(#diff.hazards == 0, "PostgreSQL migration has unresolved hazards")
    assert(type(id) == "string" and id:match("^[a-z0-9][a-z0-9_%-]*$"), "invalid migration ID")
    assert(type(name) == "string" and name:match("%S") ~= nil and name:match("[%c]") == nil and #name <= 120,
        "migration name must contain 1-120 printable characters")
    local sql = { "BEGIN;" }
    for _, operation in ipairs(diff.operations) do sql[#sql + 1] = operation.sql end
    sql[#sql + 1] = "COMMIT;"
    sql[#sql + 1] = ""
    sql = table.concat(sql, "\n")
    local operations = {}
    for _, operation in ipairs(diff.operations) do
        operations[#operations + 1] = { kind = operation.kind, persistentId = operation.persistentId, sql = operation.sql }
    end
    local artifact = { id = id, name = name, bundle = diff.bundle, parentHash = diff.parentHash, targetHash = diff.targetHash,
        transactional = true, operations = operations, sql = sql }
    artifact.contentHash = ContentHash(artifact)
    return artifact
end

function M.ValidateChain(bundle, migrations, expectedTargetHash, expectedParentHash)
    local previousTarget, ids = nil, {}
    for index, migration in ipairs(migrations or {}) do
        assert(migration.bundle == bundle, "migration '" .. tostring(migration.id) .. "' has the wrong bundle")
        assert(migration.transactional == true, "migration '" .. tostring(migration.id) .. "' must be transactional")
        assert(not ids[migration.id], "duplicate migration ID '" .. tostring(migration.id) .. "'")
        ids[migration.id] = true
        assert(migration.contentHash == ContentHash(migration), "migration '" .. migration.id .. "' content hash mismatch")
        if index == 1 and expectedParentHash ~= nil then
            assert(migration.parentHash == expectedParentHash, "migration '" .. migration.id .. "' does not start from the empty manifest")
        elseif index > 1 then
            assert(migration.parentHash == previousTarget, "migration '" .. migration.id .. "' does not continue the manifest chain")
        end
        previousTarget = migration.targetHash
    end
    if #migrations > 0 and expectedTargetHash ~= nil then
        assert(previousTarget == expectedTargetHash, "migration chain target does not match the current manifest")
    end
    return true
end

M.ContentHash = ContentHash

return M

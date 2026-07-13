local M = {}
local MAX_VARCHAR_LENGTH = 10485760

local function CopyOptions(options)
    if options == nil then return {} end
    if type(options) ~= "table" then error("PostgreSQL options must be a table") end

    local result = {}
    for key, value in pairs(options) do
        if type(key) ~= "string" then error("PostgreSQL options must use named keys") end
        result[key] = value
    end
    return result
end

local function CopyArray(values)
    if values == nil then return nil end
    if type(values) ~= "table" then error("PostgreSQL field lists must be arrays") end
    local result = {}
    for index, value in pairs(values) do result[index] = value end
    return result
end

local function RequireName(name, description)
    if type(name) ~= "string" or name == "" then
        error((description or "PostgreSQL name") .. " must be a non-empty string")
    end
end

local function PostgresType(name, sql, category, properties)
    local result =
    {
        __metagenPostgresType = true,
        kind = "postgresType",
        name = name,
        sql = sql,
        category = category
    }
    for key, value in pairs(properties or {}) do result[key] = value end
    return result
end

M.SmallInt = PostgresType("smallint", "smallint", "integer",
{
    signed = true,
    bits = 16,
    min = -0x8000,
    max = 0x7FFF
})

M.Integer = PostgresType("integer", "integer", "integer",
{
    signed = true,
    bits = 32,
    min = -0x80000000,
    max = 0x7FFFFFFF
})

M.BigInt = PostgresType("bigint", "bigint", "integer",
{
    signed = true,
    bits = 64,
    min = -0x7FFFFFFFFFFFFFFF - 1,
    max = 0x7FFFFFFFFFFFFFFF
})

M.Real = PostgresType("real", "real", "float", { bits = 32 })
M.DoublePrecision = PostgresType("double precision", "double precision", "float", { bits = 64 })
M.Boolean = PostgresType("boolean", "boolean", "boolean")
M.Text = PostgresType("text", "text", "string")
M.Bytea = PostgresType("bytea", "bytea", "binary")

function M.VarChar(length)
    if type(length) ~= "number" or length < 1 or length > MAX_VARCHAR_LENGTH or length % 1 ~= 0 then
        error("PostgreSQL varchar length must be an integer from 1 through " .. tostring(MAX_VARCHAR_LENGTH))
    end

    return PostgresType("varchar(" .. tostring(length) .. ")", "varchar(" .. tostring(length) .. ")", "string",
    {
        baseName = "varchar",
        length = length
    })
end

M.Varchar = M.VarChar
M.Bool = M.Boolean

function M.IsType(value)
    return type(value) == "table" and value.__metagenPostgresType == true
end

local function SchemaObject(kind, name, fields, options)
    RequireName(name, "PostgreSQL " .. kind .. " name")
    local result = CopyOptions(options)
    result.kind = kind
    result.name = name
    result.fields = CopyArray(fields)
    return result
end

function M.Column(name, postgresType, options)
    RequireName(name, "PostgreSQL column name")
    local result = CopyOptions(options)
    result.kind = "column"
    result.name = name
    result.type = postgresType
    return result
end

function M.PrimaryKey(name, fields, options)
    return SchemaObject("primaryKey", name, fields, options)
end

function M.Unique(name, fields, options)
    return SchemaObject("unique", name, fields, options)
end

function M.Index(name, fields, options)
    return SchemaObject("index", name, fields, options)
end

function M.UniqueIndex(name, fields, options)
    local indexOptions = CopyOptions(options)
    indexOptions.unique = true
    return M.Index(name, fields, indexOptions)
end

function M.Asc(field)
    RequireName(field, "PostgreSQL index field")
    return { kind = "indexColumn", field = field, direction = "asc" }
end

function M.Desc(field)
    RequireName(field, "PostgreSQL index field")
    return { kind = "indexColumn", field = field, direction = "desc" }
end

function M.Parameter(field, semanticType)
    RequireName(field, "PostgreSQL query parameter")
    return { kind = "queryParameter", field = field, type = semanticType }
end

M.Param = M.Parameter

function M.Reference(table, fields, database)
    RequireName(table, "PostgreSQL referenced table")
    return
    {
        kind = "foreignKeyReference",
        table = table,
        fields = CopyArray(fields),
        database = database
    }
end

function M.ForeignKey(name, fields, referencedTable, referencedFields, options)
    if type(referencedTable) == "table" then
        if referencedTable.kind ~= "foreignKeyReference" then
            error("PostgreSQL foreign key table references must be created with Postgres.Reference")
        end
        if options ~= nil then
            error("Postgres.ForeignKey reference form accepts options as its fourth argument")
        end

        options = referencedFields
        referencedFields = referencedTable.fields
        local referenceDatabase = referencedTable.database
        referencedTable = referencedTable.table

        local result = SchemaObject("foreignKey", name, fields, options)
        result.referencedTable = referencedTable
        result.referencedFields = CopyArray(referencedFields)
        result.referencedDatabase = referenceDatabase
        return result
    end

    local result = SchemaObject("foreignKey", name, fields, options)
    result.referencedTable = referencedTable
    result.referencedFields = CopyArray(referencedFields)
    return result
end

function M.Check(name, fields, operator, value, options)
    local result = SchemaObject("check", name, type(fields) == "string" and { fields } or fields, options)
    result.operator = operator
    result.value = value
    return result
end

function M.RawCheck(name, sql, options)
    local result = SchemaObject("rawCheck", name, {}, options)
    result.rawSql = sql
    return result
end

function M.Query(name, fields, options)
    return SchemaObject("query", name, fields, options)
end

function M.Upsert(name, insertFields, conflictFields, updateFields, options)
    local result = SchemaObject("upsert", name, conflictFields, options)
    result.insertFields = CopyArray(insertFields)
    result.updateFields = CopyArray(updateFields)
    return result
end

function M.Function(name, functionName, parameters, options)
    RequireName(functionName, "PostgreSQL function name")
    local result = SchemaObject("function", name, parameters, options)
    result.functionName = functionName
    return result
end

function M.Update(name, keyFields, updateFields, options)
    local result = SchemaObject("update", name, keyFields, options)
    result.updateFields = CopyArray(updateFields)
    return result
end

function M.Delete(name, keyFields, options)
    return SchemaObject("delete", name, keyFields, options)
end

return M

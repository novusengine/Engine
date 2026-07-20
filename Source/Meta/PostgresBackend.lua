local Output = require("Output")
local PostgresModel = require("PostgresModel")
local PostgresSql = require("PostgresSql")
local PostgresManifest = require("PostgresManifest")
local PostgresMigration = require("PostgresMigration")
local PostgresHistory = require("PostgresHistory")
local Sha256 = require("Sha256")

local M = {}

local bundleNames = { auth = "Auth", character = "Character", world = "World", shared = "Shared" }
local cardinalityNames = { exactlyOne = "ExactlyOne", zeroOrOne = "ZeroOrOne", zeroOrMore = "ZeroOrMore" }

local function GetNormalizedModel(model, definitions, context)
    if context ~= nil and context.postgresModel ~= nil and context.postgresSourceModel == model then
        return context.postgresModel
    end

    local normalized = PostgresModel.Build(model, definitions)
    if context ~= nil then
        context.postgresModel = normalized
        context.postgresSourceModel = model
    end
    return normalized
end

local function QuoteIdentifier(identifier)
    return "\"" .. identifier .. "\""
end

local function JoinQuotedColumns(fields)
    local columns = {}
    for _, field in ipairs(fields) do columns[#columns + 1] = QuoteIdentifier(field.columnName) end
    return table.concat(columns, ", ")
end

local function FieldCppType(field, includeNullability)
    local result = field.semanticType.name
    if includeNullability ~= false and field.nullable then result = "std::optional<" .. result .. ">" end
    return result
end

local function TupleType(references, includeNullability)
    local types = {}
    for _, reference in ipairs(references or {}) do
        types[#types + 1] = FieldCppType(reference.field, includeNullability)
    end
    return "std::tuple<" .. table.concat(types, ", ") .. ">"
end

local function KeyType(references)
    if #references == 1 then return FieldCppType(references[1].field, false) end
    return TupleType(references, false)
end

local function InsertFields(tableModel)
    local result = {}
    for _, field in ipairs(tableModel.fields) do
        if field.identity == nil then result[#result + 1] = { field = field } end
    end
    return result
end

local function StorageCppType(field)
    local postgresType = field.postgresType
    if postgresType.category == "integer" then return "i" .. tostring(postgresType.bits) end
    if postgresType.category == "float" then return "f" .. tostring(postgresType.bits) end
    if postgresType.category == "boolean" then return "bool" end
    if postgresType.category == "string" then return "std::string" end
    if postgresType.category == "binary" then return "Bytebuffer" end
    error("No C++ storage conversion for PostgreSQL type '" .. tostring(postgresType.name) .. "'", 0)
end

local function CppLiteral(cpp, field, value)
    if field.semanticType.kind == "string" then return cpp:String(value) end
    if field.semanticType.kind == "boolean" then return value and "true" or "false" end
    return tostring(value)
end

local function FieldDeclaration(cpp, field)
    local declaration = FieldCppType(field) .. " " .. field.name
    if field.nullable then return declaration .. " = std::nullopt" end
    if field.hasDefault then return declaration .. " = " .. CppLiteral(cpp, field, field.defaultValue) end
    if field.semanticType.kind == "integer" or field.semanticType.kind == "float" or field.semanticType.kind == "boolean" then
        return declaration .. " = {}"
    end
    return declaration
end

local function EmitDecodedAssignment(cpp, tableModel, field, index, binaryDecoder)
    local destination = "result." .. field.name
    local rowField = "row[" .. tostring(index - 1) .. "]"
    local storageType = StorageCppType(field)
    local valueName = "value" .. tostring(index - 1)

    if field.postgresType.category == "binary" then
        cpp:Assign(destination, binaryDecoder .. "(" .. rowField .. ")")
    elseif field.postgresType.category == "integer" then
        cpp:Variable("const auto", valueName, rowField .. ".template as<" .. storageType .. ">()")
        if storageType ~= field.semanticType.name then
            cpp:Block("if (!std::in_range<" .. field.semanticType.name .. ">(" .. valueName .. "))", function()
                cpp:Statement("throw std::out_of_range(" ..
                    cpp:String(tableModel.name .. "." .. field.name .. " is outside the C++ field range") .. ")")
            end)
            cpp:Assign(destination, "static_cast<" .. field.semanticType.name .. ">(" .. valueName .. ")")
        else
            cpp:Assign(destination, valueName)
        end
    else
        cpp:Assign(destination, rowField .. ".template as<" .. storageType .. ">()")
    end
end

local function EmitDecode(cpp, tableModel)
    local hasBinary = false
    for _, field in ipairs(tableModel.fields) do hasBinary = hasBinary or field.postgresType.category == "binary" end
    cpp:Line(hasBinary and "template <typename Row, typename DecodeBinary>" or "template <typename Row>")
    local parameters = {{ name = "row", type = "const Row", flags = { ref = true } }}
    if hasBinary then parameters[#parameters + 1] = { name = "decodeBinary", type = "DecodeBinary&&" } end
    cpp:Function(
    {
        name = "Decode",
        returns = "Record",
        parameters = parameters,
        static = true
    }, function()
        cpp:Variable("Record", "result")
        cpp:BlankLine()

        for index, field in ipairs(tableModel.fields) do
            local rowField = "row[" .. tostring(index - 1) .. "]"
            if field.nullable then
                cpp:Block("if (" .. rowField .. ".is_null())", function()
                    cpp:Assign("result." .. field.name, "std::nullopt")
                end)
                cpp:Block("else", function() EmitDecodedAssignment(cpp, tableModel, field, index, "decodeBinary") end)
            else
                cpp:Block("if (" .. rowField .. ".is_null())", function()
                    cpp:Statement("throw std::runtime_error(" ..
                        cpp:String(tableModel.name .. "." .. field.name .. " unexpectedly contains SQL NULL") .. ")")
                end)
                EmitDecodedAssignment(cpp, tableModel, field, index, "decodeBinary")
            end
            if index < #tableModel.fields then cpp:BlankLine() end
        end

        cpp:BlankLine()
        cpp:Return("result")
    end)
end

local function Predicate(fields, firstParameter)
    firstParameter = firstParameter or 1
    local terms = {}
    for index, reference in ipairs(fields) do
        local operator = reference.field.nullable and " IS NOT DISTINCT FROM $" or " = $"
        terms[#terms + 1] = QuoteIdentifier(reference.field.columnName) .. operator .. tostring(firstParameter + index - 1)
    end
    return table.concat(terms, " AND ")
end

local function AssignmentList(fields)
    local assignments = {}
    for index, reference in ipairs(fields) do
        assignments[#assignments + 1] = QuoteIdentifier(reference.field.columnName) .. " = $" .. tostring(index)
    end
    return table.concat(assignments, ", ")
end

local function OrderClause(fields)
    if fields == nil or #fields == 0 then return "" end
    local terms = {}
    for _, reference in ipairs(fields) do
        terms[#terms + 1] = QuoteIdentifier(reference.field.columnName) .. " " .. reference.direction:upper()
    end
    return " ORDER BY " .. table.concat(terms, ", ")
end

local function PreparedName(bundle, persistentId)
    return "metagen_" .. bundle .. "_" .. Sha256.Hash(persistentId):sub(1, 24)
end

local function EmitDecodeForwarder(cpp, tableModel)
    local hasBinary = false
    for _, field in ipairs(tableModel.fields) do hasBinary = hasBinary or field.postgresType.category == "binary" end
    cpp:Line(hasBinary and "template <typename Row, typename DecodeBinary>" or "template <typename Row>")
    local parameters = {{ name = "row", type = "const Row", flags = { ref = true } }}
    if hasBinary then parameters[#parameters + 1] = { name = "decodeBinary", type = "DecodeBinary&&" } end
    cpp:Function({ name = "Decode", returns = "Record", parameters = parameters, static = true }, function()
        local arguments = hasBinary and "row, std::forward<DecodeBinary>(decodeBinary)" or "row"
        cpp:Return("Table::Decode(" .. arguments .. ")")
    end)
end

local function OperationParameters(operation)
    if operation.kind ~= "update" then
        return operation.kind == "upsert" and operation.insertFields or operation.fields
    end
    local result = {}
    for _, reference in ipairs(operation.updateFields) do result[#result + 1] = reference end
    for _, reference in ipairs(operation.fields) do result[#result + 1] = reference end
    return result
end

local function EmitQueryDescriptor(cpp, tableModel, query)
    cpp:Struct(query.name, function()
        cpp:Using("Table", tableModel.name .. "Table")
        cpp:Using("Record", tableModel.name .. "Record")
        cpp:Using("Parameters", TupleType(query.fields))
        EmitDecodeForwarder(cpp, tableModel)
        cpp:Variable("std::string_view", "NAME", cpp:String(query.name), { static = true, constexpr = true })
        cpp:Variable("std::string_view", "PERSISTENT_ID", cpp:String(query.persistentId), { static = true, constexpr = true })
        cpp:Variable("std::string_view", "PREPARED_NAME", cpp:String(PreparedName(tableModel.bundle, query.persistentId)),
            { static = true, constexpr = true })
        cpp:Variable("QueryCardinality", "CARDINALITY", "QueryCardinality::" .. cardinalityNames[query.cardinality],
            { static = true, constexpr = true })
        cpp:Variable("std::string_view", "PREDICATE", cpp:String(Predicate(query.fields)),
            { static = true, constexpr = true })
        cpp:Variable("std::string_view", "SQL", cpp:String("SELECT " .. JoinQuotedColumns(tableModel.fields) ..
            " FROM " .. QuoteIdentifier(tableModel.schema) .. "." .. QuoteIdentifier(tableModel.tableName) ..
            " WHERE " .. Predicate(query.fields) .. OrderClause(query.orderBy)), { static = true, constexpr = true })
    end)
end

local function EmitOperationDescriptor(cpp, tableModel, operation)
    cpp:Struct(operation.name, function()
        cpp:Using("Parameters", TupleType(OperationParameters(operation), false))
        cpp:Variable("std::string_view", "NAME", cpp:String(operation.name), { static = true, constexpr = true })
        cpp:Variable("std::string_view", "PERSISTENT_ID", cpp:String(operation.persistentId), { static = true, constexpr = true })
        cpp:Variable("std::string_view", "PREPARED_NAME", cpp:String(PreparedName(tableModel.bundle, operation.persistentId)),
            { static = true, constexpr = true })
        cpp:Variable("std::string_view", "KIND", cpp:String(operation.kind), { static = true, constexpr = true })
        local firstKeyParameter = operation.kind == "update" and #operation.updateFields + 1 or 1
        if operation.kind ~= "function" and operation.kind ~= "upsert" then
            cpp:Variable("std::string_view", "KEY_PREDICATE", cpp:String(Predicate(operation.fields, firstKeyParameter)),
                { static = true, constexpr = true })
        end
        if operation.kind == "update" then
            cpp:Using("Values", TupleType(operation.updateFields))
            cpp:Variable("std::string_view", "ASSIGNMENTS", cpp:String(AssignmentList(operation.updateFields)),
                { static = true, constexpr = true })
        end
        local qualifiedName = QuoteIdentifier(tableModel.schema) .. "." .. QuoteIdentifier(tableModel.tableName)
        local sql
        if operation.kind == "update" then
            sql = "UPDATE " .. qualifiedName .. " SET " .. AssignmentList(operation.updateFields) ..
                " WHERE " .. Predicate(operation.fields, #operation.updateFields + 1)
        elseif operation.kind == "upsert" then
            local columns, placeholders, updates = {}, {}, {}
            for index, reference in ipairs(operation.insertFields) do
                columns[#columns + 1] = QuoteIdentifier(reference.field.columnName)
                placeholders[#placeholders + 1] = "$" .. tostring(index)
            end
            for _, reference in ipairs(operation.updateFields) do
                updates[#updates + 1] = QuoteIdentifier(reference.field.columnName) .. " = EXCLUDED." .. QuoteIdentifier(reference.field.columnName)
            end
            local conflicts = {}
            for _, reference in ipairs(operation.fields) do conflicts[#conflicts + 1] = QuoteIdentifier(reference.field.columnName) end
            sql = "INSERT INTO " .. qualifiedName .. " (" .. table.concat(columns, ", ") .. ") VALUES (" ..
                table.concat(placeholders, ", ") .. ") ON CONFLICT (" .. table.concat(conflicts, ", ") .. ") " ..
                (#updates == 0 and "DO NOTHING" or "DO UPDATE SET " .. table.concat(updates, ", "))
        elseif operation.kind == "function" then
            local placeholders = {}
            for index = 1, #operation.fields do placeholders[#placeholders + 1] = "$" .. tostring(index) end
            sql = "SELECT " .. QuoteIdentifier(operation.functionName) .. "(" .. table.concat(placeholders, ", ") .. ")"
        else
            sql = "DELETE FROM " .. qualifiedName .. " WHERE " .. Predicate(operation.fields)
        end
        cpp:Variable("std::string_view", "SQL", cpp:String(sql), { static = true, constexpr = true })
    end)
end

local function WriteDatabaseBundle(context, outputPath)
    Output.Write(context, outputPath, function()
        local cpp = context.cpp
        cpp:Line("#pragma once")
        cpp:BlankLine()
        cpp:Block("namespace MetaGen::Postgres", function()
            cpp:Block("enum class DatabaseBundle", function()
                cpp:Line("Auth,")
                cpp:Line("Character,")
                cpp:Line("World,")
                cpp:Line("Shared")
            end, ";")
            cpp:BlankLine()
            cpp:Block("enum class QueryCardinality", function()
                cpp:Line("ExactlyOne,")
                cpp:Line("ZeroOrOne,")
                cpp:Line("ZeroOrMore")
            end, ";")
        end)
    end)
end

local function WriteTable(context, tableModel, outputPath)
    Output.Write(context, outputPath, function()
        local cpp = context.cpp
        local bundleName = bundleNames[tableModel.bundle]

        cpp:Line("#pragma once")
        cpp:BlankLine()
        cpp:Line("#include <Base/Types.h>")
        local hasBytea = false
        for _, field in ipairs(tableModel.fields) do hasBytea = hasBytea or field.postgresType.category == "binary" end
        if hasBytea then cpp:Line("#include <Base/Memory/Bytebuffer.h>") end
        cpp:Line("#include <MetaGen/Postgres/DatabaseBundle.h>")
        cpp:BlankLine()
        cpp:Line("#include <optional>")
        cpp:Line("#include <stdexcept>")
        cpp:Line("#include <string>")
        cpp:Line("#include <string_view>")
        cpp:Line("#include <tuple>")
        cpp:Line("#include <utility>")
        cpp:BlankLine()

        cpp:Block("namespace MetaGen::Postgres::" .. bundleName, function()
            cpp:Struct(tableModel.name .. "Record", function()
                for _, field in ipairs(tableModel.fields) do cpp:Statement(FieldDeclaration(cpp, field)) end
            end)
            cpp:BlankLine()

            cpp:Struct(tableModel.name .. "Table", function()
                cpp:Using("Record", tableModel.name .. "Record")
                if tableModel.primaryKey ~= nil then cpp:Using("PrimaryKey", KeyType(tableModel.primaryKey.fields)) end
                cpp:BlankLine()

                cpp:Variable("DatabaseBundle", "BUNDLE", "DatabaseBundle::" .. bundleName,
                    { static = true, constexpr = true })
                cpp:Variable("std::string_view", "SCHEMA_NAME", cpp:String(tableModel.schema),
                    { static = true, constexpr = true })
                cpp:Variable("std::string_view", "TABLE_NAME", cpp:String(tableModel.tableName),
                    { static = true, constexpr = true })
                cpp:Variable("std::string_view", "PERSISTENT_ID", cpp:String(tableModel.persistentId),
                    { static = true, constexpr = true })
                cpp:Variable("std::string_view", "QUALIFIED_NAME",
                    cpp:String(QuoteIdentifier(tableModel.schema) .. "." .. QuoteIdentifier(tableModel.tableName)),
                    { static = true, constexpr = true })
                cpp:Variable("std::string_view", "SELECT_COLUMNS", cpp:String(JoinQuotedColumns(tableModel.fields)),
                    { static = true, constexpr = true })
                local qualifiedName = QuoteIdentifier(tableModel.schema) .. "." .. QuoteIdentifier(tableModel.tableName)
                cpp:Variable("std::string_view", "SELECT_ALL_SQL",
                    cpp:String("SELECT " .. JoinQuotedColumns(tableModel.fields) .. " FROM " .. qualifiedName .. OrderClause(tableModel.orderBy)),
                    { static = true, constexpr = true })
                if tableModel.primaryKey ~= nil then
                    cpp:Variable("std::string_view", "PRIMARY_KEY_PREDICATE",
                        cpp:String(Predicate(tableModel.primaryKey.fields)), { static = true, constexpr = true })
                    cpp:Variable("std::string_view", "SELECT_BY_PRIMARY_KEY_SQL",
                        cpp:String("SELECT " .. JoinQuotedColumns(tableModel.fields) .. " FROM " .. qualifiedName ..
                            " WHERE " .. Predicate(tableModel.primaryKey.fields)), { static = true, constexpr = true })
                end
                local insertFields = InsertFields(tableModel)
                cpp:Using("InsertParameters", TupleType(insertFields))
                local insertColumns, placeholders = {}, {}
                for index, reference in ipairs(insertFields) do
                    insertColumns[#insertColumns + 1] = QuoteIdentifier(reference.field.columnName)
                    placeholders[#placeholders + 1] = "$" .. tostring(index)
                end
                local insertSql
                if #insertFields == 0 then
                    insertSql = "INSERT INTO " .. qualifiedName .. " DEFAULT VALUES"
                else
                    insertSql = "INSERT INTO " .. qualifiedName .. " (" .. table.concat(insertColumns, ", ") .. ") VALUES (" ..
                        table.concat(placeholders, ", ") .. ")"
                end
                cpp:Variable("std::string_view", "INSERT_SQL",
                    cpp:String(insertSql .. " RETURNING " .. JoinQuotedColumns(tableModel.fields)),
                    { static = true, constexpr = true })
                cpp:BlankLine()
                EmitDecode(cpp, tableModel)

                cpp:BlankLine()
                cpp:Struct("Insert", function()
                    cpp:Using("Table", tableModel.name .. "Table")
                    cpp:Using("Record", tableModel.name .. "Record")
                    cpp:Using("Parameters", "InsertParameters")
                    EmitDecodeForwarder(cpp, tableModel)
                    local persistentId = tableModel.persistentId .. ".insert"
                    cpp:Variable("std::string_view", "NAME", cpp:String("Insert"), { static = true, constexpr = true })
                    cpp:Variable("std::string_view", "PERSISTENT_ID", cpp:String(persistentId), { static = true, constexpr = true })
                    cpp:Variable("std::string_view", "PREPARED_NAME", cpp:String(PreparedName(tableModel.bundle, persistentId)),
                        { static = true, constexpr = true })
                    cpp:Variable("QueryCardinality", "CARDINALITY", "QueryCardinality::ExactlyOne", { static = true, constexpr = true })
                    cpp:Variable("std::string_view", "SQL", "INSERT_SQL", { static = true, constexpr = true })
                end)
                if tableModel.primaryKey ~= nil then
                    cpp:BlankLine()
                    cpp:Struct("ByPrimaryKey", function()
                        cpp:Using("Table", tableModel.name .. "Table")
                        cpp:Using("Record", tableModel.name .. "Record")
                        cpp:Using("Parameters", TupleType(tableModel.primaryKey.fields, false))
                        EmitDecodeForwarder(cpp, tableModel)
                        local persistentId = tableModel.persistentId .. ".by_primary_key"
                        cpp:Variable("std::string_view", "NAME", cpp:String("ByPrimaryKey"), { static = true, constexpr = true })
                        cpp:Variable("std::string_view", "PERSISTENT_ID", cpp:String(persistentId), { static = true, constexpr = true })
                        cpp:Variable("std::string_view", "PREPARED_NAME", cpp:String(PreparedName(tableModel.bundle, persistentId)),
                            { static = true, constexpr = true })
                        cpp:Variable("QueryCardinality", "CARDINALITY", "QueryCardinality::ZeroOrOne", { static = true, constexpr = true })
                        cpp:Variable("std::string_view", "SQL", "SELECT_BY_PRIMARY_KEY_SQL", { static = true, constexpr = true })
                    end)
                end

                for _, query in ipairs(tableModel.queries) do
                    cpp:BlankLine()
                    EmitQueryDescriptor(cpp, tableModel, query)
                end
                for _, operation in ipairs(tableModel.operations) do
                    cpp:BlankLine()
                    EmitOperationDescriptor(cpp, tableModel, operation)
                end
            end)
        end)
    end)
end

local function WriteSchema(context, bundle, bootstrapSql, manifest, migrations, outputPath)
    Output.Write(context, outputPath, function()
        local cpp = context.cpp
        local bundleName = bundleNames[bundle.name]

        cpp:Line("#pragma once")
        cpp:BlankLine()
        cpp:Line("#include <MetaGen/Postgres/DatabaseBundle.h>")
        for _, tableModel in ipairs(bundle.tables) do
            cpp:Line("#include <MetaGen/Postgres/" .. bundleName .. "/Tables/" .. tableModel.name .. ".h>")
        end
        cpp:BlankLine()
        cpp:Line("#include <cstddef>")
        cpp:Line("#include <string_view>")
        cpp:Line("#include <tuple>")
        cpp:BlankLine()

        cpp:Block("namespace MetaGen::Postgres::" .. bundleName, function()
            cpp:Struct(bundleName .. "Schema", function()
                local tables = {}
                for _, tableModel in ipairs(bundle.tables) do tables[#tables + 1] = tableModel.name .. "Table" end
                cpp:Variable("DatabaseBundle", "BUNDLE", "DatabaseBundle::" .. bundleName,
                    { static = true, constexpr = true })
                cpp:Variable("std::size_t", "MANIFEST_VERSION", tostring(manifest.format), { static = true, constexpr = true })
                cpp:Variable("std::string_view", "MANIFEST_HASH", cpp:String(manifest.hash),
                    { static = true, constexpr = true })
                cpp:Variable("std::string_view", "BOOTSTRAP_CONTENT_HASH", cpp:String(Sha256.Hash(bootstrapSql)),
                    { static = true, constexpr = true })
                cpp:Variable("std::string_view", "BOOTSTRAP_SQL", cpp:String(bootstrapSql),
                    { static = true, constexpr = true })
                cpp:Using("Tables", "std::tuple<" .. table.concat(tables, ", ") .. ">")
                local preparedStatements = {}
                for _, tableModel in ipairs(bundle.tables) do
                    preparedStatements[#preparedStatements + 1] = tableModel.name .. "Table::Insert"
                    if tableModel.primaryKey ~= nil then
                        preparedStatements[#preparedStatements + 1] = tableModel.name .. "Table::ByPrimaryKey"
                    end
                    for _, query in ipairs(tableModel.queries) do
                        preparedStatements[#preparedStatements + 1] = tableModel.name .. "Table::" .. query.name
                    end
                    for _, operation in ipairs(tableModel.operations) do
                        preparedStatements[#preparedStatements + 1] = tableModel.name .. "Table::" .. operation.name
                    end
                end
                cpp:Using("PreparedStatements", "std::tuple<" .. table.concat(preparedStatements, ", ") .. ">")
                for index, migration in ipairs(migrations) do
                    cpp:BlankLine()
                    cpp:Struct("Migration" .. tostring(index), function()
                        cpp:Variable("std::string_view", "ID", cpp:String(migration.id), { static = true, constexpr = true })
                        cpp:Variable("std::string_view", "NAME", cpp:String(migration.name), { static = true, constexpr = true })
                        cpp:Variable("std::string_view", "PARENT_HASH", cpp:String(migration.parentHash), { static = true, constexpr = true })
                        cpp:Variable("std::string_view", "TARGET_HASH", cpp:String(migration.targetHash), { static = true, constexpr = true })
                        cpp:Variable("std::string_view", "CONTENT_HASH", cpp:String(migration.contentHash), { static = true, constexpr = true })
                        cpp:Variable("bool", "TRANSACTIONAL", "true", { static = true, constexpr = true })
                        cpp:Variable("std::string_view", "SQL", cpp:String(migration.sql), { static = true, constexpr = true })
                    end)
                end
                local migrationTypes = {}
                for index = 1, #migrations do migrationTypes[#migrationTypes + 1] = "Migration" .. tostring(index) end
                cpp:Using("Migrations", "std::tuple<" .. table.concat(migrationTypes, ", ") .. ">")
            end)
        end)
    end)
end

function M.Validate(model, definitions, context)
    local normalized = GetNormalizedModel(model, definitions, context)
    if context ~= nil and context.postgresHistoryByBundle ~= nil then
        context.postgresMigrations = context.postgresMigrations or {}
        for _, bundle in ipairs(normalized.bundles) do
            local historyRoot = context.postgresHistoryByBundle[bundle.name]
            if historyRoot == nil then error("PostgreSQL bundle '" .. bundle.name .. "' has no configured history owner", 0) end
            local history = PostgresHistory.LoadBundleRoot(historyRoot, bundle.name)
            if history == nil then
                error("PostgreSQL bundle '" .. bundle.name .. "' has no committed manifest; create its baseline migration", 0)
            end
            local current = PostgresManifest.Build(bundle)
            if current.hash ~= history.manifest.hash then
                error("PostgreSQL bundle '" .. bundle.name .. "' differs from its committed manifest; create and commit a migration", 0)
            end
            context.postgresMigrations[bundle.name] = history.migrations
        end
    end
    return normalized
end

function M.PlanOutputs(model, definitions, context)
    local normalized = GetNormalizedModel(model, definitions, context)
    local outputs = { "Postgres/DatabaseBundle.h" }
    for _, bundle in ipairs(normalized.bundles) do
        local bundleName = bundleNames[bundle.name]
        outputs[#outputs + 1] = "Postgres/" .. bundleName .. "/Bootstrap.sql"
        outputs[#outputs + 1] = "Postgres/" .. bundleName .. "/Manifest.canonical"
        outputs[#outputs + 1] = "Postgres/" .. bundleName .. "/Schema.h"
        for _, tableModel in ipairs(bundle.tables) do
            outputs[#outputs + 1] = "Postgres/" .. bundleName .. "/Tables/" .. tableModel.name .. ".h"
        end
    end
    table.sort(outputs)
    return outputs
end

function M.Emit(model, definitions, context)
    if context == nil or context.writer == nil or context.cpp == nil then
        error("MetaGen PostgreSQL backend requires a writer and C++ emitter", 0)
    end
    if type(context.stagingRootDir) ~= "string" then
        error("MetaGen PostgreSQL backend requires a staging output root", 0)
    end

    local normalized = GetNormalizedModel(model, definitions, context)
    local makeDirectory = context.makeDirectory or os.mkdir
    local postgresRoot = context.stagingRootDir .. "/Postgres"
    makeDirectory(postgresRoot)
    WriteDatabaseBundle(context, postgresRoot .. "/DatabaseBundle.h")

    for _, bundle in ipairs(normalized.bundles) do
        local bundleName = bundleNames[bundle.name]
        local bundleRoot = postgresRoot .. "/" .. bundleName
        local tablesRoot = bundleRoot .. "/Tables"
        makeDirectory(bundleRoot)
        makeDirectory(tablesRoot)
        for _, tableModel in ipairs(bundle.tables) do
            WriteTable(context, tableModel, tablesRoot .. "/" .. tableModel.name .. ".h")
        end
        local bootstrapSql = PostgresSql.GenerateBundle(bundle)
        local manifest = PostgresManifest.Build(bundle)
        local migrations = context.postgresMigrations and context.postgresMigrations[bundle.name] or {}
        PostgresMigration.ValidateChain(bundle.name, migrations, #migrations > 0 and manifest.hash or nil)
        Output.WriteText(context, bundleRoot .. "/Bootstrap.sql", bootstrapSql)
        Output.WriteText(context, bundleRoot .. "/Manifest.canonical", manifest.canonical)
        WriteSchema(context, bundle, bootstrapSql, manifest, migrations, bundleRoot .. "/Schema.h")
    end
end

return M

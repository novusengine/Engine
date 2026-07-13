local info = debug.getinfo(1, "S")
local scriptPath = info.source:sub(2)
local root = scriptPath:match("(.*/)")
package.path = root .. "?.lua;" .. package.path

require("OrderedTable")

local Type = require("Type")
local D = require("Definition")
local Model = require("Model")
local CppEmitter = require("CppEmitter")
local Postgres = require("Postgres")
local PostgresBackend = require("PostgresBackend")
local PostgresModel = require("PostgresModel")
local PostgresManifest = require("PostgresManifest")
local PostgresMigration = require("PostgresMigration")
local PostgresHistory = require("PostgresHistory")
local Sha256 = require("Sha256")
local TestSuite = require("TestSuite")

local function Source(definitions)
    return
    {
        path = "PostgresTests.lua",
        namespace = "MetaGen::PostgresTests",
        defines = definitions
    }
end

local function ExpectFailure(name, expectedText, callback)
    local succeeded, message = pcall(callback)
    if succeeded then error("Expected test '" .. name .. "' to fail") end
    if tostring(message):find(expectedText, 1, true) == nil then
        error("Test '" .. name .. "' failed with unexpected message: " .. tostring(message))
    end
end

local function Build(definitions)
    local model = Model.BuildFromSources({ Source(definitions) })
    local postgresDefinitions = model.definitionsByTarget.postgres or {}
    return model, PostgresModel.Build(model, postgresDefinitions)
end

local function EmitInMemory(model, postgresMigrations)
    local generation = CppEmitter.Create()
    local outputs = {}
    local context =
    {
        stagingRootDir = "memory",
        binRootDir = "MetaGen",
        namespaceBase = "MetaGen",
        writer = generation.writer,
        cpp = generation.cpp,
        postgresMigrations = postgresMigrations,
        makeDirectory = function() end,
        openOutput = function(path)
            local chunks = {}
            outputs[path] = chunks
            return
            {
                write = function(_, value) chunks[#chunks + 1] = value end,
                close = function() end
            }
        end
    }

    PostgresBackend.Validate(model, model.definitionsByTarget.postgres, context)
    local plan = PostgresBackend.PlanOutputs(model, model.definitionsByTarget.postgres, context)
    PostgresBackend.Emit(model, model.definitionsByTarget.postgres, context)

    local textOutputs = {}
    for path, chunks in pairs(outputs) do textOutputs[path] = table.concat(chunks) end
    return textOutputs, plan
end

local function AssertContains(text, expected)
    assert(text:find(expected, 1, true), "Missing exact generated text:\n" .. expected)
end

local function ValidDefinitions()
    local accountIdColumn = Postgres.Column("id", Postgres.BigInt, { nullable = false, identity = "byDefault" })
    local characterIdColumn = Postgres.Column("id", Postgres.BigInt, { nullable = false, identity = "byDefault" })

    return D.Definitions
    {
        D.DatabaseTable("Characters",
        {
            D.Field("id", Type.U64,
            {
                persistentId = "postgres.characters.id",
                postgres = characterIdColumn
            }),
            D.Field("accountID", Type.U64,
            {
                persistentId = "postgres.characters.account",
                postgres = Postgres.Column("account_id", Postgres.BigInt, { nullable = false })
            }),
            D.Field("name", Type.STRING,
            {
                persistentId = "postgres.characters.name",
                postgres = Postgres.Column("name", Postgres.Text, { nullable = false })
            }),
            D.Field("level", Type.U8,
            {
                persistentId = "postgres.characters.level",
                default = 1,
                postgres = Postgres.Column("level", Postgres.SmallInt, { nullable = false })
            })
        },
        {
            persistentId = "postgres.characters",
            database = "character",
            schema = "public",
            table = "characters",
            primaryKey = Postgres.PrimaryKey("characters_pk", { "id" }, { persistentId = "postgres.characters.pk" }),
            constraints =
            {
                Postgres.ForeignKey("characters_account_fk", { "accountID" },
                    Postgres.Reference("Accounts", { "id" }),
                    { persistentId = "postgres.characters.account_fk" }),
                Postgres.Unique("characters_name_key", { "name" }, { persistentId = "postgres.characters.name_key" }),
                Postgres.Check("characters_level_check", "level", "greaterEqual", 0,
                { persistentId = "postgres.characters.level_check" })
            },
            indexes =
            {
                Postgres.Index("characters_account_name_idx", { Postgres.Asc("accountID"), Postgres.Desc("name") },
                { persistentId = "postgres.characters.account_name_index" })
            },
            queries =
            {
                Postgres.Query("ByName", { Postgres.Parameter("name", Type.STRING) },
                { persistentId = "postgres.characters.by_name", cardinality = "exactlyOne", orderBy = { Postgres.Desc("id") } })
            },
            orderBy = { Postgres.Asc("accountID"), Postgres.Desc("name") },
            operations =
            {
                Postgres.Update("SetLevel", { "id" }, { "level" },
                { persistentId = "postgres.characters.set_level" }),
                Postgres.Delete("Delete", { "id" },
                { persistentId = "postgres.characters.delete" }),
                Postgres.Delete("DeleteByAccount", { "accountID" },
                { persistentId = "postgres.characters.delete_by_account", cardinality = "zeroOrMore" }),
                Postgres.Upsert("SetByName", { "accountID", "name", "level" }, { "name" }, { "level" },
                { persistentId = "postgres.characters.set_by_name" }),
                Postgres.Function("TouchPair", "touch_pair", { "id", "id" },
                { persistentId = "postgres.characters.touch_pair" })
            }
        }),

        D.DatabaseTable("Accounts",
        {
            D.Field("id", Type.U64,
            {
                persistentId = "postgres.accounts.id",
                postgres = accountIdColumn
            }),
            D.Field("name", Type.STRING,
            {
                persistentId = "postgres.accounts.name",
                default = "O'Reilly",
                postgres = Postgres.Column("name", Postgres.Text, { nullable = false })
            })
        },
        {
            persistentId = "postgres.accounts",
            database = "character",
            schema = "public",
            table = "accounts",
            primaryKey = Postgres.PrimaryKey("accounts_pk", { "id" }, { persistentId = "postgres.accounts.pk" })
        })
    }
end

local function Run()
    assert(Sha256.Hash("") == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")
    assert(Sha256.Hash("abc") == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad")
    local validModel, postgresModel = Build(ValidDefinitions())
    assert(#postgresModel.tables == 2)
    assert(postgresModel.tables[1].tableName == "accounts")
    assert(postgresModel.tables[2].tableName == "characters")
    assert(postgresModel.tables[2].fields[1].rangeRestriction.max == Postgres.BigInt.max)
    assert(postgresModel.tables[2].constraints[1].target.tableName == "accounts")
    assert(postgresModel.tables[2].queries[1].cardinality == "exactlyOne")
    assert(postgresModel.tables[2].operations[1].kind == "update")
    local _, secondPostgresModel = Build(ValidDefinitions())
    assert(postgresModel.canonical == secondPostgresModel.canonical)
    Model.AssertUnchanged(validModel, "PostgreSQL model validation")
    local baselineDiff = PostgresMigration.Diff(PostgresManifest.Empty("character"), postgresModel.bundles[1])
    assert(#baselineDiff.hazards == 0)
    assert(baselineDiff.operations[1].kind == "createSchema")
    local accountsCreate, charactersCreate
    for index, operation in ipairs(baselineDiff.operations) do
        if operation.kind == "createTable" and operation.persistentId == "postgres.accounts" then accountsCreate = index end
        if operation.kind == "createTable" and operation.persistentId == "postgres.characters" then charactersCreate = index end
    end
    assert(accountsCreate < charactersCreate)
    local baselineArtifact = PostgresMigration.BuildArtifact(baselineDiff, "0001_baseline", "baseline")
    assert(PostgresMigration.ValidateChain("character", { baselineArtifact }, baselineDiff.targetHash,
        PostgresManifest.Empty("character").hash))

    local function MigrationDefinitions(columnName, columnType, nullable, includeColumn)
        local fields =
        {
            D.Field("id", Type.I32,
            { persistentId = "postgres.migration.id", postgres = Postgres.Column("id", Postgres.Integer, { nullable = false }) })
        }
        if includeColumn ~= false then
            fields[#fields + 1] = D.Field("value", columnType == Postgres.BigInt and Type.I64 or Type.I32,
            { persistentId = "postgres.migration.value", postgres = Postgres.Column(columnName, columnType, { nullable = nullable }) })
        end
        return D.Definitions { D.DatabaseTable("MigrationRows", fields,
        { persistentId = "postgres.migration", database = "shared", schema = "public", table = "migration_rows" }) }
    end

    local _, oldMigrationModel = Build(MigrationDefinitions("value", Postgres.Integer, true))
    local renamedSourceModel, renamedMigrationModel = Build(MigrationDefinitions("renamed_value", Postgres.BigInt, true))
    local migrationDiff = PostgresMigration.Diff(PostgresManifest.Build(oldMigrationModel.bundles[1]), renamedMigrationModel.bundles[1])
    assert(#migrationDiff.hazards == 0)
    assert(#migrationDiff.operations == 2)
    assert(migrationDiff.operations[1].kind == "renameColumn")
    assert(migrationDiff.operations[2].kind == "widenColumn")
    local artifact = PostgresMigration.BuildArtifact(migrationDiff, "0002_rename_and_widen", "rename and widen value")
    AssertContains(artifact.sql, "RENAME COLUMN \"value\" TO \"renamed_value\";")
    AssertContains(artifact.sql, "ALTER COLUMN \"renamed_value\" TYPE bigint;")
    assert(#artifact.contentHash == 64)
    assert(artifact.contentHash == PostgresMigration.BuildArtifact(migrationDiff,
        "0002_rename_and_widen", "rename and widen value").contentHash)
    assert(PostgresMigration.ValidateChain("shared", { artifact }, migrationDiff.targetHash))
    local manifestSource = PostgresHistory.ManifestSource(migrationDiff.targetManifest)
    assert(not manifestSource:find("model =", 1, true))
    local loader = loadstring or load
    local loadedManifest = assert(loader(manifestSource))()
    assert(PostgresManifest.Validate(loadedManifest))
    assert(loadedManifest.hash == migrationDiff.targetHash)
    local alteredManifest = assert(loader(manifestSource))()
    alteredManifest.tables["postgres.migration"].tableName = "silently_edited"
    ExpectFailure("edited committed manifest", "manifest hash mismatch", function()
        PostgresManifest.Validate(alteredManifest)
    end)
    local migrationOutputs = EmitInMemory(renamedSourceModel, { shared = { artifact } })
    local migrationSchema = migrationOutputs["memory/Postgres/Shared/Schema.h"]
    AssertContains(migrationSchema, "struct Migration1")
    AssertContains(migrationSchema, "static constexpr std::string_view ID = \"0002_rename_and_widen\";")
    AssertContains(migrationSchema, "static constexpr bool TRANSACTIONAL = true;")
    AssertContains(migrationSchema, "using Migrations = std::tuple<Migration1>;")
    local editedArtifact = {}
    for key, value in pairs(artifact) do editedArtifact[key] = value end
    editedArtifact.sql = editedArtifact.sql .. "-- edited\n"
    ExpectFailure("edited committed migration", "content hash mismatch", function()
        PostgresMigration.ValidateChain("shared", { editedArtifact }, migrationDiff.targetHash)
    end)

    local _, droppedMigrationModel = Build(MigrationDefinitions("value", Postgres.Integer, true, false))
    local dropDiff = PostgresMigration.Diff(PostgresManifest.Build(oldMigrationModel.bundles[1]), droppedMigrationModel.bundles[1])
    assert(#dropDiff.hazards == 1 and dropDiff.hazards[1].kind == "dropColumn")
    ExpectFailure("unresolved migration hazard", "unresolved hazards", function()
        PostgresMigration.BuildArtifact(dropDiff, "0002_drop", "drop value")
    end)

    local _, notNullMigrationModel = Build(MigrationDefinitions("value", Postgres.Integer, false))
    local notNullDiff = PostgresMigration.Diff(PostgresManifest.Build(oldMigrationModel.bundles[1]), notNullMigrationModel.bundles[1])
    assert(#notNullDiff.hazards == 1 and notNullDiff.hazards[1].kind == "makeNotNull")

    local context = {}
    PostgresBackend.Validate(validModel, validModel.definitionsByTarget.postgres, context)
    assert(context.postgresModel ~= nil)

    local firstOutputs, firstPlan = EmitInMemory(validModel)
    local secondOutputs, secondPlan = EmitInMemory(validModel)
    assert(table.concat(firstPlan, "|") == table.concat(secondPlan, "|"))
    assert(table.concat(firstPlan, "|") ==
        "Postgres/Character/Bootstrap.sql|Postgres/Character/Manifest.canonical|Postgres/Character/Schema.h|Postgres/Character/Tables/Accounts.h|Postgres/Character/Tables/Characters.h|Postgres/DatabaseBundle.h")
    for path, output in pairs(firstOutputs) do assert(output == secondOutputs[path], "Repeated PostgreSQL emission changed " .. path) end

    local charactersHeader = firstOutputs["memory/Postgres/Character/Tables/Characters.h"]
    AssertContains(charactersHeader, "struct CharactersRecord")
    AssertContains(charactersHeader, "u64 id = {};")
    AssertContains(charactersHeader, "u8 level = 1;")
    AssertContains(charactersHeader, "static constexpr std::string_view SELECT_COLUMNS = \"\\\"id\\\", \\\"account_id\\\", \\\"name\\\", \\\"level\\\"\";")
    AssertContains(charactersHeader, "FROM \\\"public\\\".\\\"characters\\\" ORDER BY \\\"account_id\\\" ASC, \\\"name\\\" DESC")
    AssertContains(charactersHeader, "using InsertParameters = std::tuple<u64, std::string, u8>;")
    AssertContains(charactersHeader, "struct Insert")
    AssertContains(charactersHeader, "static constexpr QueryCardinality CARDINALITY = QueryCardinality::ExactlyOne;")
    AssertContains(charactersHeader, "static constexpr std::string_view SQL = INSERT_SQL;")
    AssertContains(charactersHeader, "struct ByPrimaryKey")
    AssertContains(charactersHeader, "static constexpr QueryCardinality CARDINALITY = QueryCardinality::ZeroOrOne;")
    AssertContains(charactersHeader, "static constexpr std::string_view SQL = SELECT_BY_PRIMARY_KEY_SQL;")
    AssertContains(charactersHeader, "INSERT INTO \\\"public\\\".\\\"characters\\\" (\\\"account_id\\\", \\\"name\\\", \\\"level\\\") VALUES ($1, $2, $3) RETURNING")
    AssertContains(charactersHeader, "if (!std::in_range<u64>(value0))")
    AssertContains(charactersHeader, "if (row[0].is_null())")
    AssertContains(charactersHeader, "using Parameters = std::tuple<std::string>;")
    AssertContains(charactersHeader, "using Table = CharactersTable;")
    AssertContains(charactersHeader, "using Record = CharactersRecord;")
    AssertContains(charactersHeader, "return Table::Decode(row);")
    AssertContains(charactersHeader, "using Parameters = std::tuple<u8, u64>;")
    AssertContains(charactersHeader, "static constexpr QueryCardinality CARDINALITY = QueryCardinality::ExactlyOne;")
    AssertContains(charactersHeader, "static constexpr std::string_view ASSIGNMENTS = \"\\\"level\\\" = $1\";")
    AssertContains(charactersHeader, "static constexpr std::string_view KEY_PREDICATE = \"\\\"id\\\" = $2\";")
    AssertContains(charactersHeader, "static constexpr std::string_view PERSISTENT_ID = \"postgres.characters.by_name\";")
    AssertContains(charactersHeader, "static constexpr std::string_view PREPARED_NAME = \"metagen_character_")
    AssertContains(charactersHeader, "static constexpr std::string_view SQL = \"SELECT ")
    AssertContains(charactersHeader, "WHERE \\\"name\\\" = $1 ORDER BY \\\"id\\\" DESC")
    AssertContains(charactersHeader, "DELETE FROM \\\"public\\\".\\\"characters\\\" WHERE \\\"account_id\\\" = $1")
    AssertContains(charactersHeader, "ON CONFLICT (\\\"name\\\") DO UPDATE SET \\\"level\\\" = EXCLUDED.\\\"level\\\"")
    AssertContains(charactersHeader, "SELECT \\\"touch_pair\\\"($1, $2)")
    assert(not charactersHeader:find("pqxx", 1, true))

    local schemaHeader = firstOutputs["memory/Postgres/Character/Schema.h"]
    AssertContains(schemaHeader, "using Tables = std::tuple<AccountsTable, CharactersTable>;")
    AssertContains(schemaHeader, "static constexpr std::string_view BOOTSTRAP_SQL = ")
    AssertContains(schemaHeader, "static constexpr std::size_t MANIFEST_VERSION = 3;")
    AssertContains(schemaHeader, "static constexpr std::string_view MANIFEST_HASH = \"")
    AssertContains(schemaHeader, "static constexpr std::string_view BOOTSTRAP_CONTENT_HASH = \"")
    AssertContains(schemaHeader, "AccountsTable::Insert, AccountsTable::ByPrimaryKey")
    AssertContains(schemaHeader, "CharactersTable::Insert, CharactersTable::ByPrimaryKey, CharactersTable::ByName")
    local manifest = firstOutputs["memory/Postgres/Character/Manifest.canonical"]
    AssertContains(manifest, "metagen-postgres-manifest-v3")
    AssertContains(schemaHeader, Sha256.Hash(manifest))

    local bootstrapSql = firstOutputs["memory/Postgres/Character/Bootstrap.sql"]
    AssertContains(bootstrapSql, "BEGIN;\nCREATE SCHEMA IF NOT EXISTS \"public\";")
    AssertContains(bootstrapSql, "CREATE TABLE \"public\".\"accounts\"")
    AssertContains(bootstrapSql, "\"id\" bigint GENERATED BY DEFAULT AS IDENTITY NOT NULL")
    AssertContains(bootstrapSql, "\"name\" text DEFAULT 'O''Reilly' NOT NULL")
    AssertContains(bootstrapSql, "CONSTRAINT \"accounts_pk\" PRIMARY KEY (\"id\")")
    AssertContains(bootstrapSql, "CREATE TABLE \"public\".\"characters\"")
    AssertContains(bootstrapSql, "CONSTRAINT \"characters_account_fk\" FOREIGN KEY (\"account_id\") REFERENCES \"public\".\"accounts\" (\"id\") ON UPDATE NO ACTION ON DELETE NO ACTION")
    AssertContains(bootstrapSql, "CONSTRAINT \"characters_level_check\" CHECK (\"level\" >= 0)")
    AssertContains(bootstrapSql, "CREATE INDEX \"characters_account_name_idx\" ON \"public\".\"characters\" (\"account_id\" ASC, \"name\" DESC);")
    AssertContains(bootstrapSql, "\nCOMMIT;\n")
    assert(bootstrapSql:find("CREATE TABLE \"public\".\"accounts\"", 1, true) <
        bootstrapSql:find("CREATE TABLE \"public\".\"characters\"", 1, true))
    assert(not bootstrapSql:find("SELECT *", 1, true))
    assert(not bootstrapSql:find("OWNER", 1, true))
    assert(not bootstrapSql:find("TABLESPACE", 1, true))

    local compositeModel = Model.BuildFromSources({ Source(D.Definitions
    {
        D.DatabaseTable("CompositeRows",
        {
            D.Field("tenantID", Type.U32,
            {
                persistentId = "postgres.composite.tenant",
                postgres = Postgres.Column("tenant_id", Postgres.BigInt, { nullable = false })
            }),
            D.Field("id", Type.I32,
            {
                persistentId = "postgres.composite.id",
                postgres = Postgres.Column("id", Postgres.Integer, { nullable = false })
            }),
            D.Field("note", Type.STRING,
            {
                persistentId = "postgres.composite.note",
                postgres = Postgres.Column("note", Postgres.Text, { nullable = true })
            })
        },
        {
            persistentId = "postgres.composite",
            database = "shared",
            schema = "public",
            table = "composite_rows",
            primaryKey = Postgres.PrimaryKey("composite_rows_pk", { "tenantID", "id" },
            { persistentId = "postgres.composite.pk" }),
            queries =
            {
                Postgres.Query("ByNote", { Postgres.Parameter("note", Type.STRING) },
                { persistentId = "postgres.composite.by_note", cardinality = "zeroOrMore" })
            }
        })
    }) })
    local compositeOutputs = EmitInMemory(compositeModel)
    local compositeHeader = compositeOutputs["memory/Postgres/Shared/Tables/CompositeRows.h"]
    AssertContains(compositeHeader, "std::optional<std::string> note = std::nullopt;")
    AssertContains(compositeHeader, "using PrimaryKey = std::tuple<u32, i32>;")
    AssertContains(compositeHeader, "if (row[2].is_null())")
    AssertContains(compositeHeader, "using Parameters = std::tuple<std::optional<std::string>>;")
    AssertContains(compositeHeader, "static constexpr std::string_view PREDICATE = \"\\\"note\\\" IS NOT DISTINCT FROM $1\";")

    local binaryModel = Model.BuildFromSources({ Source(D.Definitions
    {
        D.DatabaseTable("BinaryRows",
        {
            D.Field("id", Type.I32,
            { persistentId = "postgres.binary.id", postgres = Postgres.Column("id", Postgres.Integer, { nullable = false }) }),
            D.Field("payload", Type.BYTEBUFFER,
            { persistentId = "postgres.binary.payload", postgres = Postgres.Column("payload", Postgres.Bytea, { nullable = true }) })
        },
        { persistentId = "postgres.binary", database = "shared", schema = "public", table = "binary_rows" })
    }) })
    local binaryOutputs = EmitInMemory(binaryModel)
    local binaryHeader = binaryOutputs["memory/Postgres/Shared/Tables/BinaryRows.h"]
    AssertContains(binaryHeader, "template <typename Row, typename DecodeBinary>")
    AssertContains(binaryHeader, "static Record Decode(const Row& row, DecodeBinary&& decodeBinary)")
    AssertContains(binaryHeader, "result.payload = decodeBinary(row[1]);")
    assert(not binaryHeader:find("as<Bytebuffer>", 1, true))

    local copiedColumn = Postgres.Column("value", Postgres.Integer, { nullable = false })
    local copiedDefinitions = D.Definitions
    {
        D.DatabaseTable("CopiedOptions",
        {
            D.Field("value", Type.I32,
            {
                persistentId = "postgres.copied.value",
                postgres = copiedColumn
            })
        },
        {
            persistentId = "postgres.copied",
            database = "shared",
            schema = "public",
            table = "copied_options"
        })
    }
    local copiedModel = Model.BuildFromSources({ Source(copiedDefinitions) })
    assert(copiedModel.definitions[1].fields[1].attributes.postgres ~= copiedColumn)
    assert(copiedModel.definitions[1].fields[1].attributes.postgres.type == Postgres.Integer)
    copiedColumn.nullable = true
    assert(copiedModel.definitions[1].fields[1].attributes.postgres.nullable == false)

    local _, keywordModel = Build(D.Definitions
    {
        D.DatabaseTable("KeywordIdentifiers",
        {
            D.Field("value", Type.I32,
            { persistentId = "postgres.keyword.value", postgres = Postgres.Column("from", Postgres.Integer, { nullable = false }) })
        },
        { persistentId = "postgres.keyword", database = "shared", schema = "select", table = "table" })
    })
    assert(keywordModel.tables[1].schema == "select" and keywordModel.tables[1].tableName == "table")

    ExpectFailure("invalid PostgreSQL identifier", "not a valid PostgreSQL identifier", function()
        Build(D.Definitions
        {
            D.DatabaseTable("InvalidIdentifier",
            {
                D.Field("id", Type.I32,
                { persistentId = "postgres.invalid.id", postgres = Postgres.Column("id", Postgres.Integer, { nullable = false }) })
            },
            { persistentId = "postgres.invalid", database = "shared", schema = "bad-name", table = "invalid_identifier" })
        })
    end)

    ExpectFailure("varchar length", "varchar length must be an integer", function()
        Postgres.VarChar(10485761)
    end)

    ExpectFailure("varchar default", "exceeds varchar(3)", function()
        Build(D.Definitions
        {
            D.DatabaseTable("InvalidVarcharDefault",
            {
                D.Field("value", Type.STRING,
                {
                    persistentId = "postgres.varchar.value",
                    default = "four",
                    postgres = Postgres.Column("value", Postgres.VarChar(3), { nullable = false })
                })
            },
            { persistentId = "postgres.varchar", database = "shared", schema = "public", table = "invalid_varchar_default" })
        })
    end)

    ExpectFailure("unknown table option", "unsupported PostgreSQL option 'databse'", function()
        Build(D.Definitions
        {
            D.DatabaseTable("UnknownTableOption",
            {
                D.Field("id", Type.I32,
                { persistentId = "postgres.unknown_table.id", postgres = Postgres.Column("id", Postgres.Integer, { nullable = false }) })
            },
            {
                persistentId = "postgres.unknown_table",
                database = "shared",
                databse = "shared",
                schema = "public",
                table = "unknown_table_option"
            })
        })
    end)

    ExpectFailure("unknown index option", "unsupported option 'uniquely'", function()
        Build(D.Definitions
        {
            D.DatabaseTable("UnknownIndexOption",
            {
                D.Field("id", Type.I32,
                { persistentId = "postgres.unknown_index.id", postgres = Postgres.Column("id", Postgres.Integer, { nullable = false }) })
            },
            {
                persistentId = "postgres.unknown_index",
                database = "shared",
                schema = "public",
                table = "unknown_index_option",
                indexes =
                {
                    Postgres.Index("unknown_index_option_idx", { "id" },
                    { persistentId = "postgres.unknown_index.idx", uniquely = true })
                }
            })
        })
    end)

    ExpectFailure("sparse key fields", "must not contain holes", function()
        Build(D.Definitions
        {
            D.DatabaseTable("SparseKey",
            {
                D.Field("id", Type.I32,
                { persistentId = "postgres.sparse_key.id", postgres = Postgres.Column("id", Postgres.Integer, { nullable = false }) })
            },
            {
                persistentId = "postgres.sparse_key",
                database = "shared",
                schema = "public",
                table = "sparse_key",
                primaryKey = Postgres.PrimaryKey("sparse_key_pk", { [1] = "id", [3] = "id" },
                { persistentId = "postgres.sparse_key.pk" })
            })
        })
    end)

    ExpectFailure("relation name collision", "conflicts with table", function()
        Build(D.Definitions
        {
            D.DatabaseTable("RelationCollision",
            {
                D.Field("id", Type.I32,
                { persistentId = "postgres.relation_collision.id", postgres = Postgres.Column("id", Postgres.Integer, { nullable = false }) })
            },
            {
                persistentId = "postgres.relation_collision",
                database = "shared",
                schema = "public",
                table = "relation_collision",
                indexes =
                {
                    Postgres.Index("relation_collision", { "id" },
                    { persistentId = "postgres.relation_collision.idx" })
                }
            })
        })
    end)

    ExpectFailure("cross-bundle foreign key", "across logical database bundles", function()
        Build(D.Definitions
        {
            D.DatabaseTable("Account",
            {
                D.Field("id", Type.I32,
                { persistentId = "postgres.cross.account.id", postgres = Postgres.Column("id", Postgres.Integer, { nullable = false }) })
            },
            { persistentId = "postgres.cross.account", database = "auth", schema = "public", table = "accounts", primaryKey = Postgres.PrimaryKey("account_pk", { "id" }, { persistentId = "postgres.cross.account.pk" }) }),
            D.DatabaseTable("Character",
            {
                D.Field("id", Type.I32,
                { persistentId = "postgres.cross.character.id", postgres = Postgres.Column("id", Postgres.Integer, { nullable = false }) }),
                D.Field("accountID", Type.I32,
                { persistentId = "postgres.cross.character.account", postgres = Postgres.Column("account_id", Postgres.Integer, { nullable = false }) })
            },
            {
                persistentId = "postgres.cross.character", database = "character", schema = "public", table = "characters",
                primaryKey = Postgres.PrimaryKey("character_pk", { "id" }, { persistentId = "postgres.cross.character.pk" }),
                constraints = { Postgres.ForeignKey("character_account_fk", { "accountID" }, "Account", { "id" }, { persistentId = "postgres.cross.character.fk" }) }
            })
        })
    end)

    ExpectFailure("incompatible PostgreSQL type", "does not fit PostgreSQL type", function()
        Build(D.Definitions
        {
            D.DatabaseTable("TooSmall",
            {
                D.Field("value", Type.U64,
                { persistentId = "postgres.small.value", postgres = Postgres.Column("value", Postgres.Integer, { nullable = false }) })
            },
            { persistentId = "postgres.small", database = "shared", schema = "public", table = "too_small" })
        })
    end)

    print("MetaGen PostgreSQL model tests passed")
end

TestSuite.Register("postgres", Run)

newaction
{
    trigger = "metagen-postgres-tests",
    description = "Run MetaGen PostgreSQL model validation tests",
    execute = Run
}

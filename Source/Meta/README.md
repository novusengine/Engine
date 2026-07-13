# MetaGen

MetaGen loads ordered Lua definitions into a validated model before any output backend writes files.

## Definition form

Definition files use the constructor API:

```lua
local Type = require("Type")
local D = require("Definition")

return D.Definitions
{
    D.Packet("ClientExamplePacket",
    {
        D.Field("value", Type.U32)
    }),

    D.Enum("ExampleEnum", Type.U8,
    {
        "Invalid",
        "Active",
        "Count"
    })
}
```

Specialized enum domains use `D.LuaEnum` and `D.NetField`, avoiding direct archetype overrides in definition files.

`D.Definitions` converts the constructor list to the internal ordered representation, preserving declaration order. Handwritten `OrderedTable` definition maps are intentionally rejected so every definition passes through the same authoring API.

## Target participation

Components validate every field they consume. A field can explicitly opt out of a composable target:

```lua
D.Field("runtimeValues", Type.VECTOR,
{
    type = Type.U32,
    serialize = false,
    luaPush = false
})
```

Omitting an opt-out while using an unsupported type is a validation error.

Definitions also declare backend participation through `targets`. Existing archetypes default to C++:

```lua
D.DatabaseTable("AccountTable", fields,
{
    persistentId = "game.account",
    targets = { postgres = true }
})
```

Component validation hooks run only when their target is enabled.
Database tables and each of their fields require globally unique `persistentId` values so schema and migration tooling can survive source-level renames.
The `postgres` target emits validated pqxx-neutral C++ records/descriptors and deterministic PostgreSQL bootstrap SQL.

Model construction validates backend-neutral structure, identity, ordering, and semantic values. Backend-specific rules—such as C++ identifiers, keywords, and component capabilities—run during that backend's `Validate` phase. A PostgreSQL-only definition is therefore not constrained by C++ naming rules.

## Model tests

The model validation tests are a standalone Premake action and do not build a solution:

```text
premake5 --file=Source/Meta/ModelTests.lua metagen-tests
```

## Backend boundary

`Model.lua` is the shared semantic boundary for the current C++ generator and future backends. `Backend.lua` owns registration, target support validation, and deterministic backend dispatch; `CppBackend.lua` owns the existing C++ emission while `Init.lua` retains cache, staging, and output replacement orchestration. PostgreSQL schema generation should consume the same model and supply its own type mappings, identity policy, dependency ordering, and validation hooks.

Every backend implements `Validate`, `PlanOutputs`, and `Emit`. Output plans use safe relative paths, are sorted deterministically, cannot overlap between backends, and are verified against staged files before cache creation. The cache hashes the planned output set rather than assuming a file extension.

The C++ backend resets all component-owned counters and collection state before each emission, so repeated generation in one Lua process starts from the same IDs and metadata inputs. Output files are emitted through a scoped writer binding that always closes the file and restores writer state after success or failure; tests replace that opener with in-memory sinks for exact output assertions.
Backend callbacks receive an explicit generation context containing the C++ emitter, hash function, path ordering, and output roots. `CppEmitter.lua` creates isolated writer/emitter pairs. Component validation and C++ emission consume normalized model fields; validated models no longer retain raw authoring tables or expose the former compatibility callback view.

### C++ emission API

Components use the statement-oriented emitter available as `context.cpp`. Complete constructs are emitted with `Function`, `Struct`, `Block`, `Variable`, `Using`, `Assign`, `Return`, `Statement`, `Line`, and `BlankLine`. Pure helpers such as `String`, `Type`, `Template`, `Call`, `CallMember`, `CallPointer`, `Member`, `Index`, `Cast`, and `InitList` construct expressions without hidden mutable state. Component code does not manage token buffers or call flush/end operations.

Type callbacks make their contract explicit: `SerializeExpr`, `DeserializeExpr`, `SerializedSizeExpr`, and `CommandReadExpr` return complete C++ expressions, while `EmitLuaPush` emits complete statements through `context.cpp`. Unusual syntax can be passed deliberately as a complete expression or line; there is no ambiguous `Unknown` token operation.
Normalized field attributes and aliases are copied away from authoring tables. A semantic fingerprint is checked after each individual backend validation, planning, and emission callback, so a mutating backend fails before the next backend can observe altered shared state.

## PostgreSQL schemas and migrations

`Postgres.lua` provides typed column, key, constraint, index, query, and operation descriptors. `PostgresModel.lua` validates them and builds a deterministic model ordered by logical bundle, schema, table dependencies, columns, constraints, indexes, queries, and operations.

The four logical bundles are `auth`, `character`, `world`, and `shared`. A table belongs to exactly one bundle. Cross-bundle foreign keys and generated cross-bundle queries are forbidden even when multiple bundles currently use the same physical PostgreSQL database.

### Definition example

```lua
local D = require("Definition")
local P = require("Postgres")
local Type = require("Type")

return D.Definitions
{
    D.DatabaseTable("Characters",
    {
        D.Field("id", Type.U64,
        {
            persistentId = "postgres.characters.id",
            postgres = P.Column("id", P.BigInt,
            {
                nullable = false,
                identity = "byDefault"
            })
        }),

        D.Field("name", Type.STRING,
        {
            persistentId = "postgres.characters.name",
            postgres = P.Column("name", P.VarChar(12), { nullable = false })
        })
    },
    {
        persistentId = "postgres.characters",
        database = "character",
        schema = "public",
        table = "characters",
        primaryKey = P.PrimaryKey("characters_pkey", { "id" },
        {
            persistentId = "postgres.characters.pk"
        })
    })
}
```

### Persistent identity rules

- Every table, column, key, constraint, index, query, update, and delete descriptor requires a globally unique `persistentId`.
- A persistent ID is durable production identity, not a display name. Do not derive its continued value from a new SQL or C++ name during a rename.
- To rename an object, change its SQL/C++ name and preserve its existing persistent ID. Changing both makes the migration system see a drop plus an addition.
- Never reuse a removed persistent ID for a different object.
- Never edit an already applied migration, its ID, SQL, parent hash, target hash, or content hash. Create a forward repair migration instead.
- Table ownership (`auth`, `character`, `world`, or `shared`) is durable. Moving a table between bundles is not an ordinary rename and requires an explicitly reviewed transition.

### Definition rules

- Use typed `P.Column`, `P.PrimaryKey`, `P.Unique`, `P.ForeignKey`, `P.Check`, `P.RawCheck`, `P.Index`, `P.Query`, `P.Update`, and `P.Delete` constructors. Do not place arbitrary SQL in ordinary schema fields.
- SQL identifiers must be valid PostgreSQL identifiers no longer than 63 bytes. Generated SQL always quotes them.
- Nullable fields generate `std::optional<T>`. Non-null fields generate `T`.
- Defaults must be representable by both the semantic C++ type and PostgreSQL storage type. Identity columns cannot also declare defaults.
- `exactlyOne` and `zeroOrOne` queries must use a non-null primary or unique key. Use `zeroOrMore` when the predicate can legally return any number of rows.
- Update and delete descriptors must use a non-null primary or unique key. Updates cannot modify their own key fields.
- Foreign-key field counts and storage types must match, and foreign keys must stay inside one logical bundle.
- Preserve column order deliberately. Reordering existing columns is treated as hazardous because migrated and newly bootstrapped databases must not silently acquire different physical layouts.
- Raw SQL is limited to `P.RawCheck` and explicit migration SQL. Prefer typed checks when possible.

### Normal MetaGen generation

Ordinary generation is read-only with respect to committed schema history. It:

1. Loads and validates definitions.
2. Loads `PostgresHistory/<bundle>/Manifest.lua` and its ordered `Migrations/*.lua` files.
3. Verifies manifest SHA-256 identity, migration content hashes, unique IDs, parent/target continuity, and the final target hash.
4. Refuses generation when definitions differ from the committed manifest.
5. Emits pqxx-neutral records, table descriptors, insert/select/query/update/delete SQL, deterministic prepared names, bootstrap DDL, manifest metadata, and ordered embedded migrations.

Do not hand-edit generated `MetaGen/Postgres` output. Change definitions and author a migration instead.

### Creating a baseline

Creating the first history for a populated bundle requires an explicit `--baseline` flag:

```text
premake5 --file=Source/Meta/PostgresMigrationAction.lua metagen-migration --database=character --name=baseline --baseline
```

If the baseline needs functions or seed data, provide reviewed SQL as well:

```text
premake5 --file=Source/Meta/PostgresMigrationAction.lua metagen-migration --database=character --name=baseline --baseline --sql=baseline_character.sql
```

`--baseline` is rejected after a bundle already has committed history. An existing database must be catalog-validated before its baseline migration is recorded as applied; never silently baseline an unknown schema.

### Safe automatic migration

After changing definitions, run:

```text
premake5 --file=Source/Meta/PostgresMigrationAction.lua metagen-migration --database=character --name=add_character_last_played
```

MetaGen can automatically author:

- New schemas and tables.
- Nullable columns.
- Required columns with valid defaults or identities.
- Table and column renames whose persistent IDs are preserved.
- Lossless integer/float widening and varchar widening.
- Removal of `NOT NULL`.
- Ordinary indexes.

The action validates the complete resulting chain, stages the whole bundle history, and replaces it atomically. Review both the migration operations and final SQL before committing them.

### Hazardous or data-dependent migration

Drops, narrowing, primary-key changes, identity/default changes, nullable-to-required changes, column reordering, constraint changes over existing data, and similar operations stop with diagnostics and write nothing. Supply reviewed SQL that resolves every reported hazard:

```text
premake5 --file=Source/Meta/PostgresMigrationAction.lua metagen-migration --database=character --name=remove_legacy_flags --sql=remove_legacy_flags.sql
```

Example reviewed SQL:

```sql
DO $$
BEGIN
    IF EXISTS (SELECT 1 FROM public.characters WHERE legacy_flags <> 0) THEN
        RAISE EXCEPTION 'legacy_flags contains data that must be migrated';
    END IF;
END;
$$;

ALTER TABLE public.characters DROP COLUMN legacy_flags;
```

Explicit SQL rules:

- Do not include outer `BEGIN`, `COMMIT`, or `ROLLBACK`; MetaGen wraps the complete migration in one transaction.
- `BEGIN` inside a dollar-quoted PL/pgSQL function body is allowed.
- `CREATE INDEX CONCURRENTLY` and other non-transactional operations are not supported in V1.
- The file must contain an executable statement; an empty/comment-only file cannot resolve hazards.
- SQL executes after automatically generated safe operations in the same transaction.
- The same `--sql` form can create function-only, seed-only, or data-repair migrations when table metadata does not change.
- A failed action must leave the previously committed manifest and migration directory intact.

### Review and recovery rules

- Commit definition changes, the updated manifest, and the new migration together.
- Never commit definition drift without its migration.
- Never rewrite migration history to make a test pass or repair production. Add a new forward migration.
- There are no down migrations. Rollback means restoring a database backup or deploying a reviewed forward repair.
- Destructive changes require an appropriate backup and deployment plan outside MetaGen.
- Generated migration SQL never owns databases, roles, tablespaces, or deployment credentials.

### Generated Engine boundary

Engine-generated headers do not include pqxx. Scalar row fields use the row field's typed `as<T>()` interface. Tables containing `bytea` receive a second `Decode` argument; Server-Common supplies the pqxx-specific callable that returns a `Bytebuffer`.

Built-in insert and primary-key lookup descriptors, declared queries, and declared operations expose persistent IDs, deterministic prepared names, complete SQL, and parameter tuples. Row-returning descriptors also expose record decoding and cardinality. All descriptors, including inserts and primary-key lookups, are included in the bundle's `PreparedStatements` tuple. Bundle schema descriptors additionally expose ordered `Tables` and `Migrations` tuples plus the current manifest hash and bootstrap SQL. Applying migrations, maintaining the ledger, locking, catalog inspection, binary binding, and connection initialization belong to Server-Common.

### Tests

Run the focused model/emitter suite with:

```text
premake5 --file=Source/Meta/PostgresTests.lua metagen-postgres-tests
```

Run production definition and history validation with:

```text
premake5 --file=Source/Meta/PostgresDefinitionTests.lua metagen-postgres-definition-tests
```

Run every MetaGen suite through the shared entry point:

```text
premake5 --file=Source/Meta/Tests.lua metagen-all-tests
```

The legacy files under `Servers/Resources/Database` are reference material for the initial definitions and reviewed seed/function SQL. The Engine baseline intentionally uses logical bundle ownership, identity columns, and no cross-bundle foreign keys. Existing legacy installations therefore require explicit catalog validation and transition handling before being marked as baselined.

## Compatibility boundaries

The on-disk cache has an explicit `MGC1` format marker and incompatible cache data is discarded safely. C++ artifacts remain governed by exact generation tests. PostgreSQL manifests and migrations use independently versioned canonical formats and SHA-256 identities; they do not reuse the generator cache hash.

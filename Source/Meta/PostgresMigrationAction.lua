local info = debug.getinfo(1, "S")
local scriptPath = info.source:sub(2):gsub("\\", "/")
local root = assert(scriptPath:match("(.*/)"))
package.path = root .. "?.lua;" .. package.path

require("OrderedTable")
local Model = require("Model")
local PostgresHistory = require("PostgresHistory")
local PostgresManifest = require("PostgresManifest")
local PostgresMigration = require("PostgresMigration")
local PostgresModel = require("PostgresModel")

newoption { trigger = "database", value = "BUNDLE", description = "PostgreSQL bundle: auth, character, world, or shared" }
newoption { trigger = "name", value = "NAME", description = "Human-readable migration name" }
newoption { trigger = "sql", value = "PATH", description = "Reviewed SQL that explicitly handles reported hazards" }
newoption { trigger = "baseline", description = "Explicitly create the first migration for a bundle" }

local function ReadFile(path)
    local file = assert(io.open(path, "rb"), "failed to open '" .. path .. "'")
    local value = file:read("*a")
    file:close()
    return value
end

local function WriteFile(path, value)
    local file = assert(io.open(path, "wb"), "failed to open '" .. path .. "' for writing")
    local succeeded, message = pcall(function() file:write(value) end)
    local closed, closeError = file:close()
    if not succeeded then error(message, 0) end
    if closed == nil then error("failed to close '" .. path .. "': " .. tostring(closeError), 0) end
end

local function CopyTree(source, destination)
    if not os.isdir(source) then return end
    for _, path in ipairs(os.matchfiles(source .. "/**")) do
        local relative = path:sub(#source + 2):gsub("\\", "/")
        local directory = relative:match("(.+)/[^/]+$")
        if directory then os.mkdir(destination .. "/" .. directory) end
        WriteFile(destination .. "/" .. relative, ReadFile(path))
    end
end

local function Slug(value)
    local result = value:lower():gsub("[^a-z0-9]+", "_"):gsub("^_+", ""):gsub("_+$", "")
    if result == "" then error("migration name must contain a letter or digit", 0) end
    return result
end

local function ScrubSql(sql)
    local result, index = {}, 1
    while index <= #sql do
        local two = sql:sub(index, index + 1)
        local character = sql:sub(index, index)
        if two == "--" then
            local finish = sql:find("\n", index + 2, true) or (#sql + 1)
            result[#result + 1] = " "
            index = finish
        elseif two == "/*" then
            local finish = sql:find("*/", index + 2, true)
            if finish == nil then error("explicit migration SQL has an unterminated block comment", 0) end
            result[#result + 1] = " "
            index = finish + 2
        elseif character == "'" or character == '"' then
            local quote, finish = character, index + 1
            while finish <= #sql do
                if sql:sub(finish, finish) == quote then
                    if sql:sub(finish + 1, finish + 1) == quote then finish = finish + 2 else break end
                else
                    finish = finish + 1
                end
            end
            if finish > #sql then error("explicit migration SQL has an unterminated quoted value", 0) end
            result[#result + 1] = " "
            index = finish + 1
        elseif character == "$" then
            local remaining = sql:sub(index)
            local tag = remaining:match("^(%$[A-Za-z_][A-Za-z0-9_]*%$)") or remaining:match("^(%$%$)")
            if tag ~= nil then
                local finish = sql:find(tag, index + #tag, true)
                if finish == nil then error("explicit migration SQL has an unterminated dollar-quoted body", 0) end
                result[#result + 1] = " "
                index = finish + #tag
            else
                result[#result + 1] = character
                index = index + 1
            end
        else
            result[#result + 1] = character
            index = index + 1
        end
    end
    return table.concat(result)
end

local function ExplicitSql(path)
    local sql = ReadFile(path)
    local upper = ScrubSql(sql):upper()
    if upper:match("^%s*$") then error("explicit migration SQL must contain an executable statement", 0) end
    if upper:find("CREATE%s+INDEX%s+CONCURRENTLY") then error("non-transactional CREATE INDEX CONCURRENTLY is not supported", 0) end
    if upper:find("%f[%a]BEGIN%f[%A]") or upper:find("%f[%a]COMMIT%f[%A]") or upper:find("%f[%a]ROLLBACK%f[%A]") then
        error("explicit migration SQL must not contain transaction control", 0)
    end
    return sql
end

local function Execute()
    local bundleName, name = _OPTIONS.database, _OPTIONS.name
    if not ({ auth = true, character = true, world = true, shared = true })[bundleName] then
        error("--database must be auth, character, world, or shared", 0)
    end
    if type(name) ~= "string" or name == "" then error("--name is required", 0) end

    if MetaGen == nil then error("MetaGen project profile is not configured", 0) end
    local profile = MetaGen.ResolveProject()
    for _, provider in ipairs(profile.providers) do
        for _, moduleRoot in ipairs(provider.moduleRoots) do
            if not os.isdir(moduleRoot) then
                error("MetaGen provider '" .. provider.name .. "' module root does not exist: " .. moduleRoot, 0)
            end
            package.path = moduleRoot .. "/?.lua;" .. moduleRoot .. "/?/Init.lua;" .. package.path
        end
    end

    local sourceSets = {}
    for _, provider in ipairs(profile.providers) do
        for _, source in ipairs(provider.sources) do
            local files = os.matchfiles(source.root .. "/**.lua")
            table.sort(files)
            table.insert(sourceSets, { files = files, root = source.root, namespace = source.namespace })
        end
    end
    local model = Model.LoadSourceSets(sourceSets)
    local normalized = PostgresModel.Build(model, model.definitionsByTarget.postgres or {})
    local bundle
    for _, candidate in ipairs(normalized.bundles) do if candidate.name == bundleName then bundle = candidate end end
    if bundle == nil then error("bundle '" .. bundleName .. "' has no PostgreSQL definitions", 0) end

    if profile.postgres == nil or type(profile.postgres.historyByBundle) ~= "table" then
        error("active MetaGen project does not configure PostgreSQL history ownership", 0)
    end
    local bundleRoot = profile.postgres.historyByBundle[bundleName]
    if type(bundleRoot) ~= "string" then error("bundle '" .. bundleName .. "' has no configured history owner", 0) end
    os.mkdir(bundleRoot)
    local history = PostgresHistory.LoadBundleRoot(bundleRoot, bundleName)
    if history == nil and not _OPTIONS.baseline then
        error("bundle '" .. bundleName .. "' has no history; pass --baseline to create its first migration", 0)
    elseif history ~= nil and _OPTIONS.baseline then
        error("bundle '" .. bundleName .. "' already has a baseline", 0)
    end
    local parent = history and history.manifest or PostgresManifest.Empty(bundleName)
    local migrations = history and history.migrations or {}
    local diff = PostgresMigration.Diff(parent, bundle)
    if #diff.operations == 0 and #diff.hazards == 0 and _OPTIONS.sql == nil then
        error("bundle '" .. bundleName .. "' has no schema or explicit SQL changes", 0)
    end

    if #diff.hazards > 0 then
        if _OPTIONS.sql == nil then
            local messages = { "migration has unresolved hazards; provide reviewed handling with --sql:" }
            for _, hazard in ipairs(diff.hazards) do messages[#messages + 1] = "  " .. hazard.kind .. " " .. hazard.persistentId .. ": " .. hazard.message end
            error(table.concat(messages, "\n"), 0)
        end
        diff.operations[#diff.operations + 1] =
        {
            kind = "explicitSql",
            persistentId = "explicit." .. Slug(name),
            sql = ExplicitSql(_OPTIONS.sql)
        }
        diff.hazards = {}
    elseif _OPTIONS.sql ~= nil then
        diff.operations[#diff.operations + 1] =
        { kind = "explicitSql", persistentId = "explicit." .. Slug(name), sql = ExplicitSql(_OPTIONS.sql) }
    end

    local migrationId = string.format("%04d_%s", #migrations + 1, Slug(name))
    local migration = PostgresMigration.BuildArtifact(diff, migrationId, name)
    migrations[#migrations + 1] = migration
    PostgresMigration.ValidateChain(bundleName, migrations, diff.targetHash)

    local historyParent = path.getdirectory(bundleRoot)
    local stagingRoot = historyParent .. "/." .. bundleName .. ".staging"
    local backupRoot = historyParent .. "/." .. bundleName .. ".previous"
    os.rmdir(stagingRoot)
    os.rmdir(backupRoot)
    os.mkdir(stagingRoot)
    os.mkdir(stagingRoot .. "/Migrations")
    CopyTree(bundleRoot, stagingRoot)
    WriteFile(stagingRoot .. "/Manifest.lua", PostgresHistory.ManifestSource(diff.targetManifest))
    WriteFile(stagingRoot .. "/Migrations/" .. migrationId .. ".lua", PostgresHistory.MigrationSource(migration))

    local hadHistory = os.isdir(bundleRoot)
    if hadHistory then
        local moved, moveError = os.rename(bundleRoot, backupRoot)
        if not moved then error("failed to preserve existing PostgreSQL history: " .. tostring(moveError), 0) end
    end
    local installed, installError = os.rename(stagingRoot, bundleRoot)
    if not installed then
        if hadHistory then
            local restored, restoreError = os.rename(backupRoot, bundleRoot)
            if not restored then
                error("failed to install PostgreSQL history (" .. tostring(installError) ..
                    ") and failed to restore the previous history (" .. tostring(restoreError) .. ")", 0)
            end
        end
        error("failed to install PostgreSQL migration history: " .. tostring(installError), 0)
    end
    os.rmdir(backupRoot)
    print("Created PostgreSQL migration " .. migrationId .. " for bundle " .. bundleName)
end

newaction
{
    trigger = "metagen-migration",
    description = "Create a reviewed PostgreSQL migration and update its committed manifest",
    execute = Execute
}

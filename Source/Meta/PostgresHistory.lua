local PostgresMigration = require("PostgresMigration")
local PostgresManifest = require("PostgresManifest")

local M = {}

local function SortedKeys(value)
    local keys = {}
    for key in pairs(value) do if key ~= "model" then keys[#keys + 1] = key end end
    table.sort(keys, function(left, right)
        if type(left) == type(right) then return left < right end
        return type(left) < type(right)
    end)
    return keys
end

local function Serialize(value, indent)
    local kind = type(value)
    if kind == "nil" then return "nil" end
    if kind == "boolean" or kind == "number" then return tostring(value) end
    if kind == "string" then return string.format("%q", value) end
    assert(kind == "table", "cannot serialize PostgreSQL history value of type " .. kind)
    indent = indent or ""
    local childIndent, lines = indent .. "    ", { "{" }
    for _, key in ipairs(SortedKeys(value)) do
        local keyText = type(key) == "string" and key:match("^[A-Za-z_][A-Za-z0-9_]*$") and key or "[" .. Serialize(key) .. "]"
        lines[#lines + 1] = childIndent .. keyText .. " = " .. Serialize(value[key], childIndent) .. ","
    end
    lines[#lines + 1] = indent .. "}"
    return table.concat(lines, "\n")
end

local function ReadLua(path)
    local probe = io.open(path, "rb")
    if probe == nil then return nil end
    probe:close()
    local chunk, loadError = loadfile(path)
    if chunk == nil then error("failed to parse PostgreSQL history '" .. path .. "': " .. tostring(loadError), 0) end
    local succeeded, result = pcall(chunk)
    if not succeeded then error("failed to load PostgreSQL history '" .. path .. "': " .. tostring(result), 0) end
    if type(result) ~= "table" then error("PostgreSQL history '" .. path .. "' must return a table", 0) end
    return result
end

function M.Serialize(value)
    return "-- MetaGen PostgreSQL history. Review and commit this file.\nreturn " .. Serialize(value) .. "\n"
end

function M.LoadBundleRoot(bundleRoot, bundle)
    local manifest = ReadLua(bundleRoot .. "/Manifest.lua")
    if manifest == nil then return nil end
    if manifest.format ~= 3 or manifest.bundle ~= bundle or type(manifest.hash) ~= "string" or type(manifest.tables) ~= "table" then
        error("invalid committed PostgreSQL manifest for bundle '" .. bundle .. "'", 0)
    end
    PostgresManifest.Validate(manifest)
    local migrations = {}
    for _, path in ipairs(os.matchfiles(bundleRoot .. "/Migrations/*.lua")) do
        local migration = assert(ReadLua(path))
        local normalizedPath = path:gsub("\\", "/")
        local fileId = normalizedPath:match("/([^/]+)%.lua$")
        if fileId ~= migration.id then error("migration filename does not match ID '" .. tostring(migration.id) .. "'", 0) end
        migrations[#migrations + 1] = migration
    end
    table.sort(migrations, function(left, right) return left.id < right.id end)
    if #migrations == 0 then error("PostgreSQL history for bundle '" .. bundle .. "' has no baseline migration", 0) end
    PostgresMigration.ValidateChain(bundle, migrations, manifest.hash, PostgresManifest.Empty(bundle).hash)
    return { manifest = manifest, migrations = migrations }
end

function M.LoadBundle(root, bundle)
    return M.LoadBundleRoot(root .. "/" .. bundle, bundle)
end

function M.ManifestSource(manifest)
    local persisted = {}
    for key, value in pairs(manifest) do
        if key ~= "model" and key ~= "canonical" then persisted[key] = value end
    end
    return M.Serialize(persisted)
end

function M.MigrationSource(migration)
    return M.Serialize(migration)
end

return M

local _ = require("OrderedTable")
local Model = require("Model")
local Backend = require("Backend")
local CppBackend = require("CppBackend")
local PostgresBackend = require("PostgresBackend")
local CppEmitter = require("CppEmitter")
local profile = MetaGen.ResolveProject()

for _, provider in ipairs(profile.providers) do
    for _, moduleRoot in ipairs(provider.moduleRoots) do
        if not os.isdir(moduleRoot) then error("MetaGen provider '" .. provider.name .. "' module root does not exist: " .. moduleRoot, 0) end
        package.path = moduleRoot .. "/?.lua;" .. moduleRoot .. "/?/Init.lua;" .. package.path
    end
end

local info = debug.getinfo(1,'S')
local scriptPath = info.source:sub(2)
local root = scriptPath:match("(.*/)")

-- This is restored to the original inside of Meta.lua which includes this file
package.path = root .. "?.lua;" .. root .. "?/Init.lua;" .. package.path

-- Migration authoring must compare changed definitions with the committed history
-- before ordinary generation rejects that intentional drift.
if _ACTION == "metagen-migration" then
    return
end

local cppGeneration = CppEmitter.Create()
local cppWriter = cppGeneration.writer
local cppEmitter = cppGeneration.cpp

local CACHE_MAGIC = "MGC1"

local function GetProfileIdentity(value)
    local parts = { value.name, value.outputRoot }
    for _, provider in ipairs(value.providers) do
        table.insert(parts, provider.name)
        for _, dependency in ipairs(provider.dependencies) do table.insert(parts, "dep:" .. dependency) end
        for _, source in ipairs(provider.sources) do
            table.insert(parts, "source:" .. source.root:gsub("\\", "/") .. "|" .. source.namespace)
        end
        for _, moduleRoot in ipairs(provider.moduleRoots) do table.insert(parts, "module:" .. moduleRoot:gsub("\\", "/")) end
        for _, extension in ipairs(provider.extensions) do table.insert(parts, "extension:" .. extension:gsub("\\", "/")) end
        for _, input in ipairs(provider.inputs) do table.insert(parts, "input:" .. input:gsub("\\", "/")) end
    end
    if value.postgres ~= nil and value.postgres.historyByBundle ~= nil then
        local bundles = {}
        for bundle in pairs(value.postgres.historyByBundle) do table.insert(bundles, bundle) end
        table.sort(bundles)
        for _, bundle in ipairs(bundles) do
            table.insert(parts, "postgres-history:" .. bundle .. "=" .. value.postgres.historyByBundle[bundle]:gsub("\\", "/"))
        end
    end
    return table.concat(parts, "\n")
end

local profileIdentity = GetProfileIdentity(profile)

local function WriteBinaryFile(path, callback)
    local file = assert(io.open(path, "wb"))
    local succeeded, result = xpcall(function() callback(file) end, debug.traceback)
    local closed, closeError = pcall(function() file:close() end)
    if not succeeded then error(result, 0) end
    if not closed then error("MetaGen failed to close '" .. path .. "': " .. tostring(closeError), 0) end
end

local function GetFileHash(path)
    local file = io.open(path, "rb")
    if file == nil then return nil end

    local hash = 2166136261
    local prime = 16777619
    local uintMask = 0xFFFFFFFF

    local succeeded, readError = xpcall(function()
        while true do
            local data = file:read(64 * 1024)
            if data == nil then break end

            for i = 1, #data do
                hash = ((hash ~ string.byte(data, i)) * prime) & uintMask
            end
        end
    end, debug.traceback)

    local closed, closeError = pcall(function() file:close() end)
    if not succeeded then error("MetaGen failed to hash '" .. path .. "': " .. tostring(readError), 0) end
    if not closed then error("MetaGen failed to close '" .. path .. "': " .. tostring(closeError), 0) end
    return hash
end

local function GetCacheFile(binRoot, expectedProfileIdentity)
    local cacheFile =
    {
        files = {},
        outputs = {}
    }
    
    local path = binRoot .. "/Cache.mcache"
    local file = io.open(path, "rb")
    if file == nil then return cacheFile end

    local succeeded = pcall(function()
        if file:read(#CACHE_MAGIC) ~= CACHE_MAGIC then
            error("Unsupported MetaGen cache version")
        end
        local profileLength = string.unpack("<I4", file:read(4))
        if file:read(profileLength) ~= expectedProfileIdentity then
            error("MetaGen profile changed")
        end

        local function ReadEntries(destination)
            local numEntries = string.unpack("<I4", file:read(4))
            for _ = 1, numEntries do
                local hash = string.unpack("<I4", file:read(4))
                local pathLength = string.unpack("<I4", file:read(4))
                local entryPath = file:read(pathLength)

                destination[entryPath] = hash
            end
        end

        ReadEntries(cacheFile.files)
        ReadEntries(cacheFile.outputs)
    end)

    file:close()

    if not succeeded then
        return { files = {}, outputs = {} }
    end

    return cacheFile
end

local function FindDefinitionFiles(root)
    local pattern = root .. "/**.lua"
    return os.matchfiles(pattern)
end

function PathCompareSort(a, b)
    local aa = {}
    for part in a:gmatch("[^/]+") do
        aa[#aa + 1] = part:lower()
    end

    local bb = {}
    for part in b:gmatch("[^/]+") do
        bb[#bb + 1] = part:lower()
    end

    -- Compare directory levels only (ignore filename)
    local adepth = #aa - 1
    local bdepth = #bb - 1

    -- Compare each directory level alphabetically
    local mind = math.min(adepth, bdepth)
    for i = 1, mind do
        if aa[i] ~= bb[i] then
            return aa[i] < bb[i]
        end
    end

    -- All shared directories match -> shorter directory path wins
    if adepth ~= bdepth then
        return adepth < bdepth
    end

    -- Same directory -> finally compare filenames
    return aa[#aa] < bb[#bb]
end

function fnv1a_32(str)
    local hash = 2166136261
    local prime = 16777619
    local uintMask = 0xFFFFFFFF

    for i = 1, #str do
        hash = ((hash ~ string.byte(str, i)))
        hash = ((hash * prime))
    end

    hash = ((hash ~ 0))
    hash = ((hash * prime) & uintMask)

    return hash
end

local binRootDir = profile.outputRoot
local stagingRootDir = binRootDir .. ".staging"
local backupRootDir = binRootDir .. ".previous"

local cacheFile = GetCacheFile(binRootDir, profileIdentity)
local sourceSets = {}
local files = {}
local inputFiles = {}
local inputFileSet = {}
local filePathExist = {}

local function AddInputFile(inputPath)
    if inputFileSet[inputPath] then return end
    inputFileSet[inputPath] = true
    table.insert(inputFiles, inputPath)
end

for _, provider in ipairs(profile.providers) do
    for _, source in ipairs(provider.sources) do
        if not os.isdir(source.root) then
            error("MetaGen provider '" .. provider.name .. "' definition root does not exist: " .. source.root, 0)
        end
        local sourceFiles = FindDefinitionFiles(source.root)
        table.sort(sourceFiles, PathCompareSort)
        table.insert(sourceSets, { files = sourceFiles, root = source.root, namespace = source.namespace })
        for _, sourcePath in ipairs(sourceFiles) do
            table.insert(files, sourcePath)
            AddInputFile(sourcePath)
        end
    end
    for _, moduleRoot in ipairs(provider.moduleRoots) do
        for _, modulePath in ipairs(os.matchfiles(moduleRoot .. "/**.lua")) do AddInputFile(modulePath) end
    end
    for _, extensionPath in ipairs(provider.extensions) do
        if GetFileHash(extensionPath) == nil then error("MetaGen provider '" .. provider.name .. "' extension does not exist: " .. extensionPath, 0) end
        AddInputFile(extensionPath)
    end
    for _, inputPath in ipairs(provider.inputs) do
        if GetFileHash(inputPath) == nil then error("MetaGen provider '" .. provider.name .. "' input does not exist: " .. inputPath, 0) end
        AddInputFile(inputPath)
    end
end

for _, path in ipairs(os.matchfiles("*.lua")) do
    AddInputFile(path)
end
if profile.postgres ~= nil and profile.postgres.historyByBundle ~= nil then
    for _, historyRoot in pairs(profile.postgres.historyByBundle) do
        for _, historyPath in ipairs(os.matchfiles(historyRoot .. "/**.lua")) do AddInputFile(historyPath) end
    end
end

table.sort(files, PathCompareSort)
table.sort(inputFiles, PathCompareSort)

local requiresRebuild = false
for _, path in ipairs(inputFiles) do
    -- Check if file is not found in the cache
    local cachedHash = cacheFile.files[path]
    if cachedHash == nil then
        requiresRebuild = true
        break
    end

    -- Check content hash stored
    local fileHash = GetFileHash(path)
    if fileHash ~= cachedHash then
        requiresRebuild = true
        break
    end

    filePathExist[path] = 1
end

for path, _ in pairs(cacheFile.files) do
    -- Check if cached file have been removed
    if filePathExist[path] == nil then
        requiresRebuild = true
        break
    end
end

if not requiresRebuild then
    for relativePath, cachedHash in pairs(cacheFile.outputs) do
        local outputHash = GetFileHash(binRootDir .. "/" .. relativePath)
        if outputHash ~= cachedHash then
            requiresRebuild = true
            break
        end
    end
end

if not requiresRebuild then
    print("MetaGen : No changes detected, skipping")
    return
else
    print("MetaGen : Building Files")
end

Backend.Register("cpp", CppBackend)
Backend.Register("postgres", PostgresBackend)
local definitionKinds = {
    enum = "Engine", struct = "Engine", clientDB = "Engine", packet = "Engine", luaEnum = "Engine",
    luaEvent = "Engine", netField = "Engine", gameCommand = "Engine", databaseTable = "Engine"
}

for _, provider in ipairs(profile.providers) do
    for _, extensionPath in ipairs(provider.extensions) do
        local chunk, loadError = loadfile(extensionPath)
        if chunk == nil then error("MetaGen failed to load extension '" .. extensionPath .. "': " .. tostring(loadError), 0) end
        local succeeded, extension = pcall(chunk)
        if not succeeded then error("MetaGen failed to execute extension '" .. extensionPath .. "': " .. tostring(extension), 0) end
        if type(extension) ~= "table" or type(extension.backends) ~= "table" then
            error("MetaGen extension '" .. extensionPath .. "' must return a table with a backends array", 0)
        end
        for _, kind in ipairs(extension.definitionKinds or {}) do
            if type(kind) ~= "string" or kind == "" then error("MetaGen extension declared an invalid definition kind", 0) end
            if definitionKinds[kind] ~= nil then
                error("MetaGen definition kind '" .. kind .. "' is already registered by " .. definitionKinds[kind], 0)
            end
            definitionKinds[kind] = extensionPath
        end
        for _, registration in ipairs(extension.backends) do
            if type(registration) ~= "table" or type(registration.name) ~= "string" or type(registration.backend) ~= "table" then
                error("MetaGen extension '" .. extensionPath .. "' contains an invalid backend registration", 0)
            end
            Backend.Register(registration.name, registration.backend)
        end
    end
end

local model = Model.LoadSourceSets(sourceSets)
for _, definition in ipairs(model.definitions) do
    if definitionKinds[definition.kind] == nil then
        error("MetaGen definition kind '" .. definition.kind .. "' is not registered (" .. definition.sourcePath .. ")", 0)
    end
end

local backendContext =
{
    sourceSets = sourceSets,
    stagingRootDir = stagingRootDir,
    binRootDir = binRootDir,
    namespaceBase = "MetaGen",
    pathSort = PathCompareSort,
    hash = fnv1a_32,
    writer = cppWriter,
    cpp = cppEmitter,
    postgresHistoryByBundle = profile.postgres and profile.postgres.historyByBundle or nil
}

Backend.ValidateAll(model, backendContext)
Model.AssertUnchanged(model, "validation")
local plannedOutputs = Backend.PlanOutputs(model, backendContext)
Model.AssertUnchanged(model, "output planning")

os.rmdir(stagingRootDir)
os.mkdir(stagingRootDir)

Backend.EmitAll(model, backendContext)
Model.AssertUnchanged(model, "emission")

-- Create Cache File
local outputFiles = {}
local plannedOutputSet = {}
for _, relativePath in ipairs(plannedOutputs) do
    local outputPath = stagingRootDir .. "/" .. relativePath
    if GetFileHash(outputPath) == nil then
        error("MetaGen backend did not emit planned output '" .. relativePath .. "'")
    end
    plannedOutputSet[relativePath] = true
    table.insert(outputFiles, outputPath)
end
table.sort(outputFiles, PathCompareSort)

for _, outputPath in ipairs(os.matchfiles(stagingRootDir .. "/**")) do
    local relativePath = outputPath:sub(#stagingRootDir + 2):gsub("\\", "/")
    if plannedOutputSet[relativePath] == nil then
        error("MetaGen backend emitted unplanned output '" .. relativePath .. "'")
    end
end

local filePath = stagingRootDir .. "/Cache.mcache"
WriteBinaryFile(filePath, function(file)
    local function WriteEntries(entries, root)
        file:write(string.pack("<I4", #entries))

        for _, path in ipairs(entries) do
            local entryPath = path
            if root ~= nil then
                entryPath = path:sub(#root + 2)
            end

            local pathLength = #entryPath
            local fileHash = assert(GetFileHash(path))

            file:write(string.pack("<I4", fileHash))
            file:write(string.pack("<I4", pathLength))
            file:write(entryPath)
        end
    end

    file:write(CACHE_MAGIC)
    file:write(string.pack("<I4", #profileIdentity))
    file:write(profileIdentity)
    WriteEntries(inputFiles)
    WriteEntries(outputFiles, stagingRootDir)
end)

-- Keep the last successful output intact until every new file has been generated.
os.rmdir(backupRootDir)

local hadPreviousOutput = os.isdir(binRootDir)
if hadPreviousOutput then
    local movedPreviousOutput, movePreviousError = os.rename(binRootDir, backupRootDir)
    if not movedPreviousOutput then
        error("MetaGen failed to preserve the previous output: " .. tostring(movePreviousError))
    end
end

local installedOutput, installError = os.rename(stagingRootDir, binRootDir)
if not installedOutput then
    if hadPreviousOutput then
        os.rename(backupRootDir, binRootDir)
    end

    error("MetaGen failed to install the generated output: " .. tostring(installError))
end

if hadPreviousOutput then
    os.rmdir(backupRootDir)
end

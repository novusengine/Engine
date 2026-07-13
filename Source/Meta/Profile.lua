MetaGen = MetaGen or {}

local providers = MetaGen._providers or {}
local project = MetaGen._project
MetaGen._providers = providers

local function RequireString(value, label)
    if type(value) ~= "string" or value == "" then
        error("MetaGen " .. label .. " must be a non-empty string", 0)
    end
end

local function CopyArray(values, label)
    if values == nil then return {} end
    if type(values) ~= "table" then error("MetaGen " .. label .. " must be an array", 0) end

    local result = {}
    for index, value in ipairs(values) do result[index] = value end
    return result
end

function MetaGen.RegisterProvider(options)
    if type(options) ~= "table" then error("MetaGen provider must be a table", 0) end
    RequireString(options.name, "provider name")
    if providers[options.name] ~= nil then
        error("MetaGen provider '" .. options.name .. "' is already registered", 0)
    end

    local sources = CopyArray(options.sources, "provider sources")
    if options.definitionRoot ~= nil then
        table.insert(sources, { root = options.definitionRoot, namespace = options.namespace })
    end
    if #sources == 0 then error("MetaGen provider '" .. options.name .. "' has no definition sources", 0) end

    for _, source in ipairs(sources) do
        if type(source) ~= "table" then error("MetaGen provider source must be a table", 0) end
        RequireString(source.root, "definition root")
        RequireString(source.namespace, "logical namespace")
    end
    table.sort(sources, function(a, b)
        local aKey = a.namespace .. "|" .. a.root:gsub("\\", "/"):lower()
        local bKey = b.namespace .. "|" .. b.root:gsub("\\", "/"):lower()
        return aKey < bKey
    end)

    local dependencies = CopyArray(options.dependencies, "provider dependencies")
    local moduleRoots = CopyArray(options.moduleRoots, "module roots")
    local extensions = CopyArray(options.extensions, "extensions")
    local inputs = CopyArray(options.inputs, "provider inputs")
    table.sort(dependencies)
    table.sort(moduleRoots)
    table.sort(extensions)
    table.sort(inputs)

    providers[options.name] =
    {
        name = options.name,
        sources = sources,
        moduleRoots = moduleRoots,
        extensions = extensions,
        inputs = inputs,
        dependencies = dependencies
    }
end

function MetaGen.ConfigureProject(options)
    if project ~= nil then error("MetaGen project is already configured", 0) end
    if type(options) ~= "table" then error("MetaGen project configuration must be a table", 0) end
    RequireString(options.name, "project name")
    RequireString(options.outputRoot, "project output root")

    project =
    {
        name = options.name,
        outputRoot = options.outputRoot,
        providers = CopyArray(options.providers, "project providers"),
        postgres = options.postgres
    }
    MetaGen._project = project
end

function MetaGen.ResolveProject()
    if project == nil then error("MetaGen project has not been configured", 0) end

    local resolved = {}
    local state = {}
    local function Visit(name, chain)
        RequireString(name, "provider subscription")
        if state[name] == 2 then return end
        if state[name] == 1 then error("MetaGen provider dependency cycle at '" .. name .. "' (" .. chain .. ")", 0) end

        local provider = providers[name]
        if provider == nil then error("MetaGen provider '" .. name .. "' is not registered", 0) end
        state[name] = 1

        local dependencies = CopyArray(provider.dependencies, "provider dependencies")
        table.sort(dependencies)
        for _, dependency in ipairs(dependencies) do Visit(dependency, chain .. " -> " .. dependency) end

        state[name] = 2
        table.insert(resolved, provider)
    end

    local subscriptions = CopyArray(project.providers, "project providers")
    table.sort(subscriptions)
    for _, name in ipairs(subscriptions) do Visit(name, name) end

    local sourceKeys = {}
    for _, provider in ipairs(resolved) do
        for _, source in ipairs(provider.sources) do
            local key = source.root:gsub("\\", "/"):lower() .. "|" .. source.namespace
            if sourceKeys[key] ~= nil then
                error("MetaGen providers '" .. sourceKeys[key] .. "' and '" .. provider.name ..
                    "' declare the same logical source", 0)
            end
            sourceKeys[key] = provider.name
        end
    end

    return { name = project.name, outputRoot = project.outputRoot, providers = resolved, postgres = project.postgres }
end

function MetaGen.GetGeneratedIncludeRoot()
    if project == nil then error("MetaGen project has not been configured", 0) end
    return path.getdirectory(project.outputRoot)
end

function MetaGen.RequestFinalization()
    if MetaGen._finalizationRequested then error("MetaGen finalization has already been requested", 0) end
    MetaGen._finalizationRequested = true
end

function MetaGen.Finalize(callback)
    if not MetaGen._finalizationRequested then error("MetaGen root project did not request finalization", 0) end
    if MetaGen._finalized then error("MetaGen generation has already been finalized", 0) end
    if type(callback) ~= "function" then error("MetaGen finalizer must be a function", 0) end
    MetaGen.ResolveProject()
    MetaGen._finalized = true
    callback()
end

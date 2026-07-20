local M = {}
local Model = require("Model")

local backends = {}

local function ActiveBackendNames(model)
    local names = {}
    for name, definitions in pairs(model.definitionsByTarget) do
        if #definitions > 0 then table.insert(names, name) end
    end
    table.sort(names)
    return names
end

function M.Register(name, backend)
    if type(name) ~= "string" or name == "" then
        error("MetaGen backend name must be a non-empty string")
    end
    if type(backend) ~= "table" then
        error("MetaGen backend '" .. name .. "' must be a table")
    end
    for _, operation in ipairs({ "Validate", "PlanOutputs", "Emit" }) do
        if type(backend[operation]) ~= "function" then
            error("MetaGen backend '" .. name .. "' must provide a " .. operation .. " function")
        end
    end
    if backends[name] ~= nil then
        error("MetaGen backend '" .. name .. "' is already registered")
    end

    backends[name] = backend
end

function M.ValidateTargets(model)
    local unsupported = {}
    for targetName, definitions in pairs(model.definitionsByTarget) do
        if backends[targetName] == nil and #definitions > 0 then
            table.insert(unsupported, targetName)
        end
    end
    table.sort(unsupported)

    if #unsupported > 0 then
        error("MetaGen has definitions for unsupported target '" .. unsupported[1] .. "'")
    end
end

function M.ValidateAll(model, context)
    M.ValidateTargets(model)
    for _, name in ipairs(ActiveBackendNames(model)) do
        backends[name].Validate(model, model.definitionsByTarget[name], context)
        Model.AssertUnchanged(model, "backend '" .. name .. "' validation")
    end
end

function M.PlanOutputs(model, context)
    local outputs = {}
    local owners = {}

    for _, name in ipairs(ActiveBackendNames(model)) do
        local backendOutputs = backends[name].PlanOutputs(model, model.definitionsByTarget[name], context)
        Model.AssertUnchanged(model, "backend '" .. name .. "' output planning")
        if type(backendOutputs) ~= "table" then
            error("MetaGen backend '" .. name .. "' PlanOutputs must return an array")
        end

        local outputCount = 0
        local maximumIndex = 0
        for key in pairs(backendOutputs) do
            if type(key) ~= "number" or key < 1 or key % 1 ~= 0 then
                error("MetaGen backend '" .. name .. "' PlanOutputs must return an array")
            end
            outputCount = outputCount + 1
            maximumIndex = math.max(maximumIndex, key)
        end
        if outputCount ~= maximumIndex then
            error("MetaGen backend '" .. name .. "' PlanOutputs must not contain holes")
        end

        for _, output in ipairs(backendOutputs) do
            if type(output) ~= "string" or output == "" then
                error("MetaGen backend '" .. name .. "' planned an invalid output path")
            end
            output = output:gsub("\\", "/")
            local hasParentTraversal = false
            for part in output:gmatch("[^/]+") do
                if part == ".." then hasParentTraversal = true end
            end
            if output:match("^/") or output:match("^[A-Za-z]:") or hasParentTraversal then
                error("MetaGen backend '" .. name .. "' planned unsafe output path '" .. output .. "'")
            end
            if owners[output] ~= nil then
                error("MetaGen output '" .. output .. "' is planned by both '" .. owners[output] .. "' and '" .. name .. "'")
            end
            owners[output] = name
            table.insert(outputs, output)
        end
    end

    table.sort(outputs)
    return outputs
end

function M.EmitAll(model, context)
    for _, name in ipairs(ActiveBackendNames(model)) do
        backends[name].Emit(model, model.definitionsByTarget[name], context)
        Model.AssertUnchanged(model, "backend '" .. name .. "' emission")
    end
end

function M.Get(name)
    return backends[name]
end

return M

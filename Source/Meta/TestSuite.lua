local M = {}
local suites = {}

function M.Register(name, callback)
    if type(name) ~= "string" or name == "" then error("MetaGen test suite name must be a non-empty string") end
    if type(callback) ~= "function" then error("MetaGen test suite '" .. name .. "' requires a callback") end
    if suites[name] ~= nil then error("MetaGen test suite '" .. name .. "' is already registered") end
    suites[name] = callback
end

function M.Run(name)
    local callback = suites[name]
    if callback == nil then error("Unknown MetaGen test suite '" .. tostring(name) .. "'") end
    callback()
end

function M.RunAll()
    local names = {}
    for name in pairs(suites) do names[#names + 1] = name end
    table.sort(names)
    for _, name in ipairs(names) do
        print("MetaGen test suite: " .. name)
        suites[name]()
    end
end

return M

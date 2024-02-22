local dependencies = { "base", "entt" }
local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", "NOMINMAX" }
ProjectTemplate("Network", "StaticLib", ".", Engine.binDir, dependencies, defines)

local function Include()
    local includeDir = path.getabsolute("Network/", Engine.projectsDir)
    AddIncludeDirs(includeDir)
    AddLinks("Network")

    filter "platforms:Win64"
        AddDefines("NOMINMAX")
end
CreateDep("network", Include, dependencies)
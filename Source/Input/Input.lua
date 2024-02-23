local dependencies = { "base", "imgui" }
local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }
ProjectTemplate("Input", "StaticLib", ".", Engine.binDir, dependencies, defines)

local function Include()
    local includeDir = path.getabsolute("Input/", Engine.projectsDir)
    AddIncludeDirs(includeDir)
    AddLinks("Input")
end
CreateDep("input", Include, dependencies)
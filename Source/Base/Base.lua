local dependencies = { "glm", "spdlog", "robinhood", "json", "refl-cpp" }
local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", "WIN32_LEAN_AND_MEAN" }
ProjectTemplate("Base", "StaticLib", ".", Engine.binDir, dependencies, defines)

local function Include()
    local includeDir = path.getabsolute("Base/", Engine.projectsDir)
    AddIncludeDirs(includeDir)
    AddLinks("Base")
    AddDefines("WIN32_LEAN_AND_MEAN")
end
CreateDep("base", Include, dependencies)
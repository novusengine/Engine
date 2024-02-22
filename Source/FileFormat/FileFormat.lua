local dependencies = { "base" }
local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }
ProjectTemplate("FileFormat", "StaticLib", ".", Engine.binDir, dependencies, defines)

local function Include()
    local includeDir = path.getabsolute("FileFormat/", Engine.projectsDir)
    AddIncludeDirs(includeDir)
    AddLinks("FileFormat")
end
CreateDep("fileformat", Include, dependencies)
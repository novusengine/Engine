local dependencies = { "base", "dxcompiler" }
local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", "NOMINMAX" }
ProjectTemplate("ShaderCooker", "StaticLib", ".", Engine.binDir, dependencies, defines)

local function Include()
    local includeDir = path.getabsolute("ShaderCooker/", Engine.projectsDir)
    AddIncludeDirs(includeDir)
    AddLinks("ShaderCooker")
end
CreateDep("shadercooker", Include, dependencies)
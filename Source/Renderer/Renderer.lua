local dependencies = { "base", "shadercooker", "tracy", "imgui", "gli", "typesafe" }
local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", "NOMINMAX", "NOMINMAX" }
ProjectTemplate("Renderer", "StaticLib", ".", Engine.binDir, dependencies, defines)

local function Include()
    local includeDir = path.getabsolute("Renderer/", Engine.projectsDir)
    AddIncludeDirs(includeDir)
    AddLinks("Renderer")
end
CreateDep("renderer", Include, dependencies)
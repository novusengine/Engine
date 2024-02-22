local dependencies = { }
local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }
ProjectTemplate("Catch2", "StaticLib", path.getabsolute("catch2/", Engine.dependencyDir), Engine.binDir, dependencies, defines)

local function IncludeCatch2()
    local includePath = path.getabsolute("catch2/", Engine.dependencyDir);
    AddIncludeDirs(includePath)
    AddLinks("Catch2")
end
CreateDep("catch2", IncludeCatch2, dependencies)

local function IncludeCatch2WithMain()
    local files =
    {
        path.getabsolute("catch2/**.h", Engine.dependencyDir),
        path.getabsolute("catch2/**.cpp", Engine.dependencyDir)
    }
    AddFiles(files)

    local includePath = path.getabsolute("catch2/", Engine.dependencyDir);
    AddIncludeDirs(includePath)

    AddDefines("DO_NOT_USE_WMAIN")
end
CreateDep("catch2-withmain", IncludeCatch2WithMain)
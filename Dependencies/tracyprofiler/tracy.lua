local isEnabled = BuildSettings:Get("Enable Tracy")

local function SetupLib()
    local basePath = path.getabsolute("tracyprofiler/", Engine.dependencyDir)
    local dependencies = { }
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    ProjectTemplate("Tracy", "StaticLib", nil, Engine.binDir, dependencies, defines)

    local sourceDir = path.getabsolute("tracy/", basePath)
    local includeDir = { basePath }

    local files =
    {
        path.getabsolute("tracy/Tracy.hpp", sourceDir),
        path.getabsolute("tracy/TracyVulkan.hpp", sourceDir),
        path.getabsolute("TracyClient.cpp", sourceDir)
    }
    AddFiles(files)
    AddIncludeDirs(includeDir)

    if isEnabled then
        AddDefines("TRACY_ENABLE")
    end
end
SetupLib()

local function Include()
    local basePath = path.getabsolute("tracyprofiler/", Engine.dependencyDir)
    local sourcePath = path.getabsolute("tracy/", basePath)
    local includeDir = { basePath, sourcePath }
    AddIncludeDirs(includeDir)

    if isEnabled then
        AddDefines("TRACY_ENABLE")
    end

    AddLinks("Tracy")
end
CreateDep("tracy", Include)
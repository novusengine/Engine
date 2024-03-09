local taskPriorities = BuildSettings:Get("EnkiTS Num Task Priorities")

local function SetupLib()
    local basePath = path.getabsolute("enkiTS/", Engine.dependencyDir)
    local dependencies = { }
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    ProjectTemplate("EnkiTS", "StaticLib", nil, Engine.binDir, dependencies, defines)

    local sourceDir = path.getabsolute("enkiTS/", basePath)
    local includeDir = sourceDir
    local files =
    {
        sourceDir .. "/LockLessMultiReadPipe.h",
        sourceDir .. "/TaskScheduler.h",
        sourceDir .. "/TaskScheduler.cpp"
    }
    AddFiles(files)

    AddIncludeDirs(includeDir)

    AddDefines("ENKITS_TASK_PRIORITIES_NUM=" .. tostring(taskPriorities))
end
SetupLib()

local function Include()
    local includeDir = path.getabsolute("enkiTS/", Engine.dependencyDir)
    AddIncludeDirs(includeDir)

    AddLinks("EnkiTS")
    filter "platforms:Linux"
        AddLinks("pthread")
end
CreateDep("enkiTS", Include)
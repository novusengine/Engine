local function SetupLib()
    local basePath = path.getabsolute("spdlog/", Engine.dependencyDir)
    local dependencies = { }
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS", "SPDLOG_COMPILED_LIB" }

    ProjectTemplate("Spdlog", "StaticLib", nil, Engine.binDir, dependencies, defines)

    local sourceDir = path.getabsolute("src/", basePath)
    local includeDir = path.getabsolute("include/", basePath)

    local files =
    {
        (includeDir .. "/**.h"),
        (includeDir .. "/**.hpp"),

        (sourceDir .. "/**.c"),
        (sourceDir .. "/**.cpp")
    }
    AddFiles(files)
    AddIncludeDirs(includeDir)

    filter "platforms:Linux"
        AddLinks("pthread")
end
SetupLib()

local function Include()
    local includeDir =  path.getabsolute("spdlog/include/", Engine.dependencyDir)
    AddIncludeDirs(includeDir)

    AddLinks("Spdlog")
end
CreateDep("spdlog", Include)
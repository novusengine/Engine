local warningsAsErrors = BuildSettings:Get("Luau Warnings as Errors")

local basePath = path.getabsolute("luau/Luau", Engine.dependencyDir)

local Luau = { }
Luau.SetupCommon = function()
    local callback = function()
        local includeDir = path.getabsolute("Common/include", basePath)
        AddIncludeDirs(includeDir)
    end
    CreateDep("luau-common", callback)
end
Luau.SetupAst = function()
    local dependencies = { "luau-common" }
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    ProjectTemplate("Luau-Ast", "StaticLib", nil, Engine.binDir, dependencies, defines)

    local sourceDir = path.getabsolute("Ast/src", basePath)
    local includeDir = path.getabsolute("Ast/include", basePath)

    local files =
    {
        includeDir .. "/**.h",
        sourceDir .. "/**.c",
        sourceDir .. "/**.cpp"
    }
    AddFiles(files)
    AddIncludeDirs(includeDir)

    local callback = function()
        AddIncludeDirs(includeDir)

        AddLinks("Luau-Ast")
    end

    CreateDep("luau-ast", callback, dependencies)
end
Luau.SetupCompiler = function()
    local dependencies = { "luau-ast" }
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    ProjectTemplate("Luau-Compiler", "StaticLib", nil, Engine.binDir, dependencies, defines)

    local sourceDir = path.getabsolute("Compiler/src", basePath)
    local includeDir = path.getabsolute("Compiler/include", basePath)

    local files =
    {
        includeDir .. "/**.h",
        sourceDir .. "/**.c",
        sourceDir .. "/**.cpp"
    }
    AddFiles(files)
    AddIncludeDirs(includeDir)

    local callback = function()
        AddIncludeDirs(includeDir)

        AddLinks("Luau-Compiler")
    end

    CreateDep("luau-compiler", callback, dependencies)
end
Luau.SetupConfig = function()
    local dependencies = { "luau-ast" }
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    ProjectTemplate("Luau-Config", "StaticLib", nil, Engine.binDir, dependencies, defines)

    local sourceDir = path.getabsolute("Config/src", basePath)
    local includeDir = path.getabsolute("Config/include", basePath)

    local files =
    {
        includeDir .. "/**.h",
        sourceDir .. "/**.c",
        sourceDir .. "/**.cpp"
    }
    AddFiles(files)
    AddIncludeDirs(includeDir)

    local callback = function()
        AddIncludeDirs(includeDir)

        AddLinks("Luau-Config")
    end

    CreateDep("luau-config", callback, dependencies)
end
Luau.SetupAnalysis = function()
    local dependencies = { "luau-ast", "luau-config" }
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    ProjectTemplate("Luau-Analysis", "StaticLib", nil, Engine.binDir, dependencies, defines)

    local sourceDir = path.getabsolute("Analysis/src", basePath)
    local includeDir = path.getabsolute("Analysis/include", basePath)

    local files =
    {
        includeDir .. "/**.h",
        sourceDir .. "/**.c",
        sourceDir .. "/**.cpp"
    }
    AddFiles(files)
    AddIncludeDirs(includeDir)

    filter "platforms:Win64"
        AddDefines("_CRT_SECURE_NO_WARNINGS")

    local callback = function()
        AddIncludeDirs(includeDir)

        AddLinks("Luau-Analysis")
    end

    CreateDep("luau-analysis", callback, dependencies)
end
Luau.SetupVM = function()
    local dependencies = { "luau-common" }
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    ProjectTemplate("Luau-VM", "StaticLib", nil, Engine.binDir, dependencies, defines)

    local sourceDir = path.getabsolute("VM/src", basePath)
    local includeDir = path.getabsolute("VM/include", basePath)

    local files =
    {
        includeDir .. "/**.h",
        sourceDir .. "/**.c",
        sourceDir .. "/**.cpp"
    }
    AddFiles(files)
    AddIncludeDirs(includeDir)

    filter "platforms:Win64"
        AddDefines("_CRT_SECURE_NO_WARNINGS")

    local callback = function()
        AddIncludeDirs(includeDir)

        AddLinks("Luau-VM")
    end

    CreateDep("luau-vm", callback, dependencies)
end
Luau.SetupCodeGen = function()
    local dependencies = { "luau-common", "luau-vm" }
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    ProjectTemplate("Luau-CodeGen", "StaticLib", nil, Engine.binDir, dependencies, defines)

    local sourceDir = path.getabsolute("CodeGen/src", basePath)
    local includeDir = path.getabsolute("CodeGen/include", basePath)
    local vmSourceDir = path.getabsolute("VM/src", basePath)

    local files =
    {
        includeDir .. "/**.h",
        sourceDir .. "/**.c",
        sourceDir .. "/**.cpp"
    }
    AddFiles(files)
    AddIncludeDirs({ includeDir, vmSourceDir })

    local callback = function()
        AddIncludeDirs(includeDir)

        AddLinks("Luau-CodeGen")
    end

    CreateDep("luau-codegen", callback, dependencies)
end
Luau.SetupIsocline = function()
    local dependencies = { }
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    ProjectTemplate("Luau-Isocline", "StaticLib", nil, Engine.binDir, dependencies, defines)

    local sourceDir = path.getabsolute("extern/isocline/src", basePath)
    local includeDir = path.getabsolute("extern/isocline/include", basePath)

    local files =
    {
        includeDir .. "/isocline.h",
        sourceDir .. "/isocline.c",
    }
    AddFiles(files)
    AddIncludeDirs(includeDir)

    local callback = function()
        AddIncludeDirs(includeDir)

        AddLinks("Luau-Isocline")
    end

    CreateDep("luau-isocline", callback, dependencies)
end

local luauDependencyGroup = Engine.dependencyGroup .. "/Luau"
group (luauDependencyGroup)
Luau.SetupCommon()
Luau.SetupAst()
Luau.SetupCompiler()
Luau.SetupConfig()
Luau.SetupAnalysis()
Luau.SetupVM()
Luau.SetupCodeGen()
Luau.SetupIsocline()
group (Engine.dependencyGroup)
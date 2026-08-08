local warningsAsErrors = BuildSettings:Get("Luau Warnings as Errors")
local basePath = path.getabsolute("luau/Luau", Solution.Projects.Current.DependencyDir)

local function SetWarningsAsErrors()
    if warningsAsErrors then
        fatalwarnings({ "All" })
    end
end

local Luau = { }
Luau.SetupCommon = function()
    local dep = Solution.Util.CreateDepTable("Luau-Common", {})
    local sourceDir = basePath .. "/Common/src"
    local includeDir = basePath .. "/Common/include"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(17)
        SetWarningsAsErrors()

        local files =
        {
            includeDir .. "/**.h",
            sourceDir .. "/**.c",
            sourceDir .. "/**.cpp"
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetDefines(defines)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetLinks(dep.Name)
    end)
end
Luau.SetupAst = function()
    local dep = Solution.Util.CreateDepTable("Luau-Ast", { "luau-common" })
    local sourceDir = basePath .. "/Ast/src"
    local includeDir = basePath .. "/Ast/include"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(17)
        SetWarningsAsErrors()

        local files =
        {
            includeDir .. "/**.h",
            sourceDir .. "/**.c",
            sourceDir .. "/**.cpp"
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetDefines(defines)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetLinks(dep.Name)
    end)
end
Luau.SetupBytecode = function()
    local dep = Solution.Util.CreateDepTable("Luau-Bytecode", { "luau-common" })
    local sourceDir = basePath .. "/Bytecode/src"
    local includeDir = basePath .. "/Bytecode/include"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(17)
        SetWarningsAsErrors()

        local files =
        {
            includeDir .. "/**.h",
            sourceDir .. "/**.h",
            sourceDir .. "/**.c",
            sourceDir .. "/**.cpp"
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetDefines(defines)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetLinks(dep.Name)
    end)
end
Luau.SetupCompiler = function()
    local dep = Solution.Util.CreateDepTable("Luau-Compiler", { "luau-ast", "luau-bytecode" })
    local sourceDir = basePath .. "/Compiler/src"
    local includeDir = basePath .. "/Compiler/include"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(17)
        SetWarningsAsErrors()

        local files =
        {
            includeDir .. "/**.h",
            sourceDir .. "/**.c",
            sourceDir .. "/**.cpp"
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetDefines(defines)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetLinks(dep.Name)
    end)
end
Luau.SetupConfig = function()
    local dep = Solution.Util.CreateDepTable("Luau-Config", { "luau-ast", "luau-compiler", "luau-vm" })
    local sourceDir = basePath .. "/Config/src"
    local includeDir = basePath .. "/Config/include"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(17)
        SetWarningsAsErrors()

        local files =
        {
            includeDir .. "/**.h",
            sourceDir .. "/**.c",
            sourceDir .. "/**.cpp"
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetDefines(defines)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetLinks(dep.Name)
    end)
end
Luau.SetupInliner = function()
    local dep = Solution.Util.CreateDepTable("Luau-Inliner", { "luau-bytecode", "luau-vm" })
    local sourceDir = basePath .. "/Inliner/src"
    local includeDir = basePath .. "/Inliner/include"
    local bytecodeSourceDir = basePath .. "/Bytecode/src"
    local vmSourceDir = basePath .. "/VM/src"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(17)
        SetWarningsAsErrors()

        local files =
        {
            includeDir .. "/**.h",
            sourceDir .. "/**.h",
            sourceDir .. "/**.c",
            sourceDir .. "/**.cpp"
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetIncludes({ includeDir, bytecodeSourceDir, vmSourceDir })
        Solution.Util.SetDefines(defines)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetLinks(dep.Name)
    end)
end
Luau.SetupVM = function()
    local dep = Solution.Util.CreateDepTable("Luau-VM", { "luau-common" })
    local sourceDir = basePath .. "/VM/src"
    local includeDir = basePath .. "/VM/include"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(11)
        SetWarningsAsErrors()

        local files =
        {
            includeDir .. "/**.h",
            sourceDir .. "/**.c",
            sourceDir .. "/**.cpp"
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetDefines(defines)

        Solution.Util.SetFilter("platforms:Win64", function()
            Solution.Util.SetDefines({ "_CRT_SECURE_NO_WARNINGS" })
        end)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetLinks(dep.Name)
    end)
end
Luau.SetupAnalysis = function()
    local dep = Solution.Util.CreateDepTable("Luau-Analysis", { "luau-ast", "luau-config", "luau-compiler", "luau-vm" })
    local sourceDir = basePath .. "/Analysis/src"
    local includeDir = basePath .. "/Analysis/include"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(17)
        SetWarningsAsErrors()

        local files =
        {
            includeDir .. "/**.h",
            sourceDir .. "/**.c",
            sourceDir .. "/**.cpp"
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetDefines(defines)

        Solution.Util.SetFilter("platforms:Win64", function()
            Solution.Util.SetDefines({ "_CRT_SECURE_NO_WARNINGS" })
        end)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetLinks(dep.Name)
    end)
end
Luau.SetupCodeGen = function()
    local dep = Solution.Util.CreateDepTable("Luau-CodeGen", { "luau-common", "luau-vm" })
    local sourceDir = basePath .. "/CodeGen/src"
    local includeDir = basePath .. "/CodeGen/include"
    local vmSourceDir = basePath .. "/VM/src"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(17)
        SetWarningsAsErrors()

        local files =
        {
            includeDir .. "/**.h",
            sourceDir .. "/**.c",
            sourceDir .. "/**.cpp"
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetIncludes({ includeDir, vmSourceDir })
        Solution.Util.SetDefines(defines)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetLinks(dep.Name)
    end)
end
Luau.SetupRequire = function()
    local dep = Solution.Util.CreateDepTable("Luau-Require", { "luau-config", "luau-vm" })
    local sourceDir = basePath .. "/Require/src"
    local includeDir = basePath .. "/Require/include"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

        Solution.Util.SetLanguage("C++")
        Solution.Util.SetCppDialect(17)
        SetWarningsAsErrors()

        local files =
        {
            includeDir .. "/**.h",
            sourceDir .. "/**.h",
            sourceDir .. "/**.c",
            sourceDir .. "/**.cpp"
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetDefines(defines)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetLinks(dep.Name)
    end)
end
Luau.SetupIsocline = function()
    local dep = Solution.Util.CreateDepTable("Luau-Isocline", { "luau-common" })
    local sourceDir = basePath .. "/extern/isocline/src"
    local includeDir = basePath .. "/extern/isocline/include"

    Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
        Solution.Util.SetLanguage("C")
        SetWarningsAsErrors()

        local files =
        {
            includeDir .. "/isocline.h",
            sourceDir .. "/isocline.c"
        }
        Solution.Util.SetFiles(files)
        Solution.Util.SetIncludes(includeDir)

        Solution.Util.SetFilter("platforms:Win64", function()
            Solution.Util.SetDefines({ "_CRT_SECURE_NO_WARNINGS" })
        end)
    end)

    Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
        Solution.Util.SetIncludes(includeDir)
        Solution.Util.SetLinks(dep.Name)
    end)
end

Solution.Util.SetGroup(Solution.DependencyGroup .. "/Luau")
Luau.SetupCommon()
Luau.SetupAst()
Luau.SetupBytecode()
Luau.SetupVM()
Luau.SetupInliner()
Luau.SetupCompiler()
Luau.SetupConfig()
Luau.SetupAnalysis()
Luau.SetupCodeGen()
Luau.SetupRequire()
Luau.SetupIsocline()
Solution.Util.SetGroup(Solution.DependencyGroup)

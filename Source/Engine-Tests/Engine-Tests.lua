if BuildSettings:Get("Build UnitTests") == false then return end

local mod = Solution.Util.CreateModuleTable("Engine-Tests", { "base", "fileformat", "imgui", "catch2", "catch2-withmain" })

Solution.Util.CreateConsoleApp(mod.Name, Solution.Projects.Current.BinDir, mod.Dependencies, function()
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    Solution.Util.SetLanguage("C++")
    Solution.Util.SetCppDialect(20)

    local files = Solution.Util.GetFilesForCpp(mod.Path)
    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(mod.Path)
    Solution.Util.SetDefines(defines)
    
    vpaths { ["Tests"] = "**" }
end)
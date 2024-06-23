local mod = Solution.Util.CreateModuleTable("Base", { "glm", "robinhood", "json", "refl-cpp", "quill" })

Solution.Util.CreateStaticLib(mod.Name, Solution.Projects.Current.BinDir, mod.Dependencies, function()
    local defines = { "_CRT_SECURE_NO_WARNINGS", "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    Solution.Util.SetLanguage("C++")
    Solution.Util.SetCppDialect(20)

    local files = Solution.Util.GetFilesForCpp(mod.Path)
    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(mod.Path)
    Solution.Util.SetDefines(defines)

    Solution.Util.SetFilter("platforms:Win64", function()
        Solution.Util.SetDefines({"WIN32_LEAN_AND_MEAN", "NOMINMAX"})
    end)
end)

Solution.Util.CreateDep(mod.NameLow, mod.Dependencies, function()
    Solution.Util.SetIncludes(mod.Path)
    Solution.Util.SetLinks(mod.Name)
    
    Solution.Util.SetFilter("platforms:Win64", function()
        Solution.Util.SetDefines({"WIN32_LEAN_AND_MEAN", "NOMINMAX"})
    end)
end)
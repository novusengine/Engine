local mod = Solution.Util.CreateModuleTable("Renderer", { "base", "shadercooker", "tracyprofiler", "imgui", "gli", "typesafe", "utfcpp", "msdf-atlas-gen" })

Solution.Util.CreateStaticLib(mod.Name, Solution.Projects.Current.BinDir, mod.Dependencies, function()
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

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
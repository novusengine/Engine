local dep = Solution.Util.CreateDepTable("Catch2", {})

Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    Solution.Util.SetLanguage("C++")
    Solution.Util.SetCppDialect(20)

    local files = Solution.Util.GetFilesForCpp(dep.Path)
    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(dep.Path)
    Solution.Util.SetDefines(defines)
end)

Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
    Solution.Util.SetIncludes(dep.Path)
    Solution.Util.SetLinks(dep.Name)
end)

Solution.Util.CreateDep(dep.NameLow .. "-withmain", dep.Dependencies, function()
    local files = Solution.Util.GetFilesForCpp(dep.Path)
    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(dep.Path)
    Solution.Util.SetLinks(dep.Name)
    Solution.Util.SetDefines({ "DO_NOT_USE_WMAIN" })
end)
local dep = Solution.Util.CreateDepTable("spake2-ee", { "libsodium" })

Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    Solution.Util.SetLanguage("C++")
    Solution.Util.SetCppDialect(17)

    local files =
    {
        dep.Path .. "/**.h",
        dep.Path .. "/**.c",
        dep.Path .. "/**.cpp"
    }
    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(dep.Path)
    Solution.Util.SetDefines(defines)
end)
    
Solution.Util.CreateDep(dep.Name, dep.Dependencies, function()
    Solution.Util.SetIncludes(dep.Path)
    Solution.Util.SetLinks(dep.Name)
end)
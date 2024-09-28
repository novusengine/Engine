local dep = Solution.Util.CreateDepTable("stb_vorbis", {})

Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    Solution.Util.SetLanguage("C++")
    Solution.Util.SetCppDialect(20)

    local sourceDir = dep.Path .. "/stb_vorbis"
    local includeDirs = { dep.Path, sourceDir }
    local files =
    {
        sourceDir .. "/stb_vorbis.h",
        sourceDir .. "/stb_vorbis.c"
    }
    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(includeDirs)
    Solution.Util.SetDefines(defines)
end)

Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
    Solution.Util.SetIncludes(dep.Path)
    Solution.Util.SetLinks(dep.Name)
end)
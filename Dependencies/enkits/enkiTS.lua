local dep = Solution.Util.CreateDepTable("EnkiTS", {})

Solution.Util.CreateStaticLib(dep.Name, Solution.Projects.Current.BinDir, dep.Dependencies, function()
    local defines = { "_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS", "_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS" }

    Solution.Util.SetLanguage("C++")
    Solution.Util.SetCppDialect(20)

    local sourceDir = dep.Path .. "/enkiTS"
    local files =
    {
        sourceDir .. "/LockLessMultiReadPipe.h",
        sourceDir .. "/TaskScheduler.h",
        sourceDir .. "/TaskScheduler.cpp"
    }
    Solution.Util.SetFiles(files)
    Solution.Util.SetIncludes(dep.Path)
    Solution.Util.SetDefines(defines)

    local taskPriorities = BuildSettings:Get("EnkiTS Num Task Priorities")
    Solution.Util.SetDefines({ "ENKITS_TASK_PRIORITIES_NUM=" .. tostring(taskPriorities) })
end)

Solution.Util.CreateDep(dep.NameLow, dep.Dependencies, function()
    Solution.Util.SetIncludes(dep.Path)
    Solution.Util.SetLinks(dep.Name)

    Solution.Util.SetFilter("platforms:Linux", function()
        Solution.Util.SetLinks("pthread")
    end)
end)
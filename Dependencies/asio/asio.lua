local dep = Solution.Util.CreateDepTable("asio", {})

Solution.Util.CreateDep(dep.Name, dep.Dependencies, function()
    Solution.Util.SetIncludes(dep.Path)
    Solution.Util.SetDefines({"ASIO_STANDALONE"})
end)
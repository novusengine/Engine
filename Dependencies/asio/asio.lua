local dep = Solution.Util.CreateDepTable("asio", {})

Solution.Util.CreateDep(dep.Name, dep.Dependencies, function()
    Solution.Util.SetIncludes({dep.Path, dep.Path .. "/" .. dep.NameLow})
    Solution.Util.SetDefines({"ASIO_STANDALONE"})
end)
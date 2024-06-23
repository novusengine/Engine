local dep = Solution.Util.CreateDepTable("utfcpp", {})

Solution.Util.CreateDep(dep.Name, dep.Dependencies, function()
    Solution.Util.SetIncludes(dep.Path)
end)
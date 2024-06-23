local dep = Solution.Util.CreateDepTable("json", {})

Solution.Util.CreateDep(dep.Name, dep.Dependencies, function()
    Solution.Util.SetIncludes(dep.Path)
end)
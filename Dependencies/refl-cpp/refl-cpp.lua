local dep = Solution.Util.CreateDepTable("refl-cpp", {})

Solution.Util.CreateDep(dep.Name, dep.Dependencies, function()
    Solution.Util.SetIncludes(dep.Path)
end)